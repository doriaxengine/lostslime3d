#pragma once

#include "ScriptBase.h"
#include "ScriptProperty.h"

// walks the bundle's patrol; landing on top squashes it, any other touch costs a heart
class Enemy : public doriax::ScriptBase {
public:
    DPROPERTY("Stomp Height")
    float stompHeight = 0.35f;   // above its origin

    DPROPERTY("Score Value")
    int scoreValue = 50;

    Enemy(doriax::Scene* scene, doriax::Entity entity);
    virtual ~Enemy();

    void onUpdate();

    void squash();
    bool isSquashed() const { return squashed; }

private:
    bool squashed = false;
    bool flattened = false;
    float squashTimer = 0.0f;
};
