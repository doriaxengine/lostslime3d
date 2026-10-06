#pragma once

#include "ScriptBase.h"
#include "ScriptProperty.h"
#include "Image.h"
#include "Text.h"

class LoadingScreen : public doriax::ScriptBase {
public:
    DPROPERTY("Backdrop")
    doriax::Image* backdrop = nullptr;

    DPROPERTY("Slime")
    doriax::Image* slime = nullptr;   // hops with its Hop track

    DPROPERTY("Title Text")
    doriax::Text* titleText = nullptr;

    DPROPERTY("Status Text")
    doriax::Text* statusText = nullptr;

    DPROPERTY("Bar")
    doriax::Image* bar = nullptr;

    DPROPERTY("Bar Fill")
    doriax::Image* barFill = nullptr;

    DPROPERTY("Bar Width")
    float barWidth = 360.0f;

    DPROPERTY("Fade Out")
    float fadeOut = 0.3f;

    LoadingScreen(doriax::Scene* scene, doriax::Entity entity);
    virtual ~LoadingScreen();

    void onUpdate();
    void onSceneLoaded();

private:
    void setAlpha(float alpha);

    bool active = false;
    bool holding = false;
    float timer = 0.0f;
    float fadeTimer = 0.0f;
    float progress = 0.0f;
};
