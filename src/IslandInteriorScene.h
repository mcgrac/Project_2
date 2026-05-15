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

    bool OnUIMouseClickEvent(UIElement* uiElement) override;

    //helpers
    void OnResume() override;
    void OnPause() override;
    void CreateUI();

    void LoadAnimation();
    SDL_Texture* backgroundSpritesheet = nullptr;
    AnimationSet anims;

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

     //helper
    SDL_Rect GetDockBounds() const;
    SDL_Rect GetShopBounds() const;
    SDL_Rect GetHostelBounds() const;
    SDL_Rect GetChestBounds() const;

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
#pragma endregion
#pragma endregion

#pragma region TEXTURES
    SDL_Texture* background;
    SDL_Texture* dockyardbutton;
    SDL_Texture* shopButton;
    SDL_Texture* hostelButton;
    SDL_Texture* chestButton;
    SDL_Texture* exitButton;
    SDL_Texture* moneyCounter;
#pragma endregion

    //audio variables
    int ambiance;
    bool isMusicChanged = false;
    void LoadSound();
    void unloadSound();
    void UpdateSound();

    //bool musicPlaying = false;

    //ambiance directory
    const char* humanAmb = "Assets/Audio/Music/Ambiance/crowd_talking.wav";
    const char* sirenAmb = "Assets/Audio/Music/Ambiance/ambiance_waves.wav";
    const char* reptileAmb = "Assets/Audio/Music/Ambiance/jungle_sounds.wav";

    //fx variables
    int buttonPress;
};
