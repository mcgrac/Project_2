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

    bool OnUIMouseClickEvent(UIElement* uiElement) override;
private:
    void StartTimer();
    void RunTimer(float dt);
    void LoadTextures();
    void CheckTimer();

    float timer;

    SDL_Texture* logo;
};