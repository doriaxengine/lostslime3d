#pragma once

#include "ScriptBase.h"

// touching it costs a heart; saws spin and patrol with their bundle's animations
class Hazard : public doriax::ScriptBase {
public:
    Hazard(doriax::Scene* scene, doriax::Entity entity);
    virtual ~Hazard();
};
