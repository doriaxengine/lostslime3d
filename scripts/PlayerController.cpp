#include "PlayerController.h"

#include "GameState.h"
#include "Collectible.h"
#include "ExitPortal.h"
#include "Hazard.h"

#include "Angle.h"
#include "Animation.h"
#include "Body3D.h"
#include "Body3DComponent.h"
#include "Camera.h"
#include "Contact3D.h"
#include "Engine.h"
#include "Input.h"
#include "Model.h"
#include "ModelComponent.h"
#include "Object.h"
#include "PhysicsSystem.h"

#include <algorithm>
#include <cmath>

using namespace doriax;

static const float DEADZONE = 0.2f;
static const float GROUND_NORMAL_Y = 0.64f; // slopes up to ~50 degrees
static const float LEAVING_SPEED = 1.0f; // away from the ground, like a jump taking off
static const float LANDING_SPEED = 2.0f; // slower falls land quietly

PlayerController::PlayerController(Scene* scene, Entity entity): ScriptBase(scene, entity) {
    physics = scene->getSystem<PhysicsSystem>().get();

    REGISTER_ENGINE_EVENT(onUpdate);
    if (physics) {
        REGISTER_EVENT(physics->onContactAdded3D, onContactAdded);
        REGISTER_EVENT(physics->onContactPersisted3D, onContactPersisted);
    }
}

PlayerController::~PlayerController() {
    UNREGISTER_ENGINE_EVENT(onUpdate);
    if (physics) {
        UNREGISTER_EVENT(physics->onContactAdded3D, onContactAdded);
        UNREGISTER_EVENT(physics->onContactPersisted3D, onContactPersisted);
    }
}

void PlayerController::play(const std::string& name, bool loop) {
    if (currentAnim == name) return;

    // still loading
    ModelComponent* component = scene->findComponent<ModelComponent>(entity);
    if (!component || component->animations.empty()) return;

    Model model(scene, entity);
    model.findAnimation(name).setLoop(loop);
    model.playAnimation(name, 0.12f);
    currentAnim = name;
}

// the contact normal points from A to B
void PlayerController::checkGround(Body3D& other, const Vector3& normal, bool isA) {
    Vector3 up = isA ? -normal : normal;
    if (up.y < GROUND_NORMAL_Y) return;

    // the step a jump leaves the ground still touches it
    Vector3 relative = Body3D(scene, entity).getLinearVelocity() - other.getLinearVelocity();
    if (relative.dotProduct(up) > LEAVING_SPEED) return;

    grounded = true;
    coyoteTimer = coyoteTime;

    Body3DComponent* body = scene->findComponent<Body3DComponent>(other.getEntity());
    if (body && body->type == BodyType::KINEMATIC) {
        groundVelocity = other.getLinearVelocity();
        onMovingGround = true;
    }
}

void PlayerController::onContactAdded(Body3D bodyA, Body3D bodyB, Contact3D contact) {
    bool isA = bodyA.getEntity() == entity;
    if (!isA && bodyB.getEntity() != entity) return;

    Body3D other = isA ? bodyB : bodyA;
    Body3DComponent* otherBody = scene->findComponent<Body3DComponent>(other.getEntity());
    if (otherBody && otherBody->sensor) {
        touch(other.getEntity());
        return;
    }

    checkGround(other, contact.getWorldSpaceNormal(), isA);
}

void PlayerController::onContactPersisted(Body3D bodyA, Body3D bodyB, Contact3D contact) {
    bool isA = bodyA.getEntity() == entity;
    if (!isA && bodyB.getEntity() != entity) return;

    Body3D other = isA ? bodyB : bodyA;
    Body3DComponent* otherBody = scene->findComponent<Body3DComponent>(other.getEntity());
    if (otherBody && otherBody->sensor) {
        // the portal can open with the slime already inside
        ExitPortal* portal = findScript<ExitPortal>(scene, other.getEntity(), "ExitPortal");
        if (portal && portal->isOpen()) portal->enter();
        return;
    }

    checkGround(other, contact.getWorldSpaceNormal(), isA);
}

// called during the physics step, so no body is moved here
void PlayerController::touch(Entity other) {
    if (dead) return;

    if (Collectible* item = findScript<Collectible>(scene, other, "Collectible")) {
        item->collect();
    } else if (findScript<Hazard>(scene, other, "Hazard")) {
        hurt();
    } else if (ExitPortal* portal = findScript<ExitPortal>(scene, other, "ExitPortal")) {
        portal->enter();
    }
}

void PlayerController::hurt() {
    if (dead || hurtTimer > 0.0f) return;
    hurtTimer = 1.0f;

    playSound(scene, "Hurt Sound");
    GameState::lives--;
    if (GameState::lives <= 0) {
        dead = true;
        GameState::gameOverRequested = true;
    } else {
        GameState::respawnRequested = true;
    }
}

