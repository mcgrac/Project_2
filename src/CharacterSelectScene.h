#pragma once
#include "BaseScene.h"
#include <vector>
#include <string>
#include <SDL3/SDL.h>
#include "Vector2D.h"
#include "CharacterFactory.h"

class Character;

struct SelectableCharacter
{
    std::string name;       // identificador — el mismo que usa CharacterFactory
    std::string label;      // nombre visible en pantalla
    bool selected;
    int labelRow;
    int panelRow;


    Vector2D labelPos;
    Vector2D panelPos;

    Character* chara;

    SelectableCharacter(const std::string& _name, const std::string& _label, int _labelRow, int _panelRow,
        Vector2D _labelPos, Vector2D _panelPos)
        : name(_name), label(_label), selected(false), labelRow(_labelRow), panelRow(_panelRow),
        labelPos(_labelPos), panelPos(_panelPos), chara(nullptr)
    {}
};

struct SDL_Texture;

class CharacterSelectScene : public BaseScene
{
public:

    CharacterSelectScene();

    void Load() override;
    void LoadSounds();
    void Update(float dt) override;
    void PostUpdate(float dt) override;
    void Unload() override;
    void LoadTextures() override;

    void UnloadTextures();
    bool OnUIMouseClickEvent(UIElement* uiElement) override;


    int backgroundSwitch = 0;
    std::string backName;

    //helpers
    void OnResume() override;
    void OnPause() override;
    void CreateUI();

private:
    // all available characters
    std::vector<SelectableCharacter> availableCharacters;

    // names of the three characters selected
    std::vector<std::string> selectedNames;
    //std::vector<Character*> createdCharacters;

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
    static constexpr int PANEL_FRAME_W = 1280;
    static constexpr int PANEL_FRAME_H = 720;

    SDL_Texture* spritesheetStartBtn;
    SDL_Texture* background;
    SDL_Texture* backgroundGerbera;
    SDL_Texture* backgroundIgnis;
    SDL_Texture* backgroundMarkus;
    SDL_Texture* backgroundTheresia;
    SDL_Texture* backgroundFatuus;
    SDL_Texture* backgroundJochi;
    SDL_Texture* backgroundGerbera1;
    SDL_Texture* backgroundIgnis1;
    SDL_Texture* backgroundMarkus1;
    SDL_Texture* backgroundTheresia1;
    SDL_Texture* backgroundFatuus1;
    SDL_Texture* backgroundJochi1;
    SDL_Texture* spritesheetCharacters;
    SDL_Texture* panelInformationSpritesheet;
    SDL_Texture* labelSpritesheets;
    SDL_Texture* backButtonSpritesheet;
    SDL_Texture* switchButton;
    
    std::vector<SDL_Texture*> tutorials;
    int tutorialIndex = 0;
    bool tutorialOpen = false;
    

    //audio variables
    int buttonPress;
    //variable that controls if stats should be shown
    bool switched = true;
};
