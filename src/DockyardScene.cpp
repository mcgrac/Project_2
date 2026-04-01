#include "DockyardScene.h"
#include "Engine.h"
#include "Scene.h"
#include "UIManager.h"
#include "Render.h"
#include "Log.h"

DockyardScene::DockyardScene(Dockyard dockyard, Party* allied)
    : dockyard(dockyard), alliedParty(allied)
{
}

DockyardScene::~DockyardScene() {}

void DockyardScene::Load()
{
    LOG("DockyardScene: cargando astillero.");

    SDL_Rect backBounds = { 20, 20, 160, 40 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, BACK_BUTTON_ID, "Volver", backBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }
    );
}

void DockyardScene::Update(float dt)
{
    // TODO: lógica de mejora y reparación del barco
}

void DockyardScene::PostUpdate(float dt)
{
    Engine::GetInstance().render->DrawText(
        "ASTILLERO", 520, 50, 240, 40, { 255, 255, 255, 255 }
    );

    //Engine::GetInstance().render->DrawText(
    //    "Oro: " + std::to_string(alliedParty->GetGold()),
    //    20, 80, 200, 30,
    //    { 255, 215, 0, 255 }
    //);
}

void DockyardScene::Unload()
{
    Engine::GetInstance().uiManager->CleanUp();
}

void DockyardScene::LoadTextures()
{

}

bool DockyardScene::OnUIMouseClickEvent(UIElement* uiElement)
{
    switch (uiElement->id)
    {
    case BACK_BUTTON_ID:
        Engine::GetInstance().scene->PopScene();
        break;
    default:
        break;
    }
    return true;
}
