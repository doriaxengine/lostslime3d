#pragma once

#include <string>

#include "Scene.h"
#include "Entity.h"
#include "ScriptComponent.h"
#include "Vector2.h"
#include "Vector3.h"

struct GameState {
    static int score;
    static int levelStartScore;
    static int lives;
    static int maxLives;
    static int coins;
    static int coinsTotal;
    static int gems;
    static int gemsTotal;
    static bool hasKey; // opens the portal
    static bool hasCheckpoint;
    static doriax::Vector3 checkpoint; // where the slime respawns once a flag is reached
    static bool paused;

    static std::string levelName;
    static std::string nextScene;
    static std::string lastLevelScene; // for retry
    static std::string loadingTitle;
    static std::string message; // shown once at the bottom of the HUD

    // handled by LevelController
    static bool levelCompleteRequested;
    static bool gameOverRequested;
    static bool respawnRequested;
    static bool resumeRequested;
    static bool pauseRequested;

    // on-screen controls, set by TouchControls
    static doriax::Vector2 touchMove; // stick, y up, length up to 1
    static doriax::Vector2 touchLook; // camera drag this frame
    static bool touchJump;

    static void newGame();
    static void beginLevel(const std::string& sceneName, const std::string& displayName, const std::string& next);
    static void loadScene(const std::string& sceneName);

    static int getBestScore();
    static void recordScore();
};

// C++ script instance of an entity, or nullptr
template<typename T>
T* findScript(doriax::Scene* scene, doriax::Entity entity, const char* className){
    if (!scene || entity == NULL_ENTITY || !scene->isEntityCreated(entity)) return nullptr;
    doriax::ScriptComponent* scripts = scene->findComponent<doriax::ScriptComponent>(entity);
    if (!scripts) return nullptr;
    for (auto& entry : scripts->scripts){
        if (entry.type != doriax::ScriptType::LUA && entry.className == className && entry.instance){
            return static_cast<T*>(entry.instance);
        }
    }
    return nullptr;
}

// plays one of the Sounds bundle entities
void playSound(doriax::Scene* scene, const std::string& name);
