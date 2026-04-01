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

private:
    Island island;       // copia de la isla (datos de solo lectura)
    WorldMap* worldMap;     // NO owner
    Party* alliedParty;  // NO owner

    bool combatLaunched;

    // Entra en la isla (lógica futura: tienda, NPCs, etc.)
    void EnterIsland();

    // Ataca la isla: hace hostil la facción y lanza combate
    void AttackIsland();

    SDL_Texture* spritesheetButtons;
};
