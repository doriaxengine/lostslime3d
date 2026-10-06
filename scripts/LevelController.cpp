#include "LevelController.h"

#include "GameState.h"
#include "Collectible.h"
#include "PlayerController.h"

#include "Engine.h"
#include "Input.h"
#include "Log.h"
#include "SceneManager.h"
#include "PhysicsSystem.h"
#include "ActionSystem.h"
#include "AudioSystem.h"

using namespace doriax;

LevelController::LevelController(Scene* scene, Entity entity): ScriptBase(scene, entity) {
    REGISTER_ENGINE_EVENT(onUpdate);
    REGISTER_ENGINE_EVENT(onSceneLoaded);
    REGISTER_ENGINE_EVENT(onKeyDown);
    REGISTER_ENGINE_EVENT(onGamepadButtonDown);
}

LevelController::~LevelController() {
    UNREGISTER_ENGINE_EVENT(onUpdate);
    UNREGISTER_ENGINE_EVENT(onSceneLoaded);
    UNREGISTER_ENGINE_EVENT(onKeyDown);
    UNREGISTER_ENGINE_EVENT(onGamepadButtonDown);
}

void LevelController::start() {
    started = true;
    GameState::beginLevel(sceneName, levelTitle, nextScene);

    Entity slime = scene->findEntity("Slime", scene->findEntity("Player"));
    player = findScript<PlayerController>(scene, slime, "PlayerController");
    if (!player) {
        Log::error("LevelController: no Player bundle with the Slime in the scene");
    } else if (spawnPoint) {
        player->respawn(spawnPoint->getWorldPosition());
    }

    countPickups();
    setPaused(false);
}

void LevelController::setPaused(bool paused) {
    GameState::paused = paused;
    scene->getSystem<PhysicsSystem>()->setPaused(paused);
    scene->getSystem<ActionSystem>()->setPaused(paused);
    scene->getSystem<AudioSystem>()->setPaused(paused);
}

// counted every frame, a coin taken before start() still counts
void LevelController::countPickups() {
    int coins = 0, coinsTotal = 0, gems = 0, gemsTotal = 0;

    auto scripts = scene->getComponentArray<ScriptComponent>();
    for (size_t i = 0; i < scripts->size(); i++) {
        for (auto& entry : scripts->getComponentFromIndex(i).scripts) {
            if (entry.type == ScriptType::LUA || entry.className != "Collectible" || !entry.instance) continue;

            Collectible* item = static_cast<Collectible*>(entry.instance);
            if (item->isGem()) {
                gemsTotal++;
                if (item->isCollected()) gems++;
            } else {
                coinsTotal++;
                if (item->isCollected()) coins++;
            }
        }
    }

    GameState::coins = coins;
    GameState::coinsTotal = coinsTotal;
    GameState::gems = gems;
    GameState::gemsTotal = gemsTotal;
}

void LevelController::finish(const std::string& overlay, const std::string& next, float delay) {
    finished = true;
    overlayScene = overlay;
    loadNext = next;
    timer = delay;
}

void LevelController::togglePause() {
    if (GameState::paused) {
        GameState::resumeRequested = true;
    } else {
        setPaused(true);
        SceneManager::addChildScene("Pause Scene");
    }
}

void LevelController::onKeyDown(int key, bool repeat, int) {
    if (repeat || !started || finished) return;
    if (key == D_KEY_ESCAPE || key == D_KEY_P) togglePause();
}

void LevelController::onGamepadButtonDown(int, int button) {
    if (!started || finished || button != D_GAMEPAD_BUTTON_START) return;
    togglePause();
}

void LevelController::onSceneLoaded() {
    if (!started) start();
}

void LevelController::onUpdate() {
    if (!started) return;

    if (GameState::resumeRequested) {
        GameState::resumeRequested = false;
        SceneManager::removeChildScene("Pause Scene");
        setPaused(false);
    }

    // the on-screen pause button
    if (GameState::pauseRequested) {
        GameState::pauseRequested = false;
        if (!GameState::paused && !finished) togglePause();
    }

    if (GameState::paused) return;

    countPickups();

    if (GameState::respawnRequested) {
        GameState::respawnRequested = false;
        if (player && spawnPoint) {
            player->respawn(spawnPoint->getWorldPosition());
        }
    }

    if (finished) {
        timer -= Engine::getDeltatime();
        if (timer > 0.0f) return;

        if (!loadNext.empty()) {
            GameState::loadScene(loadNext);
            loadNext.clear();
        } else if (!overlayScene.empty()) {
            setPaused(true);
            SceneManager::addChildScene(overlayScene);
            overlayScene.clear();
        }
        return;
    }

    if (GameState::levelCompleteRequested) {
        GameState::levelCompleteRequested = false;
        GameState::score += completionBonus;
        if (nextScene.empty()) {
            GameState::recordScore();
            finish("Win Scene", "", 1.0f);
        } else {
            finish("", nextScene, 0.8f);
        }
    } else if (GameState::gameOverRequested) {
        GameState::gameOverRequested = false;
        GameState::recordScore();
        finish("Game Over Scene", "", 1.6f);
    }
}
