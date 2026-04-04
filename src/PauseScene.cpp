#include "PauseScene.h"
#include "MainMenuScene.h"
#include "SaveLoad.h"
#include "Engine.h"
#include "Scene.h"
#include "UIManager.h"
#include "Render.h"
#include "Log.h"

PauseScene::PauseScene(Party* allied, int currentIslandId)
    : alliedParty(allied)
    , currentIslandId(currentIslandId)
{
    sceneName = "PauseScene";
}

PauseScene::~PauseScene() {}

void PauseScene::Load()
{
    LOG("PauseScene: abierto.");
    LoadTextures();
    CreateUI();
}

void PauseScene::Update(float dt) 
{
    // Panel semitransparente oscuro
    SDL_Rect panel = { 426, 200, 428, 300 };
    Engine::GetInstance().render->DrawRectangle(panel, 10, 10, 10, 210, true, false);

    // Título
    Engine::GetInstance().render->DrawText(
        "PAUSED", 550, 215, 180, 30,
        { 255, 255, 255, 255 }
    );
}

void PauseScene::PostUpdate(float dt)
{

}

void PauseScene::Unload()
{
    Engine::GetInstance().uiManager->CleanUp();
    LOG("PauseScene: cerrado.");
}

void PauseScene::LoadTextures()
{
    //load textures
}

bool PauseScene::OnUIMouseClickEvent(UIElement* uiElement)
{
    switch (uiElement->id)
    {
    case CONTINUE_BUTTON_ID:
        LOG("PauseScene: reanudando juego.");
        Engine::GetInstance().scene->PopScene();
        break;

    case OPTIONS_BUTTON_ID:
        LOG("PauseScene: opciones — pendiente de implementar.");
        break;

    case MAIN_MENU_BUTTON_ID:
        LOG("PauseScene: guardando y volviendo al menu principal.");
        SaveLoad::Save(alliedParty, currentIslandId);
        Engine::GetInstance().scene->ReplaceScene(new MainMenuScene());
        break;

    default:
        break;
    }
    return true;
}
void PauseScene::OnResume()
{
    CreateUI();
}

void PauseScene::OnPause()
{
    Engine::GetInstance().uiManager->CleanUp();
}

void PauseScene::CreateUI()
{
    SDL_Rect continueBounds = { 486, 250, 308, 50 };
    SDL_Rect optionsBounds = { 486, 320, 308, 50 };
    SDL_Rect mainMenuBounds = { 486, 390, 308, 50 };

    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, CONTINUE_BUTTON_ID, "Continue", continueBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }
    );

    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, OPTIONS_BUTTON_ID, "Options", optionsBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }
    );

    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, MAIN_MENU_BUTTON_ID, "Return to Main Menu", mainMenuBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }
    );
}