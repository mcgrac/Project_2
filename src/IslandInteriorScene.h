#pragma once
#include "BaseScene.h"
#include "Island.h"
#include "Party.h"

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

    bool OnUIMouseClickEvent(UIElement* uiElement) override;

    //helpers
    void OnResume() override;
    void OnPause() override;
    void CreateUI();

private:
    Island* island;
    Party* alliedParty;
    Ship* ship;

    static constexpr int SHOP_BUTTON_ID = 1;
    static constexpr int HOSTEL_BUTTON_ID = 2;
    static constexpr int DOCKYARD_BUTTON_ID = 3;
    static constexpr int LEAVE_BUTTON_ID = 4;

     //helper
    SDL_Rect GetDockBounds() const;
    SDL_Rect GetShopBounds() const;
    SDL_Rect GetHostelBounds() const;

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
#pragma endregion

#pragma region TEXTURES
    SDL_Texture* background;
    SDL_Texture* dockyardbutton;
    SDL_Texture* shopButton;
    SDL_Texture* hostelButton;
    SDL_Texture* exitButton;
#pragma endregion

};
