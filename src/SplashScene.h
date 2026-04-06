#pragma once
#include "BaseScene.h"

struct SDL_Texture;

class SplashScene : public BaseScene
{
public:
    SplashScene();
    ~SplashScene();

    void Load() override;
    void Update(float dt) override;
    void PostUpdate(float dt) override;
    void Unload() override;
    void LoadTextures() override;
    void LoadSounds();

    bool OnUIMouseClickEvent(UIElement* uiElement) override;

private:
    void StartTimer();
    void RunTimer(float dt);
    void CheckTimer();

    float timer;

    SDL_Texture* logo;
    int mewCatFx;
};