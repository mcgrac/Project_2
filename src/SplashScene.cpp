#include "SplashScene.h"
#include "InGameScene.h"
#include "MainMenuScene.h"
#include "Scene.h"
#include "Render.h"
#include "Textures.h"
#include "Engine.h"
#include "Audio.h"
#include "UIManager.h"
#include "Log.h"
#include "Window.h"

SplashScene::SplashScene(){
    timer = 0.0f;
    logo = nullptr;
    sceneName = "SplashScene";
}

SplashScene::~SplashScene(){

}

void SplashScene::Load()
{
    StartTimer();
    LoadTextures();
    LoadSounds();

    //play audio once
    Engine::GetInstance().audio->PlayFx(mewCatFx);
}

void SplashScene::Update(float dt)
{
    RunTimer(dt);
    CheckTimer();
}

void SplashScene::PostUpdate(float dt)
{
    //draw logo in the middle
    float xMidPos = Engine::GetInstance().window->width / 2;
    float yMidPos = Engine::GetInstance().window->height / 2;
    Engine::GetInstance().render->DrawTexture(logo, xMidPos - logo->w / 2, yMidPos-logo->h / 2);
}

void SplashScene::Unload()
{
    //UI
    Engine::GetInstance().uiManager->CleanUp();

    //textures
    Engine::GetInstance().textures->UnLoad(logo);
}

bool SplashScene::OnUIMouseClickEvent(UIElement* uiElement)
{
    //empty no buttons
    return true;
}

void SplashScene::StartTimer(){
    timer = 0.0f;
}

void SplashScene::RunTimer(float dt){
    timer += dt;
    LOG("Timer: %f", timer);
}

void SplashScene::LoadTextures(){

    logo = Engine::GetInstance().textures->Load("Assets/Textures/UI/Logo.png");
    if (logo != nullptr) {
        LOG("Logo texture loaded succesfully");
    }
    else {
        LOG("Error loading logo texture");
    }
}

void SplashScene::LoadSounds()
{
    mewCatFx = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/MewCat.wav");
}

void SplashScene::CheckTimer(){
    if (timer >= 1500.0f) {
        LOG("Chagne to main menu scene");
        Engine::GetInstance().scene->ReplaceScene(new MainMenuScene());
    }
}
