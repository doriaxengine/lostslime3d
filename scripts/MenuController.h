#pragma once

#include "ScriptBase.h"
#include "ScriptProperty.h"
#include "Text.h"

#include <string>

class MenuController : public doriax::ScriptBase {
public:
    DPROPERTY("Primary Action")
    std::string primaryAction = "play";

    DPROPERTY("Primary Scene")
    std::string primaryScene = "Level One";

    DPROPERTY("Escape Action")
    std::string escapeAction = "";

    DPROPERTY("Escape Scene")
    std::string escapeScene = "Intro Scene";

    DPROPERTY("Score Text")
    doriax::Text* scoreText = nullptr;

    DPROPERTY("Best Text")
    doriax::Text* bestText = nullptr;

    DPROPERTY("Help Text")
    doriax::Text* helpText = nullptr;

    DPROPERTY("Touch Help")
    std::string touchHelp = "";   // replaces the help on phones and tablets

    DPROPERTY("Jingle")
    std::string jingle = "";

    MenuController(doriax::Scene* scene, doriax::Entity entity);
    virtual ~MenuController();

    void onUpdate();
    void onKeyDown(int key, bool repeat, int mods);
    void onGamepadButtonDown(int id, int button);

private:
    void show();

    bool shown = false;
    bool armed = false;
    float armTimer = 0.0f;
};
