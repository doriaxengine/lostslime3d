#include "Checkpoint.h"

#include "GameState.h"

#include "Object.h"

using namespace doriax;

Checkpoint::Checkpoint(Scene* scene, Entity entity): ScriptBase(scene, entity) {
}

Checkpoint::~Checkpoint() {
}

// called during the physics step
void Checkpoint::reach() {
    if (reached) return;
    reached = true;

    GameState::hasCheckpoint = true;
    GameState::checkpoint = Object(scene, entity).getWorldPosition() + Vector3(0.0f, 0.3f, 0.0f);
    GameState::message = "Checkpoint!";
    playSound(scene, "Checkpoint Sound");
    if (reachAnimation) reachAnimation->start();
}
