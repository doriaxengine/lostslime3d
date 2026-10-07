#include "GameState.h"

#include "System.h"
#include "SceneManager.h"
#include "Sound.h"
#include "SoundComponent.h"

using namespace doriax;

int GameState::score = 0;
int GameState::levelStartScore = 0;
int GameState::lives = 3;
int GameState::maxLives = 3;
int GameState::coins = 0;
int GameState::coinsTotal = 0;
int GameState::gems = 0;
int GameState::gemsTotal = 0;
bool GameState::hasKey = false;
bool GameState::paused = false;

std::string GameState::levelName;
std::string GameState::nextScene;
std::string GameState::lastLevelScene;
std::string GameState::loadingTitle;
std::string GameState::message;

bool GameState::levelCompleteRequested = false;
bool GameState::gameOverRequested = false;
bool GameState::respawnRequested = false;
bool GameState::resumeRequested = false;
bool GameState::pauseRequested = false;

Vector2 GameState::touchMove;
Vector2 GameState::touchLook;
bool GameState::touchJump = false;

static int bestScore = -1;  // loaded on first use

void GameState::newGame(){
    score = 0;
    levelStartScore = 0;
    lives = maxLives;
    paused = false;
}

void GameState::beginLevel(const std::string& sceneName, const std::string& displayName, const std::string& next){
    lastLevelScene = sceneName;
    levelName = displayName;
    nextScene = next;
    hasKey = false;
    message.clear();
    paused = false;
    levelCompleteRequested = false;
    gameOverRequested = false;
    respawnRequested = false;
    resumeRequested = false;
    pauseRequested = false;
}

void GameState::loadScene(const std::string& sceneName){
    if (SceneManager::isLoading()) return;

    bool level = sceneName.rfind("Level ", 0) == 0;
    if (level) levelStartScore = score;
    loadingTitle = level ? sceneName : "";
    levelName.clear();
    SceneManager::loadScene(sceneName);
}

int GameState::getBestScore(){
    if (bestScore < 0) {
        bestScore = System::instance().getIntegerForKey("bestScore", 0);
    }
    return bestScore;
}

void GameState::recordScore(){
    if (score <= getBestScore()) return;
    bestScore = score;
    System::instance().setIntegerForKey("bestScore", bestScore);
}

void playSound(Scene* scene, const std::string& name){
    if (!scene) return;
    Entity entity = scene->findEntity(name);
    if (entity == NULL_ENTITY || !scene->findComponent<SoundComponent>(entity)) return;
    Sound sound(scene, entity);
    sound.stop();
    sound.play();
}
