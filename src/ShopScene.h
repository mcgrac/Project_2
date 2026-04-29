#pragma once
#include "BaseScene.h"
#include "Shop.h"

struct SDL_Texture;

class Party;

enum class ShopState
{
    CLOSED,
    SHOW_ITEMS,
    SELECT_CHARACTER
};

class ShopScene : public BaseScene
{
public:
    ShopScene(Shop* shop, Party* allied);
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

    void CreateItemButtons();
    void CreateCharacterSelectionUI();

    Shop* shop; 
    Party* alliedParty;

    static constexpr int BACK_BUTTON_ID = 1;
    static constexpr int OPEN_SHOP_BUTTON = 2;
    static constexpr int ITEMS_AVAILABLE_BASE = 100;
    static constexpr int CHARACTERS_AVAILABLE_BASE = 200;

    SDL_Texture* exitButton;
    SDL_Texture* background;

    ShopState state = ShopState::CLOSED;
    Item* selectedItem = nullptr;


};
