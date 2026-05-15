#pragma once
#include "BaseScene.h"
#include "Hostel.h"
#include "Party.h"
#include "SceneUtils.h"
#include <SDL3/SDL.h>

struct SDL_Texture;

class HostelScene : public BaseScene
{
public:
    HostelScene(Hostel* hostel, Party* allied);
    ~HostelScene();

    void Load() override;
    void LoadSound();
    void Update(float dt) override;
    void PostUpdate(float dt) override;
    void Unload() override;
    void LoadTextures() override;

    bool OnUIMouseClickEvent(UIElement* uiElement) override;

    //helpers
    void OnResume() override;
    void OnPause() override;
    void CreateUI();

    void OpenRestPanel();
    void OpenSelectCharaPanel();

    //helpers
    SDL_Rect GetRestBounds() const;
    SDL_Rect GetMealBounds() const;
    SDL_Rect GetOwnerBounds() const;



private:
    void PushDialogue();

    bool pendingDialogue = false;
    bool pendingPop = false;

    GoldCounter goldCounter;
    int goldCounterx = 986;
    int goldCountery = 201;

    Hostel* hostel;         
    Party* alliedParty;   

    static constexpr int BACK_BUTTON_ID = 1;
    static constexpr int START_DIALOGUE = 10;

#pragma region TEXTURES
    SDL_Texture* exitButton;
    SDL_Texture* background;
    SDL_Texture* fullBackground;
    SDL_Texture* ownerSprite;
    SDL_Texture* restButton;
    SDL_Texture* mealButton;
    SDL_Texture* emptyButtons;
#pragma endregion


#pragma region POSITIONS
#pragma region REST_BUTTON
    static const SDL_Rect REST_BOUNDS;
#pragma endregion
#pragma region MEAL_BUTTON
    static const SDL_Rect MEAL_BOUNDS;
#pragma endregion

    static const SDL_Rect HUMAN_CHARA_SELECT_BOUNDS;
    static const SDL_Rect BIRD_CHARA_SELECT_BOUNDS;
    static const SDL_Rect SIREN_CHARA_SELECT_BOUNDS;
    static const SDL_Rect REPTILE_CHARA_SELECT_BOUNDS;

#pragma endregion

    bool showRestPanel;
    bool showSelectCharaPanel;
    bool pendingRefresh;

    //audio variables
    int restfx;
    int buttonPress;
    bool hostelOpen = false;
};
