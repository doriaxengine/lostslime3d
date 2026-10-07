#pragma once

#include "ScriptBase.h"
#include "ScriptProperty.h"
#include "Animation.h"
#include "Object.h"

// the star's spin, float and opening are actions of the Portal bundle, the key opens it
class ExitPortal : public doriax::ScriptBase {
public:
    DPROPERTY("Star Spin")
    doriax::Action* starSpin = nullptr;

    DPROPERTY("Open Animation")
    doriax::Animation* openAnimation = nullptr;

    DPROPERTY("Lock")
    doriax::Object* lock = nullptr;   // shown while closed

    DPROPERTY("Open Spin Speed")
    float openSpinSpeed = 4.0f;

    ExitPortal(doriax::Scene* scene, doriax::Entity entity);
    virtual ~ExitPortal();

    void onUpdate();
    void enter();

    bool isOpen() const { return open; }

private:
    bool open = false;
    bool entered = false;
    float lockedTimer = 0.0f;
};
