#include "MainMenuScene.h"
#include "InGameScene.h"
#include "CharacterSelectScene.h"
#include "Scene.h"
#include "Engine.h"
#include "Audio.h"
#include "UIManager.h"
#include "Log.h"
#include "Window.h"
#include "Render.h"
#include "Textures.h"


MainMenuScene::MainMenuScene() : background(nullptr)
{
    state = State::DEFAULT;
}

MainMenuScene::~MainMenuScene()
{

}

void MainMenuScene::Load()
{
    Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/retro-gaming-short-248416.wav");

    // Crear botones del menú principal aquí
    // SDL_Rect btPos = { 520, 350, 120, 20 };
    // Engine::GetInstance().uiManager->CreateUIElement(...);

    // La lambda captura 'this' (MainMenuScene) y llama a OnUIMouseClickEvent.
    // UIElement no sabe nada de Module ni de BaseScene — solo guarda la función.
    SDL_Rect btPos1 = { 520, 350, 120, 20 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 1, "Nueva Partida", btPos1,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }
    );

    SDL_Rect btPos2 = { 520, 300, 120, 20 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 2, "Options", btPos2,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }
    );

    SDL_Rect btPos3 = { 520, 400, 120, 20 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 3, "Change fullscreen", btPos3,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }
    );

    //textures
    LoadTextures();
}

void MainMenuScene::Update(float dt)
{
    // Lógica de update del menú principal
    Draw(dt);
}

void MainMenuScene::PostUpdate(float dt)
{

    // Render / input tardío del menú principal
}

void MainMenuScene::Unload()
{
    Engine::GetInstance().uiManager->CleanUp();
    UnloadTextures();
}

void MainMenuScene::LoadTextures()
{
    background = Engine::GetInstance().textures->Load("Assets/Textures/Backgrounds/StartScreenBackground.png");

}

void MainMenuScene::UnloadTextures()
{
    Engine::GetInstance().textures->UnLoad(background);
}

void MainMenuScene::Draw(float dt)
{
    Engine::GetInstance().render->DrawTexture(background, 0, 0);
}

bool MainMenuScene::OnUIMouseClickEvent(UIElement* uiElement)
{
    switch (uiElement->id)
    {
    case 1: // new game button
        LOG("MainMenu: Nueva Partida clicked!");
        Engine::GetInstance().scene->ReplaceScene(new CharacterSelectScene());
        break;
    case 2: //options button
        state = State::OPTIONS;
        break;
    case 3: //fullscreen button
        Engine::GetInstance().window->ToggleFullscreen();
        LOG("ToggleFullscreen llamado, ahora llamando UpdateCamera");
        Engine::GetInstance().render->UpdateCamera();
        break;
    default:
        break;
    }
    return true;
}
