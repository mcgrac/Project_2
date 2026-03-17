#pragma once
#include "BaseScene.h"

class MainMenuScene : public BaseScene
{
public:
    void Load() override;
    void Update(float dt) override;
    void PostUpdate(float dt) override;
    void Unload() override;

    bool OnUIMouseClickEvent(UIElement* uiElement) override;
};