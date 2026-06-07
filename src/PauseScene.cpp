#include "PauseScene.h"
#include "MainMenuScene.h"
#include "SaveLoad.h"
#include "Engine.h"
#include "Audio.h"
#include "Scene.h"
#include "UIManager.h"
#include "Render.h"
#include "Textures.h"
#include "Log.h"
#include "SettingsScene.h"

PauseScene::PauseScene(Party* allied, int currentIslandId, int _shipLevel)
    : alliedParty(allied)
    , currentIslandId(currentIslandId)
    , shipLevel(_shipLevel)
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
    //play music
    //background
    Engine::GetInstance().render->DrawTexture(background, 0, 0);

    // Panel
    SDL_Rect panel = { 426, 200, 428, 300 };
    Engine::GetInstance().render->DrawRectangle(panel, 10, 10, 10, 210, true, false);

    // Title
    Engine::GetInstance().render->DrawText(
        "PAUSED", 550, 50, 180, 30,
        { 255, 255, 255, 255 }
    );
}

void PauseScene::PostUpdate(float dt)
{

}

void PauseScene::Unload()
{
    Engine::GetInstance().textures->UnLoad(background);
    Engine::GetInstance().textures->UnLoad(buttons);

    Engine::GetInstance().uiManager->CleanUp();
    LOG("PauseScene: cerrado.");
}

void PauseScene::LoadTextures()
{
    buttons = Engine::GetInstance().textures->Load("Assets/Textures/Pause/ButtonsPause.png");
    background = Engine::GetInstance().textures->Load("Assets/Textures/Pause/AlternativeBackground.png");
}

void PauseScene::LoadSound() {
    buttonPress = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/UIfx/button_press.wav");
}

bool PauseScene::OnUIMouseClickEvent(UIElement* uiElement)
{
    Engine::GetInstance().audio->PlayFx(buttonPress);
    switch (uiElement->id)
    {
    case 1:
        LOG("PauseScene: continue.");
        Engine::GetInstance().scene->PopScene();
        break;

    case 2:
        LOG("PauseScene: opciones.");
        Engine::GetInstance().scene->PushScene(new SettingsScene());
        break;

    case 3:
        LOG("PauseScene: Save and return main menu.");
        SaveLoad::Save(alliedParty, currentIslandId, shipLevel);
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
    SDL_Rect continueBounds = { 550, 200, 221, 85 };
    SDL_Rect optionsBounds = { 550, 300, 221, 85 };
    SDL_Rect mainMenuBounds = { 550, 400, 221, 85 };

    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 1, "", continueBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, buttons, 0, continueBounds.w, continueBounds.h
    );

    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 2, "", optionsBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, buttons, 1, optionsBounds.w, optionsBounds.h
    );

    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 3, "", mainMenuBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, buttons, 2, mainMenuBounds.w, mainMenuBounds.h
    );
}