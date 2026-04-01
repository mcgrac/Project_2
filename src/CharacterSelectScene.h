#pragma once
#include "BaseScene.h"
#include <vector>
#include <string>
#include <SDL3/SDL.h>
#include "Vector2D.h"

struct SelectableCharacter
{
    std::string name;       // identificador — el mismo que usa CharacterFactory
    std::string label;      // nombre visible en pantalla
    //SDL_Rect bounds;     // área clickable
    bool selected;
    //int portraitCol;
    int labelRow;
    int panelRow;

    Vector2D labelPos;
    Vector2D panelPos;

    SelectableCharacter(const std::string& _name, const std::string& _label, int _labelRow, int _panelRow,
        Vector2D _labelPos, Vector2D _panelPos)
        : name(_name), label(_label), selected(false) , labelRow(_labelRow), panelRow (_panelRow),
        labelPos (_labelPos), panelPos(_panelPos)
    {}
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

    void UnloadTextures();
    bool OnUIMouseClickEvent(UIElement* uiElement) override;

private:
    // all available characters
    std::vector<SelectableCharacter> availableCharacters;

    // names of the three characters selected
    std::vector<std::string> selectedNames;

    // Activa/desactiva la selección de un personaje
    void ToggleSelection(int index);

    // Dibuja el estado actual de la selección
    void RenderSelection();

    // True si ya hay 3 personajes seleccionados
    bool HasFullTeam() const { return selectedNames.size() == 3; }

    // Lanza InGameScene con los 3 seleccionados
    void ConfirmSelection();

    void CreateCharactersButtons();
    void CreateInterfaceButtons();

    void SetPortraitButtonStatePressed(int index);
    void SetPortraitButtonStateNormal(int index);

    bool IsPortraitHoveredOrSelected(int index) const;

    // Dimensiones frames label spritesheet
    static constexpr int LABEL_FRAME_W = 336;
    static constexpr int LABEL_FRAME_H = 81;

    // Dimensiones frames panel spritesheet
    static constexpr int PANEL_FRAME_W = 396;
    static constexpr int PANEL_FRAME_H = 309;

    SDL_Texture* spritesheetStartBtn;
    SDL_Texture* background;
    SDL_Texture* spritesheetCharacters;
    SDL_Texture* panelInformationSpritesheet;
    SDL_Texture* labelSpritesheets;
    SDL_Texture* backButtonSpritesheet;
};
