#include "MenuButton.h"

#include "GameState.h"

#include "Engine.h"
#include "Log.h"
#include "SceneManager.h"
#include "System.h"

using namespace doriax;

MenuButton::MenuButton(Scene* scene, Entity entity): Button(scene, entity) {
    REGISTER_BUTTON_EVENT(onPress, onPress);
    REGISTER_ENGINE_EVENT(onUpdate);
}

MenuButton::~MenuButton() {
    UNREGISTER_ENGINE_EVENT(onUpdate);
    if (scene && scene->isEntityCreated(entity)) {
        UNREGISTER_BUTTON_EVENT(onPress, onPress);
    }
}

void MenuButton::onPress() {
    runAction(scene, action, targetScene);
}

// properties are set after the constructor, so the first frame checks them
void MenuButton::onUpdate() {
    if (started) return;
    started = true;

    // a browser tab has nothing to quit to, and iOS apps don't close themselves
    const Platform platform = Engine::getPlatform();
    if (action == "quit" && (platform == Platform::Web || platform == Platform::iOS)) {
        setVisible(false);
    }
}

void MenuButton::runAction(Scene* scene, const std::string& action, const std::string& targetScene) {
    // keys still reach the old scenes under the loading screen
    if (SceneManager::isLoading()) return;

    playSound(scene, "Click Sound");

    if (action == "play") {
        GameState::newGame();
        GameState::loadScene(targetScene);
    } else if (action == "retry") {
        GameState::lives = GameState::maxLives;
        GameState::score = GameState::levelStartScore;
        GameState::loadScene(GameState::lastLevelScene.empty() ? targetScene : GameState::lastLevelScene);
    } else if (action == "resume") {
        GameState::resumeRequested = true;
    } else if (action == "menu") {
        GameState::paused = false;
        GameState::loadScene(targetScene);
    } else if (action == "quit") {
        System::instance().quit();
    } else {
        Log::warn("MenuButton: unknown action '%s'", action.c_str());
    }
}
