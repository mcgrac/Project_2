#pragma once
#include "BaseScene.h"
#include "Shop.h"
#include "Party.h"

struct SDL_Texture;

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

    //helpers
    void OnResume() override;
    void OnPause() override;
    void CreateUI();

private:
    Shop shop; 
    Party* alliedParty;

    static constexpr int BACK_BUTTON_ID = 1;

    SDL_Texture* exitButton;
    SDL_Texture* background;
};
