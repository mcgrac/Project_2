#pragma once
#include "BaseScene.h"
#include "Shop.h"
#include "Party.h"

class ShopScene : public BaseScene
{
public:
    ShopScene(Shop shop, Party* allied);
    ~ShopScene();

    void Load() override;
    void Update(float dt) override;
    void PostUpdate(float dt) override;
    void Unload() override;
    void LoadTextures() override;

    bool OnUIMouseClickEvent(UIElement* uiElement) override;

private:
    Shop shop; 
    Party* alliedParty;

    static constexpr int BACK_BUTTON_ID = 1;
};
