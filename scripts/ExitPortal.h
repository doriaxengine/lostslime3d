#pragma once

#include "ScriptBase.h"
#include "ScriptProperty.h"
#include "Animation.h"

// the star's spin, float and opening are actions of the Portal bundle
class ExitPortal : public doriax::ScriptBase {
public:
    DPROPERTY("Star Spin")
    doriax::Action* starSpin = nullptr;

    DPROPERTY("Open Animation")
    doriax::Animation* openAnimation = nullptr;

    DPROPERTY("Open Spin Speed")
    float openSpinSpeed = 4.0f;

    ExitPortal(doriax::Scene* scene, doriax::Entity entity);
    virtual ~ExitPortal();

    void onUpdate();
    void enter();

private:
    bool open = false;
    bool entered = false;
};
