#include "MainMenuScene.h"
#include "InGameScene.h"
#include "CharacterSelectScene.h"
#include "Scene.h"
#include "Engine.h"
#include "Audio.h"
#include "UIManager.h"
#include "Log.h"

void MainMenuScene::Load()
{
    Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/retro-gaming-short-248416.wav");

    // Crear botones del menú principal aquí
    // SDL_Rect btPos = { 520, 350, 120, 20 };
    // Engine::GetInstance().uiManager->CreateUIElement(...);

    // La lambda captura 'this' (MainMenuScene) y llama a OnUIMouseClickEvent.
    // UIElement no sabe nada de Module ni de BaseScene — solo guarda la función.
    SDL_Rect btPos = { 520, 350, 120, 20 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 1, "Nueva Partida", btPos,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }
    );
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

void MainMenuScene::LoadTextures(){

}

bool MainMenuScene::OnUIMouseClickEvent(UIElement* uiElement)
{
    switch (uiElement->id)
    {
    case 1: // Botón "Nueva Partida"
        LOG("MainMenu: Nueva Partida clicked!");
        Engine::GetInstance().scene->ReplaceScene(new CharacterSelectScene());
        break;
    default:
        break;
    }
    return true;
}
