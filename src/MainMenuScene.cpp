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
#include "SaveLoad.h"

MainMenuScene::MainMenuScene() : background(nullptr), spritesheet(nullptr)
{
    state = State::DEFAULT;
    sceneName = "mainMenu";
}

MainMenuScene::~MainMenuScene()
{

}

void MainMenuScene::Load()
{
    Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/MainMenuScene.wav");

    //textures
    LoadTextures();
    //sounds
    LoadSounds();
    //UI
    CreateUI();
}

void MainMenuScene::Update(float dt)
{
    //check if music is playing
    if (!Engine::GetInstance().audio->IsMusicPlaying()) {
        LOG("Play music again!");
        Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/MainMenuScene.wav");
    }
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
    if (background) {
        LOG("Background buttons loaded succesfully");
    }

    spritesheet = Engine::GetInstance().textures->Load("Assets/Textures/UI/StartButtons.png");
    if (spritesheet) {
        LOG("Spritehseet buttons loaded succesfully");
    }

}

void MainMenuScene::LoadSounds()
{
    buttonSelectionFx = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/buttonSelection.wav");
}

void MainMenuScene::UnloadTextures()
{
    Engine::GetInstance().textures->UnLoad(background);
    Engine::GetInstance().textures->UnLoad(spritesheet);
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
        Engine::GetInstance().audio->PlayFx(buttonSelectionFx);

        SaveLoad::ClearSave(); //clear any data saved
        Engine::GetInstance().scene->ReplaceScene(new CharacterSelectScene());
        break;
        {
    case 2: //continue button
        Engine::GetInstance().audio->PlayFx(buttonSelectionFx);

        SaveData data = SaveLoad::Load();
        if (data.exists)
        {
            // Reconstruir los nombres de personajes del save
            std::vector<std::string> names;
            for (const auto& charSave : data.characters)
            {
                names.push_back(charSave.name);
            }
            Engine::GetInstance().scene->ReplaceScene(new InGameScene(names, true));
        }
        break;
        }
    case 3: //fullscreen button
        Engine::GetInstance().audio->PlayFx(buttonSelectionFx);

        Engine::GetInstance().window->ToggleFullscreen();
        LOG("ToggleFullscreen llamado, ahora llamando UpdateCamera");
        Engine::GetInstance().render->UpdateCamera();
        break;
    case 4: //quit button
        Engine::GetInstance().audio->PlayFx(buttonSelectionFx);
        //Engine::GetInstance().input->quit = true;
        //quit
        break;
    default:
        break;
    }
    return true;
}

void MainMenuScene::OnResume()
{
    CreateUI();
}

void MainMenuScene::OnPause()
{
    Engine::GetInstance().uiManager->CleanUp();
}

void MainMenuScene::CreateUI()
{
    //Play button
    SDL_Rect btPos1 = { 520, 200, 221, 85 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 1, "", btPos1,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); },
        {}, spritesheet, 0, btPos1.w, btPos1.h
    );

    //continue
    SDL_Rect btPos2 = { 520, 300, 221, 85 };
    auto continueBtn = Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 2, "", btPos2,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); },
        {}, spritesheet, 1, btPos2.w, btPos2.h
    );
    if (!SaveLoad::HasSaveFile())
    {
        continueBtn->state = UIElementState::DISABLED;
    }

    //options (change fullscreen por ahora)
    SDL_Rect btPos3 = { 520, 400, 221, 85 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 3, "", btPos3,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); },
        {}, spritesheet, 2, btPos3.w, btPos3.h
    );

    //Quit
    SDL_Rect btPos4 = { 520, 500, 221, 85 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 4, "", btPos4,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); },
        {}, spritesheet, 3, btPos4.w, btPos4.h
    );
}
