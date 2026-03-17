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

    LoadTextures();
}

SplashScene::~SplashScene(){

}

void SplashScene::Load()
{
    //Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/retro-gaming-short-248416.wav");

    // Crear botones del menú principal aquí
    // SDL_Rect btPos = { 520, 350, 120, 20 };
    // Engine::GetInstance().uiManager->CreateUIElement(...);

    // La lambda captura 'this' (MainMenuScene) y llama a OnUIMouseClickEvent.
    // UIElement no sabe nada de Module ni de BaseScene — solo guarda la función.
    //SDL_Rect btPos = { 520, 350, 120, 20 };
    //Engine::GetInstance().uiManager->CreateUIElement(
    //    UIElementType::BUTTON, 1, "Nueva Partida", btPos,
    //    [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }
    //);

    StartTimer();
}

void SplashScene::Update(float dt)
{
    RunTimer(dt);
    CheckTimer();
}

void SplashScene::PostUpdate(float dt)
{
    //draw textures
    Engine::GetInstance().render->DrawTexture(logo, Engine::GetInstance().window->width / 2, Engine::GetInstance().window->height / 2);
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
    switch (uiElement->id)
    {
    case 1: // Botón "Nueva Partida"
        LOG("MainMenu: Nueva Partida clicked!");
        Engine::GetInstance().scene->ReplaceScene(new MainMenuScene());
        break;
    default:
        break;
    }
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

void SplashScene::CheckTimer(){
    if (timer >= 3000.0f) {
        LOG("Chagne to main menu scene");
        Engine::GetInstance().scene->ReplaceScene(new MainMenuScene());
    }
}
