#include "ExitPortal.h"

#include "GameState.h"

#include "Engine.h"

using namespace doriax;

ExitPortal::ExitPortal(Scene* scene, Entity entity): ScriptBase(scene, entity) {
    REGISTER_ENGINE_EVENT(onUpdate);
}

ExitPortal::~ExitPortal() {
    UNREGISTER_ENGINE_EVENT(onUpdate);
}

// opens once the key is taken
void ExitPortal::onUpdate() {
    if (GameState::paused) return;
    if (lockedTimer > 0.0f) lockedTimer -= Engine::getDeltatime();
    if (open || !GameState::hasKey) return;

    open = true;
    playSound(scene, "Portal Sound");
    GameState::message = "The portal is open!";
    if (lock) lock->setVisible(false);
    if (openAnimation) openAnimation->start();
    if (starSpin) starSpin->setSpeed(openSpinSpeed);
}

void ExitPortal::enter() {
    if (entered) return;

    if (open) {
        entered = true;
        GameState::levelCompleteRequested = true;
    } else if (lockedTimer <= 0.0f) {
        lockedTimer = 2.0f;
        playSound(scene, "Locked Sound");
        GameState::message = "Find the key first!";
    }
}