void PlayerController::respawn(Vector3 position) {
    Body3D body(scene, entity);
    body.setLinearVelocity(Vector3::ZERO);
    body.setAngularVelocity(Vector3::ZERO);
    body.setGravityFactor(1.0f);
    body.setPosition(position);

    dead = false;
    frozen = false;
    grounded = false;
    onMovingGround = false;
    coyoteTimer = 0.0f;
    jumpBufferTimer = 0.0f;
    jumpCutPending = false;
    hurtTimer = 1.0f;
    cameraFocusValid = false;
    currentAnim.clear();
}

void PlayerController::updateCamera(float dt) {
    Entity cameraEntity = scene->getCamera();
    if (cameraEntity == NULL_ENTITY) return;

    // the drawn position, smoothed so landings don't shake the view
    Vector3 focus = Object(scene, entity).getWorldPosition();
    if (!cameraFocusValid) {
        cameraFocus = focus;
        cameraFocusValid = true;
    } else {
        cameraFocus = cameraFocus + (focus - cameraFocus) * std::min(cameraLag * dt, 1.0f);
    }

    Quaternion rotation;
    rotation.fromEulerAngles(pitch, yaw, 0.0f, RotationOrder::YXZ);
    Vector3 forward = rotation * Vector3(0, 0, -1);
    Vector3 target = cameraFocus + Vector3(0, lookHeight, 0);

    Camera camera(scene, cameraEntity);
    camera.setPosition(target - forward * cameraDistance);
    camera.setTarget(target);
}

void PlayerController::emitDust() {
    if (!dust) return;
    dust->reset();
    dust->start();
}

void PlayerController::updateAnimation(bool moving, bool sprinting) {
    if (dead) {
        play("die", false);
    } else if (!grounded) {
        play(Body3D(scene, entity).getLinearVelocity().y > 0.5f ? "jump" : "fall", false);
    } else if (moving) {
        play(sprinting ? "sprint" : "walk", true);
    } else {
        play("idle", true);
    }
}

