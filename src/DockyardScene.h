#pragma once
#include "BaseScene.h"
#include "Dockyard.h"
#include "Party.h"
#include "SceneUtils.h"
#include <SDL3/SDL.h>

struct SDL_Texture;

class DockyardScene : public BaseScene
{
public:
    DockyardScene(Dockyard* dockyard, Party* allied);
    ~DockyardScene();

    void Load() override;
    void Update(float dt) override;
    void PostUpdate(float dt) override;
    void Unload() override;
    void LoadTextures() override;
    void LoadSound();

    bool OnUIMouseClickEvent(UIElement* uiElement) override;

    //helpers
    void OnResume() override;
    void OnPause() override;
    void CreateUI();

private:
    void PushDialogue();
    bool pendingDialogue = false;
    bool pendingPop = false;
    bool inputConsumed = false;

    Dockyard* dockyard; 
    Party* alliedParty;  

    GoldCounter goldCounter;

    //GoldCounter goldCounter;

    static constexpr int BACK_BUTTON_ID = 1;
    static constexpr int BACK_BUTTON_ID2 = 4;
    static constexpr int START_DIALOGUE = 2;
    static constexpr int IMPROVE_SHIP = 3;

#pragma region TEXTURES
    SDL_Texture* exitButton;
    SDL_Texture* background;
    SDL_Texture* chartNormal;
    SDL_Texture* chartImproved;
    SDL_Texture* improveShip;
    SDL_Texture* ownerSprite;

    SDL_Texture* moneyCounter;
    SDL_Texture* owner;
    SDL_Texture* fullBack;

#pragma endregion

    void CreateChartButtons();
    void DrawChartStats();

    bool showChart;
    bool shipImproved = false;
    int levelBeforeImprove = 1;
    static constexpr int COST_IMPROVE_SHIP = 50;

#pragma region POSITIONS

#pragma endregion Chart

    static const SDL_Rect CHART_BOUNDS;

#pragma endregion
#pragma endregion LEVEL

    static const SDL_Rect LEVEL1_BOUNDS;
    static const SDL_Rect LEVEL2_BOUNDS;

#pragma endregion
#pragma endregion STATS1

    static const SDL_Rect BACK_POWER_BOUNDS;

    static const SDL_Rect SIDE_POWER_BOUND;
    static const SDL_Rect SIDE_SPEED_BOUND;

    static const SDL_Rect FRONT_HEALTH_BOUND;
    static const SDL_Rect FRONT_SPEED_BOUND;

#pragma endregion
#pragma endregion STATS2

    static const SDL_Rect BACK_POWER_BOUNDS2;

    static const SDL_Rect SIDE_POWER_BOUND2;
    static const SDL_Rect SIDE_SPEED_BOUND2;

    static const SDL_Rect FRONT_HEALTH_BOUND2;
    static const SDL_Rect FRONT_SPEED_BOUND2;

#pragma endregion
#pragma endregion NPC

    static const SDL_Rect HUMAN_NPC_BOUNDS;
    static const SDL_Rect BIRD_NPC_BOUNDS;

#pragma endregion
#pragma endregion Sprite

    static const SDL_Rect HUMAN_SPRITE_BOUNDS;
    static const SDL_Rect BIRD_SPRITE_BOUNDS;
    static const SDL_Rect SIREN_SPRITE_BOUNDS;
    static const SDL_Rect REPTILE_SPRITE_BOUNDS;

#pragma endregion
#pragma endregion BackButton

    static const SDL_Rect BACK_BOUNDS;
    static const SDL_Rect BACK_BUTTON_BOUNDS;

#pragma endregion
#pragma endregion UpgradeButton

    static const SDL_Rect UPGRADE_BUTTON_BOUNDS;

#pragma endregion
#pragma endregion Gold Counter

    static const SDL_Rect GOLD_COUNTER_BOUNDS;
    static const SDL_Rect GOLD_COUNTER2_BOUNDS;

#pragma endregion
#pragma endregion

    //audio variables
    int buttonPress;
    int shipUpgrade;
};
