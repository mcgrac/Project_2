#pragma once
#include "BaseScene.h"
#include "Combat.h"
#include "Party.h"

class CombatScene : public BaseScene
{
public:
    // Recibe la party aliada de InGameScene y crea una enemiga de prueba internamente
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
};
