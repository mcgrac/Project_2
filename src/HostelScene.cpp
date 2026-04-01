#include "HostelScene.h"
#include "Engine.h"
#include "Scene.h"
#include "UIManager.h"
#include "Render.h"
#include "Log.h"

HostelScene::HostelScene(Hostel hostel, Party* allied)
    : hostel(hostel), alliedParty(allied)
{
}

HostelScene::~HostelScene() {}

void HostelScene::Load()
{
    LOG("HostelScene: cargando hostel.");

    SDL_Rect backBounds = { 20, 20, 160, 40 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, BACK_BUTTON_ID, "Volver", backBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }
    );
}

void HostelScene::Update(float dt)
{
    // TODO: lógica de descanso y compra de experiencia
}

void HostelScene::PostUpdate(float dt)
{
    Engine::GetInstance().render->DrawText(
        "HOSTEL", 540, 50, 200, 40, { 255, 255, 255, 255 }
    );

    //Engine::GetInstance().render->DrawText(
    //    "Oro: " + std::to_string(alliedParty->GetGold()),
    //    20, 80, 200, 30,
    //    { 255, 215, 0, 255 }
    //);
}

void HostelScene::Unload()
{
    Engine::GetInstance().uiManager->CleanUp();
}

void HostelScene::LoadTextures()
{
}

bool HostelScene::OnUIMouseClickEvent(UIElement* uiElement)
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
