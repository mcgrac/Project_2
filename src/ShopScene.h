#pragma once
#include "BaseScene.h"
#include "Shop.h"
#include "SDL3/SDL.h"
#include "EquippableItem.h"

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
    void DeleteUI();

   // Faction IslandFactionToFaction(IslandFaction fact);


private:

    void PushDialogue();
    void CreateItemButtons();
    void OpenUIChest();
    void CloseUIChest();
    void CreateCharacterSelectionUI();

    bool pendingDialogue = false;
    bool pendingPop = false;

    Shop* shop; 
    Party* alliedParty;

    static constexpr int BACK_BUTTON_ID = 1;
    static constexpr int OPEN_SHOP_BUTTON = 2;
    static constexpr int BUY_KEY = 3;
    static constexpr int BUY_POTION = 4;
    static constexpr int BACK_CHEST_BUTTON_ID = 5;
    static constexpr int OPEN_CHEST_ID = 6;
    static constexpr int CLOSE_CHEST_ID = 7;
    static constexpr int ITEMS_AVAILABLE_BASE = 100;
    static constexpr int CHARACTERS_AVAILABLE_BASE = 200;

#pragma region TEXTURES
    SDL_Texture* exitButton;
    SDL_Texture* background;    
    SDL_Texture* fullBackground;
    SDL_Texture* emptyButtons;
    SDL_Texture* keyButton;
    SDL_Texture* potionButton;
    SDL_Texture* ownerSprite;
    SDL_Texture* chestButton;
    std::vector<SDL_Texture*> loadedItemTextures;
#pragma endregion

    SDL_Rect GetChestBoundsShop() const;
    SDL_Rect GetOwnerBounds() const;

#pragma region POSITIONS
#pragma region CHEST
    static const SDL_Rect CHEST_HUMAN_BOUNDS;
    static const SDL_Rect CHEST_BIRD_BOUNDS;
    static const SDL_Rect CHEST_SIREN_BOUNDS;
    static const SDL_Rect CHEST_REPTILE_BOUNDS;
#pragma endregion
#pragma endregion NPC

    static const SDL_Rect HUMAN_CHARA_SELECT_BOUNDS;
    static const SDL_Rect BIRD_CHARA_SELECT_BOUNDS;
    static const SDL_Rect SIREN_CHARA_SELECT_BOUNDS;
    static const SDL_Rect REPTILE_CHARA_SELECT_BOUNDS;

#pragma endregion
#pragma endregion Cross

    static const SDL_Rect CROSS_BOUNDS;

#pragma endregion
#pragma endregion


    ShopState state = ShopState::CLOSED;
    Item* selectedItem = nullptr;

    //audio variables
    int spendMoneyfx;
    int buttonPress;
    void LoadSound();

};
