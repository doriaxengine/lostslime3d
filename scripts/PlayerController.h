#pragma once

#include "ScriptBase.h"
#include "ScriptProperty.h"
#include "Particles.h"
#include "Vector2.h"
#include "Vector3.h"

#include <string>

class Enemy;

namespace doriax {
    class Body3D;
    class Contact3D;
    class PhysicsSystem;
}

class PlayerController : public doriax::ScriptBase {
public:
    DPROPERTY("Move Speed")
    float moveSpeed = 6.5f;

    DPROPERTY("Sprint Speed")
    float sprintSpeed = 9.5f;

    DPROPERTY("Jump Speed")
    float jumpSpeed = 9.0f;

    DPROPERTY("Acceleration")
    float acceleration = 18.0f;

    DPROPERTY("Air Control")
    float airControl = 0.55f;

    DPROPERTY("Turn Speed")
    float turnSpeed = 14.0f;

    DPROPERTY("Coyote Time")
    float coyoteTime = 0.12f;

    DPROPERTY("Jump Buffer")
    float jumpBuffer = 0.15f;

    DPROPERTY("Jump Cut")
    float jumpCut = 0.45f;

    DPROPERTY("Camera Yaw")
    float cameraYaw = -90.0f;

    DPROPERTY("Camera Pitch")
    float cameraPitch = -22.0f;

    DPROPERTY("Camera Distance")
    float cameraDistance = 9.0f;

    DPROPERTY("Look Height")
    float lookHeight = 1.0f;

    DPROPERTY("Camera Lag")
    float cameraLag = 7.0f;

    DPROPERTY("Mouse Sensitivity")
    float mouseSensitivity = 0.25f;

    DPROPERTY("Fall Death Y")
    float fallDeathY = -12.0f;

    DPROPERTY("Dust")
    doriax::Particles* dust = nullptr;   // puffs at the feet on jumps and landings

    PlayerController(doriax::Scene* scene, doriax::Entity entity);
    virtual ~PlayerController();

    void onUpdate();
    void onContactAdded(doriax::Body3D bodyA, doriax::Body3D bodyB, doriax::Contact3D contact);
    void onContactPersisted(doriax::Body3D bodyA, doriax::Body3D bodyB, doriax::Contact3D contact);

    void respawn(doriax::Vector3 position);
    bool isDead() const { return dead; }

private:
    void checkGround(doriax::Body3D& other, const doriax::Vector3& normal, bool isA);
    void touch(doriax::Entity other);
    void touchEnemy(Enemy* enemy, doriax::Entity other);
    void hurt();
    void setModelVisible(bool visible);
    void updateCamera(float dt);
    void updateAnimation(bool moving, bool sprinting);
    void play(const std::string& name, bool loop);
    void emitDust();

    doriax::PhysicsSystem* physics = nullptr;
    bool started = false;

    float yaw = 0.0f;
    float pitch = 0.0f;
    doriax::Vector2 lastMouse;
    bool mouseValid = false;
    doriax::Vector3 cameraFocus;
    bool cameraFocusValid = false;

    float coyoteTimer = 0.0f;
    float jumpBufferTimer = 0.0f;
    float hurtTimer = 0.0f;
    float blinkTimer = 0.0f;
    bool stompPending = false;
    bool jumpHeld = false;
    bool jumpCutPending = false;
    bool grounded = false;
    bool wasGrounded = false;
    float fallSpeed = 0.0f;
    bool dead = false;
    bool frozen = false;

    // speed of the kinematic platform under the feet
    doriax::Vector3 groundVelocity;
    bool onMovingGround = false;

    std::string currentAnim;
};
