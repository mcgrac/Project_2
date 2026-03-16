#include "MainMenuScene.h"
#include "Engine.h"
#include "Audio.h"
#include "UIManager.h"

void MainMenuScene::Load()
{
    Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/retro-gaming-short-248416.wav");

    // Crear botones del menú principal aquí
    // SDL_Rect btPos = { 520, 350, 120, 20 };
    // Engine::GetInstance().uiManager->CreateUIElement(...);
}

void MainMenuScene::Update(float dt)
{
    // Lógica de update del menú principal
}

void MainMenuScene::PostUpdate(float dt)
{
    // Render / input tardío del menú principal
}

void MainMenuScene::Unload()
{
    Engine::GetInstance().uiManager->CleanUp();
}
