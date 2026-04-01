#include "IslandInteriorScene.h"
#include "ShopScene.h"
#include "HostelScene.h"
#include "DockyardScene.h"
#include "Engine.h"
#include "Scene.h"
#include "UIManager.h"
#include "Render.h"
#include "Log.h"

IslandInteriorScene::IslandInteriorScene(Island* island, Party* allied)
    : island(island)
    , alliedParty(allied)
{
}

IslandInteriorScene::~IslandInteriorScene() {}


void IslandInteriorScene::Load()
{
    LOG("IslandInteriorScene: entrando en '%s'.", island->GetName().c_str());

    SDL_Rect shopBounds     = { 486, 250, 308, 119 };
    SDL_Rect hostelBounds   = { 486, 390, 308, 119 };
    SDL_Rect dockyardBounds = { 486, 530, 308, 119 };
    SDL_Rect leaveBounds    = { 20,  20,  160,  40 };

    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, SHOP_BUTTON_ID, "Tienda", shopBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }
    );

    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, HOSTEL_BUTTON_ID, "Hostel", hostelBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }
    );

    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, DOCKYARD_BUTTON_ID, "Astillero", dockyardBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }
    );

    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, LEAVE_BUTTON_ID, "Salir", leaveBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }
    );
}

void IslandInteriorScene::Update(float dt) {}

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
    Engine::GetInstance().uiManager->CleanUp();
}

void IslandInteriorScene::LoadTextures()
{
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
