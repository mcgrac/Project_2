#pragma once
#include "BaseScene.h"
#include "Party.h"

struct SDL_Texture;

class PauseScene : public BaseScene
{
public:
    PauseScene(Party* allied, int currentIslandId);
    ~PauseScene();

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
    Party* alliedParty;  
    int currentIslandId;

    SDL_Texture* buttons;
    SDL_Texture* background;
};
