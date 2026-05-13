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
   

   // Faction IslandFactionToFaction(IslandFaction fact);


private:

    void PushDialogue();
    void CreateItemButtons();
    void OpenUIChest();

    void GiveReward();
   
    void CreateCharacterSelectionUI();

    bool pendingDialogue = false;
    bool pendingPop = false;
    bool animationPlaying = false;

    Shop* shop; 
    Party* alliedParty;

    static constexpr int BACK_BUTTON_ID = 1;
    static constexpr int OPEN_SHOP_BUTTON = 2;
    static constexpr int BUY_KEY = 3;
    static constexpr int BUY_POTION = 4;
    static constexpr int BACK_CHEST_BUTTON_ID = 5;
    static constexpr int OPEN_CHEST_ID = 6;
    static constexpr int CLOSE_CHEST_ID = 7;
    static constexpr int OPEN_BUTTON_ID = 8;
    static constexpr int CLOSE_SHOP_ID = 9;
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
    SDL_Texture* chestBackground;
    SDL_Texture* openButton;
    SDL_Texture* moneyCounter;
    SDL_Texture* otherCounter;
    SDL_Texture* keyCounter;
    SDL_Texture* characterSprite;
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
#pragma endregion Sprite

    static const SDL_Rect HUMAN_SPRITE_BOUNDS;
    static const SDL_Rect BIRD_SPRITE_BOUNDS;
    static const SDL_Rect SIREN_SPRITE_BOUNDS;
    static const SDL_Rect REPTILE_SPRITE_BOUNDS;

#pragma endregion
#pragma endregion Cross

    static const SDL_Rect CROSS_BOUNDS;

#pragma endregion
#pragma endregion Cross2

    static const SDL_Rect CROSS_BOUNDS2;

#pragma endregion

#pragma endregion OpenButton

    static const SDL_Rect OPEN_BUTTON_BOUNDS;

#pragma endregion
#pragma endregion


    ShopState state = ShopState::CLOSED;
    Item* selectedItem = nullptr;

    //audio variables
    int spendMoneyfx;
    int buttonPress;
    bool chestOpen = 0;
    bool shopOpen = 0;
    void LoadSound();

};
