#include "HostelScene.h"
#include "Engine.h"
#include "Scene.h"
#include "UIManager.h"
#include "Render.h"
#include "Log.h"
#include "Textures.h"

HostelScene::HostelScene(Hostel hostel, Party* allied)
    : hostel(hostel), alliedParty(allied)
{
    sceneName = "HostelScene";
}

HostelScene::~HostelScene() {}

void HostelScene::Load()
{
    LOG("HostelScene: cargando hostel.");
    LoadTextures();
    CreateUI();
}

void HostelScene::Update(float dt)
{
    Engine::GetInstance().render->DrawTexture(background, 0, 0);
    // lógica de descanso y compra de experiencia
}

void HostelScene::PostUpdate(float dt)
{
    Engine::GetInstance().render->DrawText(
        "HOSTEL", 540, 50, 200, 40, { 255, 255, 255, 255 }
    );
}

void HostelScene::Unload()
{
    Engine::GetInstance().uiManager->CleanUp();
}

void HostelScene::LoadTextures()
{
    exitButton = Engine::GetInstance().textures->Load("Assets/Textures/HumanIsland/BackButton.png");
    background = Engine::GetInstance().textures->Load("Assets/Textures/HumanIsland/HostelBackground.png");
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
void HostelScene::OnResume()
{
    CreateUI();
}

void HostelScene::OnPause()
{
    Engine::GetInstance().uiManager->CleanUp();
}

void HostelScene::CreateUI()
{
    SDL_Rect backBounds = { 20, 20, 72, 72 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, BACK_BUTTON_ID, "", backBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, exitButton, 0, backBounds.w, backBounds.h
    );
}