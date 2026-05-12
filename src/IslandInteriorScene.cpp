#include "IslandInteriorScene.h"
#include "ShopScene.h"
#include "HostelScene.h"
#include "DockyardScene.h"
#include "Engine.h"
#include "Audio.h"
#include "Textures.h"
#include "Scene.h"
#include "UIManager.h"
#include "Render.h"
#include "Log.h"
#include "Ship.h"
#include "SceneUtils.h"

#pragma region POSITIONS
#pragma region DOCK
const SDL_Rect IslandInteriorScene::DOCK_HUMAN_BOUNDS = { 780,  0,  507,  720 };
const SDL_Rect IslandInteriorScene::DOCK_BIRD_BOUNDS = { 5,  6,  7,  8 };
const SDL_Rect IslandInteriorScene::DOCK_SIREN_BOUNDS = { 9,  10, 11, 12 };
const SDL_Rect IslandInteriorScene::DOCK_REPTILE_BOUNDS = { 300, 300, 342, 366 };
#pragma endregion

#pragma region SHOP
const SDL_Rect IslandInteriorScene::SHOP_HUMAN_BOUNDS = { 20,  470,  270,  204 };
const SDL_Rect IslandInteriorScene::SHOP_BIRD_BOUNDS = { 5,  6,  7,  8 };
const SDL_Rect IslandInteriorScene::SHOP_SIREN_BOUNDS = { 9,  10, 11, 12 };
const SDL_Rect IslandInteriorScene::SHOP_REPTILE_BOUNDS = { 13, 14, 15, 16 };
#pragma endregion

#pragma region HOSTEL
const SDL_Rect IslandInteriorScene::HOSTEL_HUMAN_BOUNDS = { 200,  55,  492,  435 };
const SDL_Rect IslandInteriorScene::HOSTEL_BIRD_BOUNDS = { 5,  6,  7,  8 };
const SDL_Rect IslandInteriorScene::HOSTEL_SIREN_BOUNDS = { 9,  10, 11, 12 };
const SDL_Rect IslandInteriorScene::HOSTEL_REPTILE_BOUNDS = { 13, 14, 15, 16 };
#pragma endregion
#pragma endregion

IslandInteriorScene::IslandInteriorScene(Island* island, Party* allied, Ship* _ship)
    : island(island)
    , alliedParty(allied)
    , ship(_ship)
    , background(nullptr)
    , dockyardbutton(nullptr)
    , shopButton(nullptr)
    , hostelButton(nullptr)
    , exitButton(nullptr)
{
    sceneName = "islandInterior";
}

IslandInteriorScene::~IslandInteriorScene() {}


void IslandInteriorScene::Load()
{
    LoadTextures();
    LoadSound();
    CreateUI();
}

void IslandInteriorScene::Update(float dt) {
    Engine::GetInstance().render->DrawTexture(background, 0, 0);
    UpdateSound();

}

void IslandInteriorScene::PostUpdate(float dt)
{
    // Nombre de la isla
    Engine::GetInstance().render->DrawText(
        island->GetName().c_str(),
        440, 180, 400, 50,
        { 255, 255, 255, 255 }
    );
}

void IslandInteriorScene::Unload()
{
    Engine::GetInstance().textures->UnLoad(background);
    Engine::GetInstance().textures->UnLoad(dockyardbutton);
    Engine::GetInstance().textures->UnLoad(shopButton);
    Engine::GetInstance().textures->UnLoad(hostelButton);
    Engine::GetInstance().textures->UnLoad(exitButton);

    Engine::GetInstance().uiManager->CleanUp();
}

void IslandInteriorScene::unloadSound() {
    
}

void IslandInteriorScene::LoadTextures()
{
    exitButton = Engine::GetInstance().textures->Load("Assets/Textures/HumanIsland/BackButton.png");

    std::string backgroundPath = SceneUtils::GetIslandTexturePath( island->GetIslandFaction());
    background = Engine::GetInstance().textures->Load(backgroundPath.c_str());

    std::string dockyardPath = SceneUtils::GetBuildingTexturePath(island->GetIslandFaction(), "Dockyard");
    dockyardbutton = Engine::GetInstance().textures->Load(dockyardPath.c_str());

    std::string shopPath = SceneUtils::GetBuildingTexturePath(island->GetIslandFaction(), "Shop");
    shopButton = Engine::GetInstance().textures->Load(shopPath.c_str());

    std::string hostelPath = SceneUtils::GetBuildingTexturePath(island->GetIslandFaction(), "Hostel");         
    hostelButton = Engine::GetInstance().textures->Load(hostelPath.c_str());
}


