#pragma once
#include "BaseScene.h"
#include "Combat.h"
#include "Party.h"

struct SDL_Texture;

class CombatScene : public BaseScene
{
public:

    CombatScene(Party* allied);
    ~CombatScene();

    void Load() override;
    void Update(float dt) override;
    void PostUpdate(float dt) override;
    void Unload() override;
    void LoadTextures() override;

    bool OnUIMouseClickEvent(UIElement* uiElement) override;

private:
    Party* alliedParty;   // NO owner — pertenece a InGameScene
    Party* enemyParty;    // owner — creada y destruida por CombatScene
    Combat* combat;

    bool combatFinished;

    void CreateEnemyParty();
    void DestroyEnemyParty();

    SDL_Texture* background;
};
