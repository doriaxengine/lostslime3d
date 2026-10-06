#pragma once

#include "Button.h"
#include "ScriptProperty.h"

#include <string>

class MenuButton : public doriax::Button {
public:
    DPROPERTY("Action")
    std::string action = "play";   // play, retry, resume, menu or quit

    DPROPERTY("Target Scene")
    std::string targetScene = "Level One";

    MenuButton(doriax::Scene* scene, doriax::Entity entity);
    virtual ~MenuButton();

    void onPress();

    static void runAction(doriax::Scene* scene, const std::string& action, const std::string& targetScene);
};
