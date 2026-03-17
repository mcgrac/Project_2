#pragma once
#include "BaseScene.h"
#include "Party.h"
#include <vector>
#include <string>

// IDs de personaje — definidos aquí para que CharacterSelectScene
// también pueda incluir este header sin circularidad
enum class CharacterID
{
    WARRIOR,
    MAGE,
    ROGUE
    // Añade más según tu juego
};

class InGameScene : public BaseScene
{
public:
    // Recibe los IDs de los 3 personajes seleccionados por el jugador
    InGameScene(std::vector<std::string> _characterNames);
    ~InGameScene();

    void Load() override;
    void Update(float dt) override;
    void PostUpdate(float dt) override;
    void Unload() override;

    bool OnUIMouseClickEvent(UIElement* uiElement) override;

    // Acceso a la party aliada para otros sistemas (CombatScene, TeamScene...)
    Party* GetAlliedParty() { return alliedParty; }

private:
    std::vector<std::string> characterNames;

    // InGameScene es owner de la party y los characters — los crea y destruye
    Party* alliedParty;

    void DestroyParty();
};
