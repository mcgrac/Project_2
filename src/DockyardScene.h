#pragma once
#include "BaseScene.h"
#include "Dockyard.h"
#include "Party.h"

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
    Dockyard* dockyard; 
    Party* alliedParty;  

    static constexpr int BACK_BUTTON_ID = 1;
    static constexpr int START_DIALOGUE = 2;
    static constexpr int IMPROVE_SHIP = 3;

#pragma region TEXTURES
    SDL_Texture* exitButton;
    SDL_Texture* background;
    SDL_Texture* chartNormal;
    SDL_Texture* chartImproved;
    SDL_Texture* improveShip;
#pragma endregion

    void DrawChartStats();
    bool shipImproved = false;
    int levelBeforeImprove = 1;
    static constexpr int COST_IMPROVE_SHIP = 50;

#pragma region POSITIONS
    // Posición del chart en pantalla
    static constexpr int CHART_X = 50;
    static constexpr int CHART_Y = 100;

    // Offsets relativos al chart para cada stat
    static constexpr int CHART_BACK_POWER_OFFSET_Y = 125; //35
    static constexpr int CHART_SIDE_POWER_OFFSET_Y = 200;
    static constexpr int CHART_SIDE_SPEED_OFFSET_Y = 238;
    static constexpr int CHART_FRONT_SPEED_OFFSET_Y = 313;
    static constexpr int CHART_FRONT_HP_OFFSET_Y = 348;
    static constexpr int CHART_STAT_OFFSET_X_OLD = 300;
    static constexpr int CHART_STAT_OFFSET_X_NEW = 600;
    static constexpr int CHART_STAT_W = 40;
    static constexpr int CHART_STAT_H = 20;

    //offsets relativos al chart pora level
    static constexpr int CHART_LEVEL_X = 250;
    static constexpr int CHART_LEVEL_Y = 15;
    static constexpr int CHART_LEVEL_W = 30;
    static constexpr int CHART_LEVEL_H = 60;
    static constexpr int CHART_LEVEL_NEW_X = 370;

    //buttons
    static constexpr int IMPROVE_SHIP_X = 715;
    static constexpr int IMPROVE_SHIP_Y = 200;
    static constexpr int IMPROVE_SHIP_W = 183;
    static constexpr int IMPROVE_SHIP_H = 63;

    static constexpr int NPC_X = 800;
    static constexpr int NPC_Y = 20;
    static constexpr int NPC_W = 549;
    static constexpr int NPC_H = 616;
#pragma endregion

    //audio variables
    int buttonPress;
    int shipUpgrade;
};