bool IslandInteriorScene::OnUIMouseClickEvent(UIElement* uiElement)
{
    //play button Press when pressing a button
    Engine::GetInstance().audio->PlayFx(buttonPress);
    switch (uiElement->id)
    {
    case SHOP_BUTTON_ID:
        LOG("IslandInteriorScene: abriendo tienda.");
        Engine::GetInstance().scene->PushScene(
            new ShopScene(island->GetShop(), alliedParty)
        );
        break;

    case HOSTEL_BUTTON_ID:
        LOG("IslandInteriorScene: abriendo hostel.");
        Engine::GetInstance().scene->PushScene(
            new HostelScene(island->GetHostel(), alliedParty)
        );
        break;

    case DOCKYARD_BUTTON_ID:
        LOG("IslandInteriorScene: abriendo astillero.");
        island->GetDockyard()->AssignShip(ship);
        Engine::GetInstance().scene->PushScene(
            new DockyardScene(island->GetDockyard(), alliedParty)
        );
        break;

    case LEAVE_BUTTON_ID:
        // Volver al mapa (IslandScene → InGameScene)
        Engine::GetInstance().scene->PopScene();
        break;

    default:
        break;
    }
    return true;
}

void IslandInteriorScene::OnResume()
{
    CreateUI();
}

void IslandInteriorScene::OnPause()
{
    Engine::GetInstance().uiManager->CleanUp();
}

void IslandInteriorScene::CreateUI()
{
    LOG("IslandInteriorScene: entrando en '%s'.", island->GetName().c_str());

    SDL_Rect shopBounds = GetShopBounds();
    SDL_Rect hostelBounds = GetHostelBounds();
    SDL_Rect dockyardBounds = GetDockBounds();
    SDL_Rect leaveBounds = { 20, 20, 72, 72 };

    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, SHOP_BUTTON_ID, "", shopBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, shopButton, 0, shopBounds.w, shopBounds.h
    );

    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, HOSTEL_BUTTON_ID, "", hostelBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, hostelButton, 0, hostelBounds.w, hostelBounds.h
    );

    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, DOCKYARD_BUTTON_ID, "", dockyardBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, dockyardbutton, 0, dockyardBounds.w, dockyardBounds.h
    );

    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, LEAVE_BUTTON_ID, "", leaveBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, exitButton, 0, leaveBounds.w, leaveBounds.h
    );
}

//helpers
SDL_Rect IslandInteriorScene::GetDockBounds() const
{
    switch (island->GetIslandFaction())
    {
    case IslandFaction::HUMANS:   return DOCK_HUMAN_BOUNDS;
    case IslandFaction::BIRD:     return DOCK_BIRD_BOUNDS;
    case IslandFaction::SIRENS:   return DOCK_SIREN_BOUNDS;
    case IslandFaction::REPTILES: return DOCK_REPTILE_BOUNDS;
    default:                      return DOCK_HUMAN_BOUNDS;
    }
}

SDL_Rect IslandInteriorScene::GetShopBounds() const
{
    switch (island->GetIslandFaction())
    {
    case IslandFaction::HUMANS:   return SHOP_HUMAN_BOUNDS;
    case IslandFaction::BIRD:     return SHOP_BIRD_BOUNDS;
    case IslandFaction::SIRENS:   return SHOP_SIREN_BOUNDS;
    case IslandFaction::REPTILES: return SHOP_REPTILE_BOUNDS;
    default:                      return SHOP_HUMAN_BOUNDS;
    }
}

SDL_Rect IslandInteriorScene::GetHostelBounds() const
{
    switch (island->GetIslandFaction())
    {
    case IslandFaction::HUMANS:   return HOSTEL_HUMAN_BOUNDS;
    case IslandFaction::BIRD:     return HOSTEL_BIRD_BOUNDS;
    case IslandFaction::SIRENS:   return HOSTEL_SIREN_BOUNDS;
    case IslandFaction::REPTILES: return HOSTEL_REPTILE_BOUNDS;
    default:                      return HOSTEL_HUMAN_BOUNDS;
    }
}

void IslandInteriorScene::LoadSound() {
    buttonPress = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/UIfx/button_press.wav");
}

void IslandInteriorScene::UpdateSound() {
    //ckeck for type of island to put ambience
    if (!Engine::GetInstance().audio->IsMusicPlaying()) {
        LOG("Play music again!");
        switch (island->GetIslandFaction()) {
        case IslandFaction::HUMANS:
            //play crowd sounds
            Engine::GetInstance().audio->PlayMusic(humanAmb, 0);
            break;
        case IslandFaction::SIRENS:
            //play wave sounds
            Engine::GetInstance().audio->PlayMusic(sirenAmb, 0);
            break;
        case IslandFaction::REPTILES:
            //play jungle sounds
            Engine::GetInstance().audio->PlayMusic(reptileAmb, 0);
            break;
        }
    }
}
