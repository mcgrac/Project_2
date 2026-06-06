#pragma once
#include "BaseScene.h"
#include "Island.h"
#include "Party.h"
#include "Animation.h"
#include "SceneUtils.h"

struct SDL_Texture;
struct SDL_Rect;

class Ship;

class IslandInteriorScene : public BaseScene
{
public:
    IslandInteriorScene(Island* island, Party* allied, Ship* _ship);
    ~IslandInteriorScene();

    void Load() override;
    void Update(float dt) override;
    void PostUpdate(float dt) override;
    void Unload() override;
    void LoadTextures() override;

    void Draw(float dt);
    void DrawChest(float dt);
    void DrawReward();

    bool OnUIMouseClickEvent(UIElement* uiElement) override;

    //helpers
    void OnResume() override;
    void OnPause() override;
    void CreateUI();
    void OpenUIChest();
    void GiveReward();
    void CreateCharacterSelectionUI();
    void LoadAnimation();

    SDL_Texture* backgroundSpritesheet = nullptr;
    SDL_Texture* chestSpritesheet;
    AnimationSet anims;
    AnimationSet animsChest;
    Item* selectedItem = nullptr;
    SDL_Texture* chestItemTexture = nullptr;

    //gold counter
    GoldCounter goldCounter;


private:
    Island* island;
    Party* alliedParty;
    Ship* ship;

    static constexpr int SHOP_BUTTON_ID = 1;
    static constexpr int HOSTEL_BUTTON_ID = 2;
    static constexpr int DOCKYARD_BUTTON_ID = 3;
    static constexpr int LEAVE_BUTTON_ID = 4;
    static constexpr int CHEST_BUTTON_ID = 5;
    static constexpr int CLOSE_CHEST_BUTTON_ID = 6;
    static constexpr int OPEN_BUTTON_ID = 8;
    static constexpr int REWARD_GOLD = 10;
    static constexpr int REWARD_EMPTY = 11;
    static constexpr int CHARACTERS_AVAILABLE_BASE = 200;

    int rewardAmount = 1;


     //helper
    SDL_Rect GetDockBounds() const;
    SDL_Rect GetShopBounds() const;
    SDL_Rect GetHostelBounds() const;
    SDL_Rect GetChestBounds() const;

    void PlayAnimation(float dt);


#pragma region POSITIONS
#pragma region DOCK
    static const SDL_Rect DOCK_HUMAN_BOUNDS;
    static const SDL_Rect DOCK_BIRD_BOUNDS;
    static const SDL_Rect DOCK_SIREN_BOUNDS;
    static const SDL_Rect DOCK_REPTILE_BOUNDS;
#pragma endregion

#pragma region SHOP
    static const SDL_Rect SHOP_HUMAN_BOUNDS;
    static const SDL_Rect SHOP_BIRD_BOUNDS;
    static const SDL_Rect SHOP_SIREN_BOUNDS;
    static const SDL_Rect SHOP_REPTILE_BOUNDS;
#pragma endregion

#pragma region HOSTEL
    static const SDL_Rect HOSTEL_HUMAN_BOUNDS;
    static const SDL_Rect HOSTEL_BIRD_BOUNDS;
    static const SDL_Rect HOSTEL_SIREN_BOUNDS;
    static const SDL_Rect HOSTEL_REPTILE_BOUNDS;
#pragma endregion

#pragma region CHEST
    static const SDL_Rect CHEST_HUMAN_BOUNDS;
    static const SDL_Rect CHEST_BIRD_BOUNDS;
    static const SDL_Rect CHEST_SIREN_BOUNDS;
    static const SDL_Rect CHEST_REPTILE_BOUNDS;

    static const SDL_Rect CHEST_ITEM_POSITIONS;

#pragma endregion
#pragma region Rewards

    static const SDL_Rect REWARD_BOUNDS;

#pragma endregion
#pragma endregion

#pragma region TEXTURES
    SDL_Texture* background;
    SDL_Texture* dockyardbutton;
    SDL_Texture* shopButton;
    SDL_Texture* hostelButton;
    SDL_Texture* chestButton;
    SDL_Texture* exitButton;
    SDL_Texture* emptyButtons = nullptr;
    SDL_Texture* claimButton = nullptr;
    SDL_Texture* chestBackground = nullptr; 
    SDL_Texture* keyCounter = nullptr;
    SDL_Texture* moneyCounter = nullptr;
    SDL_Texture* moneyCard = nullptr;
    SDL_Texture* emptyCard = nullptr;
    SDL_Texture* openButton;
#pragma endregion

    //audio variables
    int ambiance;
    bool isMusicChanged = false;
    void LoadSound();
    void unloadSound();
    void UpdateSound();

    std::string musicIsland;

    //fx variables
    int buttonPress;

    bool showChest;
    bool chestOpened;
    bool chestPopped;
    int animationPlaying = false;
};