void PlayerController::onUpdate() {
    if (GameState::paused) return;

    float dt = Engine::getDeltatime();
    if (dt <= 0.0f) return;

    if (!started) {
        started = true;
        yaw = cameraYaw;
        pitch = cameraPitch;
        // a sleeping body reports no ground contacts
        Body3D(scene, entity).setAllowSleeping(false);
    }

    Body3D body(scene, entity);
    if (hurtTimer > 0.0f) hurtTimer -= dt;

    int pad = Input::numGamepads() > 0 ? Input::getGamepadId(0) : -1;
    bool hasPad = pad != -1 && Input::isGamepadConnected(pad);

    // camera: drag with a mouse button or a finger, right stick or Q/E
    Vector2 mouse = Input::getMousePosition();
    if (mouseValid && (Input::isMousePressed(D_MOUSE_BUTTON_LEFT) || Input::isMousePressed(D_MOUSE_BUTTON_RIGHT))) {
        yaw -= (mouse.x - lastMouse.x) * mouseSensitivity;
        pitch -= (mouse.y - lastMouse.y) * mouseSensitivity;
    }
    lastMouse = mouse;
    mouseValid = true;

    yaw -= GameState::touchLook.x * mouseSensitivity;
    pitch -= GameState::touchLook.y * mouseSensitivity;

    if (hasPad) {
        float lookX = Input::getGamepadAxis(pad, D_GAMEPAD_AXIS_RIGHT_X);
        float lookY = Input::getGamepadAxis(pad, D_GAMEPAD_AXIS_RIGHT_Y);
        if (std::fabs(lookX) > DEADZONE) yaw -= lookX * 130.0f * dt;
        if (std::fabs(lookY) > DEADZONE) pitch += lookY * 130.0f * dt;
    }
    if (Input::isKeyPressed(D_KEY_Q)) yaw += 90.0f * dt;
    if (Input::isKeyPressed(D_KEY_E)) yaw -= 90.0f * dt;
    pitch = std::clamp(pitch, -70.0f, 5.0f);

    updateCamera(dt);

    if (dead) {
        // stays where it died while the game over screen comes up
        if (!frozen) {
            frozen = true;
            body.setLinearVelocity(Vector3::ZERO);
            body.setGravityFactor(0.0f);
        }
        updateAnimation(false, false);
        return;
    }

    Quaternion heading;
    heading.fromEulerAngles(0.0f, yaw, 0.0f, RotationOrder::YXZ);
    Vector3 forward = heading * Vector3(0, 0, -1);
    Vector3 right = heading * Vector3(1, 0, 0);

    float inputX = 0.0f;
    float inputY = 0.0f;
    if (Input::isKeyPressed(D_KEY_W) || Input::isKeyPressed(D_KEY_UP)) inputY += 1.0f;
    if (Input::isKeyPressed(D_KEY_S) || Input::isKeyPressed(D_KEY_DOWN)) inputY -= 1.0f;
    if (Input::isKeyPressed(D_KEY_D) || Input::isKeyPressed(D_KEY_RIGHT)) inputX += 1.0f;
    if (Input::isKeyPressed(D_KEY_A) || Input::isKeyPressed(D_KEY_LEFT)) inputX -= 1.0f;

    bool sprinting = Input::isKeyPressed(D_KEY_LEFT_SHIFT);
    bool jumpPressed = Input::isKeyPressed(D_KEY_SPACE) || GameState::touchJump;

    // the on-screen stick runs when pushed to the edge
    float touchAmount = GameState::touchMove.length();
    if (touchAmount > DEADZONE) {
        inputX += GameState::touchMove.x;
        inputY += GameState::touchMove.y;
        sprinting = sprinting || touchAmount > 0.9f;
    }

    if (hasPad) {
        float stickX = Input::getGamepadAxis(pad, D_GAMEPAD_AXIS_LEFT_X);
        float stickY = Input::getGamepadAxis(pad, D_GAMEPAD_AXIS_LEFT_Y);
        if (std::fabs(stickX) > DEADZONE) inputX += stickX;
        if (std::fabs(stickY) > DEADZONE) inputY -= stickY;
        if (Input::isGamepadButtonPressed(pad, D_GAMEPAD_BUTTON_DPAD_UP)) inputY += 1.0f;
        if (Input::isGamepadButtonPressed(pad, D_GAMEPAD_BUTTON_DPAD_DOWN)) inputY -= 1.0f;
        if (Input::isGamepadButtonPressed(pad, D_GAMEPAD_BUTTON_DPAD_RIGHT)) inputX += 1.0f;
        if (Input::isGamepadButtonPressed(pad, D_GAMEPAD_BUTTON_DPAD_LEFT)) inputX -= 1.0f;
        sprinting = sprinting || Input::isGamepadButtonPressed(pad, D_GAMEPAD_BUTTON_X);
        jumpPressed = jumpPressed || Input::isGamepadButtonPressed(pad, D_GAMEPAD_BUTTON_A);
    }

    Vector3 direction = forward * inputY + right * inputX;
    bool moving = direction.length() > 0.1f;
    if (moving) direction.normalize();

    // ground contacts refresh the coyote timer on every physics step
    if (coyoteTimer > 0.0f) {
        coyoteTimer -= dt;
    } else {
        grounded = false;
        onMovingGround = false;
    }

    // only after a real fall, not when the contacts flicker
    fallSpeed = std::min(fallSpeed, body.getLinearVelocity().y);
    if (grounded && !wasGrounded && fallSpeed < -LANDING_SPEED) {
        playSound(scene, "Land Sound");
        emitDust();
    }
    if (grounded) fallSpeed = 0.0f;
    wasGrounded = grounded;

    if (jumpPressed && !jumpHeld) {
        jumpBufferTimer = jumpBuffer;
    } else if (jumpBufferTimer > 0.0f) {
        jumpBufferTimer -= dt;
    }
    jumpHeld = jumpPressed;

    Vector3 velocity = body.getLinearVelocity();

    // letting go of jump early makes a short hop
    if (jumpCutPending && !jumpHeld) {
        jumpCutPending = false;
        if (velocity.y > 0.0f) velocity.y *= jumpCut;
    }

    Vector3 target = moving ? direction * (sprinting ? sprintSpeed : moveSpeed) : Vector3::ZERO;
    if (onMovingGround) target = target + groundVelocity;

    float blend = std::min((grounded ? acceleration : acceleration * airControl) * dt, 1.0f);
    velocity.x += (target.x - velocity.x) * blend;
    velocity.z += (target.z - velocity.z) * blend;

    if (jumpBufferTimer > 0.0f && coyoteTimer > 0.0f) {
        velocity.y = jumpSpeed;
        jumpBufferTimer = 0.0f;
        coyoteTimer = 0.0f;
        jumpCutPending = true;
        if (grounded) emitDust();
        grounded = false;
        onMovingGround = false;
        playSound(scene, "Jump Sound");
    }

    body.setLinearVelocity(velocity);

    // the body has its rotation locked, so turning is done here
    if (moving) {
        Quaternion facing(0.0f, Angle::radToDefault(std::atan2(direction.x, direction.z)), 0.0f);
        body.setRotation(Quaternion::slerp(std::min(turnSpeed * dt, 1.0f), body.getRotation(), facing));
    }

    if (body.getPosition().y < fallDeathY) {
        hurtTimer = 0.0f;
        hurt();
    }

    updateAnimation(moving, sprinting);
}
