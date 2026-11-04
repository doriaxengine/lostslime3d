#include "MenuController.h"

#include "GameState.h"
#include "MenuButton.h"

#include "Engine.h"
#include "Input.h"
#include "System.h"

using namespace doriax;

MenuController::MenuController(Scene* scene, Entity entity): ScriptBase(scene, entity) {
    REGISTER_ENGINE_EVENT(onUpdate);
    REGISTER_ENGINE_EVENT(onKeyDown);
    REGISTER_ENGINE_EVENT(onGamepadButtonDown);
}

MenuController::~MenuController() {
    UNREGISTER_ENGINE_EVENT(onUpdate);
    UNREGISTER_ENGINE_EVENT(onKeyDown);
    UNREGISTER_ENGINE_EVENT(onGamepadButtonDown);
}

void MenuController::show() {
    shown = true;
    armed = false;
    armTimer = 0.0f;

    if (scoreText) scoreText->setText(std::to_string(GameState::score));
    if (bestText) bestText->setText("Best " + std::to_string(GameState::getBestScore()));
    if (helpText && !touchHelp.empty() && System::instance().isTouchDevice()) helpText->setText(touchHelp);
    if (!jingle.empty()) playSound(scene, jingle);
}

void MenuController::onUpdate() {
    // overlay scenes stay alive while hidden
    if (!Engine::isSceneRunning(scene)) {
        shown = false;
        return;
    }
    if (!shown) show();

    // ignore the key press that opened this menu
    if (!armed) {
        armTimer += Engine::getDeltatime();
        armed = armTimer > 0.3f;
        if (armed) focusPrimaryButton();
    }
}

// keys and gamepads start on the button of the primary action
void MenuController::focusPrimaryButton() {
    auto scripts = scene->getComponentArray<ScriptComponent>();
    for (size_t i = 0; i < scripts->size(); i++) {
        for (auto& entry : scripts->getComponentFromIndex(i).scripts) {
            if (entry.type == ScriptType::LUA || entry.className != "MenuButton" || !entry.instance) continue;

            MenuButton* button = static_cast<MenuButton*>(entry.instance);
            if (button->action == primaryAction) {
                button->setFocus();
                return;
            }
        }
    }
}

void MenuController::onKeyDown(int key, bool repeat, int) {
    if (repeat || !armed || !Engine::isSceneRunning(scene)) return;
    if ((key == D_KEY_ENTER || key == D_KEY_SPACE) && !primaryAction.empty()) {
        MenuButton::runAction(scene, primaryAction, primaryScene);
    } else if (key == D_KEY_ESCAPE && !escapeAction.empty()) {
        MenuButton::runAction(scene, escapeAction, escapeScene);
    }
}

void MenuController::onGamepadButtonDown(int, int button) {
    if (!armed || !Engine::isSceneRunning(scene)) return;
    if ((button == D_GAMEPAD_BUTTON_A || button == D_GAMEPAD_BUTTON_START) && !primaryAction.empty()) {
        MenuButton::runAction(scene, primaryAction, primaryScene);
    } else if (button == D_GAMEPAD_BUTTON_B && !escapeAction.empty()) {
        MenuButton::runAction(scene, escapeAction, escapeScene);
    }
}
