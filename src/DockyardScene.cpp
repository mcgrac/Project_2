#include "DockyardScene.h"
#include "Engine.h"
#include "Scene.h"
#include "UIManager.h"
#include "Render.h"
#include "Textures.h"
#include "Log.h"
#include "DialogueScene.h"
#include "DialogueManager.h"
#include "Log.h"
#include "NPC.h"

DockyardScene::DockyardScene(Dockyard* dockyard, Party* allied)
    : dockyard(dockyard), alliedParty(allied), background(nullptr), exitButton(nullptr)
{
    sceneName = "DockyardScene";
}

DockyardScene::~DockyardScene() {}

void DockyardScene::Load()
{
    LOG("DockyardScene: cargando astillero.");
    LoadTextures();
    CreateUI();
}

void DockyardScene::Update(float dt)
{
    Engine::GetInstance().render->DrawTexture(background, 0, 0);

    // lógica de mejora y reparación del barco
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
    Engine::GetInstance().textures->UnLoad(exitButton);
    Engine::GetInstance().textures->UnLoad(background);

    Engine::GetInstance().uiManager->CleanUp();
}

void DockyardScene::LoadTextures()
{
    exitButton = Engine::GetInstance().textures->Load("Assets/Textures/HumanIsland/BackButton.png");
    background = Engine::GetInstance().textures->Load("Assets/Textures/HumanIsland/DocksMennu.png");
}

bool DockyardScene::OnUIMouseClickEvent(UIElement* uiElement)
{
    switch (uiElement->id)
    {
    case BACK_BUTTON_ID:
        Engine::GetInstance().scene->PopScene();
        break;
    case START_DIALOGUE:
    {
        NPC* npc = dockyard->GetOwner();
        if (npc == nullptr)
        {
            LOG("Hostel: NPC es nullptr");
            break;
        }
        else {
            LOG("Hostel: NPC correcto");
        }

        LOG("Dialogue id npc: %s", npc->GetDialogueId().c_str());
        Engine::GetInstance().scene->PushScene(
            new DialogueScene(npc->GetDialogueId(),
                [this]()
                {
                }
            )
        );
        break;
    }
    case IMPROVE_SHIP:
        dockyard->ImproveShip();
        break;
    default:
        break;
    }
    return true;
}

void DockyardScene::OnResume()
{
    CreateUI();
}

void DockyardScene::OnPause()
{
    Engine::GetInstance().uiManager->CleanUp();
}

void DockyardScene::CreateUI()
{
    SDL_Rect backBounds = { 20, 20, 72, 72 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, BACK_BUTTON_ID, "", backBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, exitButton, 0, backBounds.w, backBounds.h
    );

    SDL_Rect improveBounds = { 400, 400, 72, 72 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, IMPROVE_SHIP, "Improve Ship", improveBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, exitButton, 0, improveBounds.w, improveBounds.h
    );

    SDL_Rect talkBounds = { 500, 20, 549, 616 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, START_DIALOGUE, "", talkBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, dockyard->GetOwner()->GetTexture(), 0, talkBounds.w, talkBounds.h
    );
}