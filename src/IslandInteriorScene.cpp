#include "IslandInteriorScene.h"
#include "ShopScene.h"
#include "HostelScene.h"
#include "DockyardScene.h"
#include "Engine.h"
#include "Textures.h"
#include "Scene.h"
#include "UIManager.h"
#include "Render.h"
#include "Log.h"

IslandInteriorScene::IslandInteriorScene(Island* island, Party* allied)
    : island(island)
    , alliedParty(allied)
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
    CreateUI();
}

void IslandInteriorScene::Update(float dt) {
    Engine::GetInstance().render->DrawTexture(background, 0, 0);

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

void IslandInteriorScene::LoadTextures()
{
    background = Engine::GetInstance().textures->Load("Assets/Textures/HumanIsland/Background.png");
    dockyardbutton = Engine::GetInstance().textures->Load("Assets/Textures/HumanIsland/DocksButton.png");
    shopButton = Engine::GetInstance().textures->Load("Assets/Textures/HumanIsland/ShopButton.png");
    hostelButton = Engine::GetInstance().textures->Load("Assets/Textures/HumanIsland/HostelButton.png");
    exitButton = Engine::GetInstance().textures->Load("Assets/Textures/HumanIsland/BackButton.png");
}


bool IslandInteriorScene::OnUIMouseClickEvent(UIElement* uiElement)
{
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

    SDL_Rect shopBounds = { 20, 470, 270, 204 };
    SDL_Rect hostelBounds = { 200, 55, 492, 435 };
    SDL_Rect dockyardBounds = { 780, 0, 507, 720 };
    SDL_Rect leaveBounds = { 20,  20,  72,  72 };

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
