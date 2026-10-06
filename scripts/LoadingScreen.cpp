#include "LoadingScreen.h"

#include "GameState.h"

#include "Engine.h"
#include "SceneManager.h"

#include <algorithm>

using namespace doriax;

LoadingScreen::LoadingScreen(Scene* scene, Entity entity): ScriptBase(scene, entity) {
    REGISTER_ENGINE_EVENT(onUpdate);
    REGISTER_ENGINE_EVENT(onSceneLoaded);
}

LoadingScreen::~LoadingScreen() {
    UNREGISTER_ENGINE_EVENT(onUpdate);
    UNREGISTER_ENGINE_EVENT(onSceneLoaded);
    if (holding) SceneManager::releaseLoading();
}

void LoadingScreen::setAlpha(float alpha) {
    if (backdrop) backdrop->setAlpha(alpha);
    if (slime) slime->setAlpha(alpha);
    if (titleText) titleText->setAlpha(alpha);
    if (statusText) statusText->setAlpha(alpha);
    if (bar) bar->setAlpha(alpha);
    if (barFill) barFill->setAlpha(alpha);
}

// keeps the screen up while it fades out
void LoadingScreen::onSceneLoaded() {
    if (holding || !Engine::isSceneRunning(scene)) return;

    holding = SceneManager::holdLoading();
    fadeTimer = 0.0f;
}

void LoadingScreen::onUpdate() {
    if (!SceneManager::isLoading()) {
        active = false;
        return;
    }

    if (!active) {
        active = true;
        timer = 0.0f;
        progress = 0.0f;
        if (titleText) titleText->setText(GameState::loadingTitle);
    }

    // the first frame after the switch is a long one
    float dt = std::min(Engine::getDeltatime(), 1.0f / 30.0f);
    timer += dt;

    if (holding) {
        fadeTimer += dt;
        float alpha = fadeOut > 0.0f ? 1.0f - fadeTimer / fadeOut : 0.0f;
        setAlpha(std::max(0.0f, alpha));
        if (alpha <= 0.0f) {
            holding = false;
            SceneManager::releaseLoading();
        }
    } else {
        float delay = SceneManager::getLoadingDelay();
        setAlpha(delay > 0.0f ? std::min(1.0f, timer / delay) : 1.0f);
    }

    if (barFill) {
        progress += (SceneManager::getLoadingProgress() - progress) * std::min(1.0f, dt * 10.0f);
        // never narrower than its rounded ends
        barFill->setWidth((unsigned int)std::max((float)barFill->getHeight(), barWidth * progress));
    }
}
