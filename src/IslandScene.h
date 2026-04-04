#pragma once
#include "BaseScene.h"
#include "Island.h"
#include "WorldMap.h"
#include "Party.h"

class IslandScene : public BaseScene
{
public:
    IslandScene(const Island& island, WorldMap* worldMap, Party* allied);
    ~IslandScene();

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
    Island island;       
    WorldMap* worldMap;     
    Party* alliedParty;  

    bool combatLaunched;

    // Entra en la isla (lógica futura: tienda, NPCs, etc.)
    void EnterIsland();

    // Ataca la isla: hace hostil la facción y lanza combate
    void AttackIsland();

    SDL_Texture* enterButton;
    SDL_Texture* pillageButon;
    SDL_Texture* background;
    SDL_Texture* exitButton;
};
