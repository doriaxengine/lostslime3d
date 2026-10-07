#pragma once

#include "ScriptBase.h"
#include "ScriptProperty.h"

#include <string>

// spinning and bobbing is the bundle's Idle animation
class Collectible : public doriax::ScriptBase {
public:
    DPROPERTY("Kind")
    std::string kind = "coin";   // coin, gem or key

    DPROPERTY("Score Value")
    int scoreValue = 10;

    Collectible(doriax::Scene* scene, doriax::Entity entity);
    virtual ~Collectible();

    void onUpdate();
    void collect();

    bool isGem() const { return kind == "gem"; }
    bool isKey() const { return kind == "key"; }
    bool isCollected() const { return collected; }

private:
    bool collected = false;
    bool hidden = false;
};
