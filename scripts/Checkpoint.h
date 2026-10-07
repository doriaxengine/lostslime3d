#pragma once

#include "ScriptBase.h"
#include "ScriptProperty.h"
#include "Animation.h"

// touching the flag moves the respawn here; its tint and pop are the bundle's Reach animation
class Checkpoint : public doriax::ScriptBase {
public:
    DPROPERTY("Reach Animation")
    doriax::Animation* reachAnimation = nullptr;

    Checkpoint(doriax::Scene* scene, doriax::Entity entity);
    virtual ~Checkpoint();

    void reach();

private:
    bool reached = false;
};
