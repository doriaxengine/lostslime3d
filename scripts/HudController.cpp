#include "HudController.h"

#include "GameState.h"

#include "Engine.h"

#include <algorithm>

using namespace doriax;

static const float MESSAGE_TIME = 2.5f;
static const float MESSAGE_FADE = 0.4f;

HudController::HudController(Scene* scene, Entity entity): ScriptBase(scene, entity) {
    // the hearts are full in the scene
    shownLives = GameState::maxLives;

    REGISTER_ENGINE_EVENT(onUpdate);
}

HudController::~HudController() {
    UNREGISTER_ENGINE_EVENT(onUpdate);
}

void HudController::showBanner(const std::string& text) {
    if (!banner || !bannerText || !bannerAnimation) return;
    bannerText->setText(text);
    banner->setVisible(true);
    bannerAnimation->restart();
}

void HudController::showMessage(const std::string& text) {
    if (!message || !messageText) return;
    messageText->setText(text);
    message->setVisible(true);
    messageTimer = MESSAGE_TIME;
}

void HudController::onUpdate() {
    if (GameState::lives != shownLives) {
        Image* hearts[3] = {heart1, heart2, heart3};
        for (int i = 0; i < 3; i++) {
            bool full = i < GameState::lives;
            if (hearts[i] && full != (i < shownLives)) {
                hearts[i]->setTexture(full ? fullHeart : emptyHeart);
            }
        }
        shownLives = GameState::lives;
    }

    std::string coins = std::to_string(GameState::coins) + "/" + std::to_string(GameState::coinsTotal);
    if (coinText && coins != shownCoins) {
        shownCoins = coins;
        coinText->setText(coins);
    }

    std::string gems = std::to_string(GameState::gems) + "/" + std::to_string(GameState::gemsTotal);
    if (gemText && gems != shownGems) {
        shownGems = gems;
        gemText->setText(gems);
    }

    if (keyIcon && GameState::hasKey != shownKey) {
        shownKey = GameState::hasKey;
        keyIcon->setColor(1.0f, 1.0f, 1.0f, shownKey ? 1.0f : 0.3f);
    }

    if (scoreText && GameState::score != shownScore) {
        shownScore = GameState::score;
        scoreText->setText(std::to_string(shownScore));
    }

    if (GameState::levelName != shownLevel) {
        shownLevel = GameState::levelName;
        if (!shownLevel.empty()) showBanner(shownLevel);
    }

    if (!GameState::message.empty()) {
        showMessage(GameState::message);
        GameState::message.clear();
    }

    if (messageTimer > 0.0f) {
        messageTimer -= Engine::getDeltatime();
        // fades out at the end
        float alpha = std::clamp(messageTimer / MESSAGE_FADE, 0.0f, 1.0f);
        message->setColor(1.0f, 1.0f, 1.0f, alpha);
        messageText->setColor(1.0f, 1.0f, 1.0f, alpha);
        if (messageTimer <= 0.0f) message->setVisible(false);
    }
}
