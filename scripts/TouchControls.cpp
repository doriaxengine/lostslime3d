#include "TouchControls.h"

#include "GameState.h"

#include "Input.h"
#include "System.h"
#include "SceneManager.h"

#include <algorithm>

using namespace doriax;

// follows the last input used, starting from what the device is
static int usingTouch = -1;

TouchControls::TouchControls(Scene* scene, Entity entity): ScriptBase(scene, entity) {
    REGISTER_ENGINE_EVENT(onUpdate);
    REGISTER_ENGINE_EVENT(onKeyDown);
    REGISTER_ENGINE_EVENT(onGamepadButtonDown);
}

TouchControls::~TouchControls() {
    UNREGISTER_ENGINE_EVENT(onUpdate);
    UNREGISTER_ENGINE_EVENT(onKeyDown);
    UNREGISTER_ENGINE_EVENT(onGamepadButtonDown);
}

bool TouchControls::isInside(Image* image, Vector2 point) const {
    if (!image) return false;

    Vector3 pos = image->getWorldPosition();
    Vector3 scale = image->getWorldScale();
    float right = pos.x + image->getWidth() * scale.x;
    float bottom = pos.y + image->getHeight() * scale.y;

    return point.x >= pos.x - touchMargin && point.x <= right + touchMargin &&
           point.y >= pos.y - touchMargin && point.y <= bottom + touchMargin;
}

// fingers moving the stick or the camera don't press buttons
bool TouchControls::updateButton(Image* button) {
    if (!button || !shown) return false;

    bool touched = false;
    for (const Touch& touch : Input::getTouches()) {
        if (touch.pointer != stickFinger && touch.pointer != lookFinger && isInside(button, touch.position)) {
            touched = true;
        }
    }
    button->setAlpha(touched ? 0.9f : 0.5f);
    return touched;
}

void TouchControls::setShown(bool value) {
    shown = value;
    for (Image* image : {stick, jump, pause}) {
        if (image) image->setVisible(value);
    }
}

void TouchControls::onUpdate() {
    if (usingTouch < 0) {
        usingTouch = System::instance().isTouchDevice() ? 1 : 0;
    }
    if (Input::numTouches() > 0) {
        usingTouch = 1;
    }

    bool active = usingTouch && !GameState::paused && !SceneManager::isLoading();
    if (active != shown) {
        setShown(active);
    }
    if (!shown) {
        stickFinger = -1;
        lookFinger = -1;
    }

    std::vector<Touch> touches = Input::getTouches();

    Vector2 center;
    float radius = 1.0f;
    if (stick) {
        Vector3 pos = stick->getWorldPosition();
        Vector3 scale = stick->getWorldScale();
        radius = stick->getWidth() * scale.x * 0.5f;
        center = Vector2(pos.x + radius, pos.y + stick->getHeight() * scale.y * 0.5f);
    }

    Vector2 move;
    Vector2 look;
    bool stickHeld = false;
    bool lookHeld = false;

    for (const Touch& touch : touches) {
        // a new finger takes the stick, or the camera if it is not on a button
        bool isNew = std::find(fingers.begin(), fingers.end(), touch.pointer) == fingers.end();
        if (isNew && shown) {
            if (stickFinger < 0 && isInside(stick, touch.position)) {
                stickFinger = touch.pointer;
            } else if (lookFinger < 0 && !isInside(jump, touch.position) && !isInside(pause, touch.position)) {
                lookFinger = touch.pointer;
                lastLook = touch.position;
            }
        }

        if (touch.pointer == stickFinger) {
            stickHeld = true;
            move = touch.position - center;
            if (move.length() > radius) move = move * (radius / move.length());
        } else if (touch.pointer == lookFinger) {
            lookHeld = true;
            look = touch.position - lastLook;
            lastLook = touch.position;
        }
    }

    fingers.clear();
    for (const Touch& touch : touches) {
        fingers.push_back(touch.pointer);
    }
    if (!stickHeld) stickFinger = -1;
    if (!lookHeld) lookFinger = -1;

    if (stick && shown) {
        GameState::touchMove = Vector2(move.x, -move.y) / radius;
        stick->setAlpha(stickHeld ? 0.9f : 0.5f);
        if (knob) {
            knob->setPositionOffset(move);
            knob->setAlpha(stickHeld ? 0.9f : 0.5f);
        }
    } else {
        GameState::touchMove = Vector2::ZERO;
    }
    GameState::touchLook = look;
    GameState::touchJump = updateButton(jump);

    bool pauseDown = updateButton(pause);
    if (pauseDown && !pauseTouched) {
        GameState::pauseRequested = true;
    }
    pauseTouched = pauseDown;
}

// a keyboard or gamepad takes over from the screen
void TouchControls::onKeyDown(int, bool, int) {
    usingTouch = 0;
}

void TouchControls::onGamepadButtonDown(int, int) {
    usingTouch = 0;
}
