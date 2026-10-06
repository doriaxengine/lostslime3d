#include "ExitPortal.h"

#include "GameState.h"

using namespace doriax;

ExitPortal::ExitPortal(Scene* scene, Entity entity): ScriptBase(scene, entity) {
    REGISTER_ENGINE_EVENT(onUpdate);
}

ExitPortal::~ExitPortal() {
    UNREGISTER_ENGINE_EVENT(onUpdate);
}

// opens once every coin is taken
void ExitPortal::onUpdate() {
    if (open || GameState::paused || !GameState::isPortalOpen()) return;

    open = true;
    playSound(scene, "Portal Sound");
    if (openAnimation) openAnimation->start();
    if (starSpin) starSpin->setSpeed(openSpinSpeed);
}

void ExitPortal::enter() {
    if (entered || !open) return;
    entered = true;
    GameState::levelCompleteRequested = true;
}
