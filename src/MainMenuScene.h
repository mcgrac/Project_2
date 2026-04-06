#pragma once
#include "BaseScene.h"

enum class State {
    DEFAULT,        //Normal
    OPTIONS,        //show options panel
    HOW_TO_PLAY     //show tutorial panel
};

struct SDL_Texture;

class MainMenuScene : public BaseScene
{
public:
    MainMenuScene();
    ~MainMenuScene();

    void Load() override;
    void Update(float dt) override;
    void PostUpdate(float dt) override;
    void Unload() override;
    void LoadTextures() override;
    void LoadSounds();

    void UnloadTextures();
    void Draw(float dt);

    bool OnUIMouseClickEvent(UIElement* uiElement) override;

    //helpers
    void OnResume() override;
    void OnPause() override;
    void CreateUI();

private:
    State state;

    SDL_Texture* background;
    SDL_Texture* spritesheet;

    int buttonSelectionFx;
};