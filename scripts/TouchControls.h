#pragma once

#include "ScriptBase.h"
#include "ScriptProperty.h"
#include "Image.h"

#include <vector>

class TouchControls : public doriax::ScriptBase {
public:
    DPROPERTY("Stick")
    doriax::Image* stick = nullptr;

    DPROPERTY("Knob")
    doriax::Image* knob = nullptr;

    DPROPERTY("Jump")
    doriax::Image* jump = nullptr;

    DPROPERTY("Pause")
    doriax::Image* pause = nullptr;

    DPROPERTY("Touch Margin")
    float touchMargin = 12.0f;   // extra area around each control

    TouchControls(doriax::Scene* scene, doriax::Entity entity);
    virtual ~TouchControls();

    void onUpdate();
    void onKeyDown(int key, bool repeat, int mods);
    void onGamepadButtonDown(int id, int button);

private:
    bool isInside(doriax::Image* image, doriax::Vector2 point) const;
    bool updateButton(doriax::Image* button);
    void setShown(bool value);

    bool shown = true;   // visible in the scene, hidden on the first update if not used
    bool pauseTouched = false;

    // fingers on the stick and on the camera, -1 if none
    int stickFinger = -1;
    int lookFinger = -1;
    doriax::Vector2 lastLook;
    std::vector<int> fingers;   // down last frame
};
