#include "Collectible.h"

#include "GameState.h"

#include "Body3D.h"
#include "Object.h"

using namespace doriax;

Collectible::Collectible(Scene* scene, Entity entity): ScriptBase(scene, entity) {
    REGISTER_ENGINE_EVENT(onUpdate);
}

Collectible::~Collectible() {
    UNREGISTER_ENGINE_EVENT(onUpdate);
}

// collect() runs in a contact callback, the body is turned off here
void Collectible::onUpdate() {
    if (!collected || hidden || GameState::paused) return;

    hidden = true;
    Object(scene, entity).setVisible(false);
    Body3D(scene, entity).deactivate();
}

void Collectible::collect() {
    if (collected) return;
    collected = true;

    GameState::score += scoreValue;
    playSound(scene, isGem() ? "Gem Sound" : "Coin Sound");
}
