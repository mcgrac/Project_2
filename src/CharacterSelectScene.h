#pragma once
#include "BaseScene.h"
#include <vector>
#include <string>
#include <SDL3/SDL.h>


// Representa un personaje disponible en la pantalla de selección
struct SelectableCharacter
{
    std::string name;       // identificador — el mismo que usa CharacterFactory
    std::string label;      // nombre visible en pantalla
    SDL_Rect bounds;     // área clickable
    bool selected;

    SelectableCharacter(const std::string& _name, const std::string& _label, SDL_Rect _bounds)
        : name(_name), label(_label), bounds(_bounds), selected(false) {}
};

struct SDL_Texture;

class CharacterSelectScene : public BaseScene
{
public:
    CharacterSelectScene();

    void Load() override;
    void Update(float dt) override;
    void PostUpdate(float dt) override;
    void Unload() override;
    void LoadTextures() override;

    bool OnUIMouseClickEvent(UIElement* uiElement) override;

private:
    // all available characters
    std::vector<SelectableCharacter> availableCharacters;

    // names of the three characters selected
    std::vector<std::string> selectedNames;

    // Comprueba si el jugador ha clickado sobre algún personaje
    void HandleCharacterClick(int mouseX, int mouseY);

    // Activa/desactiva la selección de un personaje
    void ToggleSelection(int index);

    // Dibuja el estado actual de la selección
    void RenderSelection();

    // True si ya hay 3 personajes seleccionados
    bool HasFullTeam() const { return selectedNames.size() == 3; }

    // Lanza InGameScene con los 3 seleccionados
    void ConfirmSelection();

    SDL_Texture* spritesheet;
};
