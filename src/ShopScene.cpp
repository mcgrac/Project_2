#include "ShopScene.h"
#include "Engine.h"
#include "Scene.h"
#include "UIManager.h"
#include "Render.h"
#include "Log.h"
#include "Textures.h"

ShopScene::ShopScene(Shop shop, Party* allied)
    : shop(shop), alliedParty(allied)
{
    sceneName = "ShopScene";
}

ShopScene::~ShopScene() {}

void ShopScene::Load()
{
    LOG("ShopScene: cargando tienda.");
    LoadTextures();
    CreateUI();
}

void ShopScene::Update(float dt)
{
    Engine::GetInstance().render->DrawTexture(background, 0, 0);
    //lógica de compra de items
}

void ShopScene::PostUpdate(float dt)
{
    Engine::GetInstance().render->DrawText(
        "TIENDA", 540, 50, 200, 40, { 255, 255, 255, 255 }
    );

    //Engine::GetInstance().render->DrawText(
    //    "Gold: " + std::to_string(alliedParty->GetGold()),
    //    20, 80, 200, 30,
    //    { 255, 215, 0, 255 }
    //);
}

void ShopScene::Unload()
{
    Engine::GetInstance().textures->UnLoad(exitButton);
    Engine::GetInstance().textures->UnLoad(background);

    Engine::GetInstance().uiManager->CleanUp();
}

void ShopScene::LoadTextures()
{
    exitButton = Engine::GetInstance().textures->Load("Assets/Textures/HumanIsland/BackButton.png");
    background = Engine::GetInstance().textures->Load("Assets/Textures/HumanIsland/ShopBackground.png");
}

bool ShopScene::OnUIMouseClickEvent(UIElement* uiElement)
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
void ShopScene::OnResume()
{
    CreateUI();
}

void ShopScene::OnPause()
{
    Engine::GetInstance().uiManager->CleanUp();
}

void ShopScene::CreateUI()
{
    SDL_Rect backBounds = { 20, 20, 72, 72 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, BACK_BUTTON_ID, "", backBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, exitButton, 0, backBounds.w, backBounds.h
    );
}