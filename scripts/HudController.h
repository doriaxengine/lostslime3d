#pragma once

#include "ScriptBase.h"
#include "ScriptProperty.h"
#include "Animation.h"
#include "Image.h"
#include "Text.h"

#include <string>

class HudController : public doriax::ScriptBase {
public:
    DPROPERTY("Heart 1")
    doriax::Image* heart1 = nullptr;

    DPROPERTY("Heart 2")
    doriax::Image* heart2 = nullptr;

    DPROPERTY("Heart 3")
    doriax::Image* heart3 = nullptr;

    DPROPERTY("Coin Text")
    doriax::Text* coinText = nullptr;

    DPROPERTY("Gem Text")
    doriax::Text* gemText = nullptr;

    DPROPERTY("Score Text")
    doriax::Text* scoreText = nullptr;

    DPROPERTY("Banner")
    doriax::Image* banner = nullptr;

    DPROPERTY("Banner Text")
    doriax::Text* bannerText = nullptr;

    DPROPERTY("Banner Animation")
    doriax::Animation* bannerAnimation = nullptr;   // fades the banner in and out

    DPROPERTY("Full Heart Texture")
    std::string fullHeart = "ui/heart.png";

    DPROPERTY("Empty Heart Texture")
    std::string emptyHeart = "ui/heart_empty.png";

    HudController(doriax::Scene* scene, doriax::Entity entity);
    virtual ~HudController();

    void onUpdate();

private:
    void showBanner(const std::string& text);

    int shownLives = -1;
    int shownScore = -1;
    std::string shownCoins;
    std::string shownGems;
    std::string shownLevel;
    bool portalOpen = false;
};
