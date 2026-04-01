#include "ShopScene.h"
#include "Engine.h"
#include "Scene.h"
#include "UIManager.h"
#include "Render.h"
#include "Log.h"

ShopScene::ShopScene(Shop shop, Party* allied)
    : shop(shop), alliedParty(allied)
{
}

ShopScene::~ShopScene() {}

void ShopScene::Load()
{
    LOG("ShopScene: cargando tienda.");

    SDL_Rect backBounds = { 20, 20, 160, 40 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, BACK_BUTTON_ID, "Volver", backBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }
    );
}

void ShopScene::Update(float dt)
{
    // TODO: lógica de compra de items
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
    Engine::GetInstance().uiManager->CleanUp();
}

void ShopScene::LoadTextures()
{
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
