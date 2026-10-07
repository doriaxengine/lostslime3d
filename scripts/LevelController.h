#pragma once

#include "ScriptBase.h"
#include "ScriptProperty.h"
#include "Object.h"

#include <string>

class PlayerController;

class LevelController : public doriax::ScriptBase {
public:
    DPROPERTY("Scene Name")
    std::string sceneName = "Level One";

    DPROPERTY("Level Title")
    std::string levelTitle = "Verdant Rise";

    DPROPERTY("Next Scene")
    std::string nextScene = "Level Two";

    DPROPERTY("Spawn Point")
    doriax::Object* spawnPoint = nullptr;

    DPROPERTY("Completion Bonus")
    int completionBonus = 100;

    LevelController(doriax::Scene* scene, doriax::Entity entity);
    virtual ~LevelController();

    void onUpdate();
    void onSceneLoaded();
    void onKeyDown(int key, bool repeat, int mods);
    void onGamepadButtonDown(int id, int button);

private:
    void start();
    void setPaused(bool paused);
    void togglePause();
    void countPickups();
    void finish(const std::string& overlay, const std::string& next, float delay);

    bool started = false;
    bool finished = false;
    float timer = 0.0f;
    float finishDelay = 0.0f;
    float musicVolume = 0.0f;
    std::string overlayScene;
    std::string loadNext;

    PlayerController* player = nullptr;
};
