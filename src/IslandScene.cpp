#include "IslandScene.h"
#include "IslandInteriorScene.h"
#include "CombatScene.h"
#include "Scene.h"
#include "Engine.h"
#include "Textures.h"
#include "UIManager.h"
#include "Render.h"
#include "Log.h"
#include "SaveLoad.h"

IslandScene::IslandScene(const Island& island, WorldMap* worldMap, Party* allied)
    : island(island)
    , worldMap(worldMap)
    , alliedParty(allied)
    , combatLaunched(false)
    , enterButton(nullptr)
    , pillageButon(nullptr)
    , background(nullptr)
{
    sceneName = "BeforeIslandScene";
}

IslandScene::~IslandScene() {}

void IslandScene::Load()
{
    LoadTextures();
    CreateUI();
}

void IslandScene::Update(float dt)
{
    if (island.GetType() == IslandType::HOSTILE && !combatLaunched)
    {
        combatLaunched = true;
        AttackIsland();
    }

    if (island.GetType() == IslandType::FRIENDLY)
    {
        Engine::GetInstance().render->DrawTexture(background, 0, 0);

        SDL_Rect panel = { 426, 300, 428, 360 };
        Engine::GetInstance().render->DrawRectangle(panel, 20, 20, 20, 200, true, false);

        Engine::GetInstance().render->DrawText(
            island.GetName().c_str(),
            486, 310, 308, 50,
            { 255, 255, 255, 255 }
        );
    }
}

void IslandScene::PostUpdate(float dt)
{

}

void IslandScene::Unload()
{
    Engine::GetInstance().textures->UnLoad(enterButton);
    Engine::GetInstance().textures->UnLoad(pillageButon);
    Engine::GetInstance().textures->UnLoad(background);
    Engine::GetInstance().textures->UnLoad(exitButton);
    Engine::GetInstance().uiManager->CleanUp();

}

void IslandScene::LoadTextures()
{
    enterButton = Engine::GetInstance().textures->Load("Assets/Textures/BeforeIslandScene/EnterButton.png");
    pillageButon = Engine::GetInstance().textures->Load("Assets/Textures/BeforeIslandScene/PillageButton.png");
    background = Engine::GetInstance().textures->Load("Assets/Textures/BeforeIslandScene/HumanBackground.png");
    exitButton = Engine::GetInstance().textures->Load("Assets/Textures/HumanIsland/BackButton.png");
}

bool IslandScene::OnUIMouseClickEvent(UIElement* uiElement)
{
    switch (uiElement->id)
    {
    case 0:
        Engine::GetInstance().scene->PopScene();
        break;
    case 1:
        EnterIsland();
        break;
    case 2:
        AttackIsland();
        break;
    default:
        break;
    }
    return true;
}

void IslandScene::EnterIsland()
{
    LOG("IslandScene: entrando en '%s'.", island.GetName().c_str());

    //save data
    SaveLoad::Save(alliedParty, worldMap->GetCurrentIslandId());

    Engine::GetInstance().scene->PushScene(new IslandInteriorScene(&island, alliedParty));
}

void IslandScene::AttackIsland()
{
    LOG("AttackIsland: alliedParty tiene %d miembros",
        alliedParty ? alliedParty->GetMemberCount() : -1);

    //save data
    SaveLoad::Save(alliedParty, worldMap->GetCurrentIslandId());

    if (island.GetType() == IslandType::FRIENDLY)
    {

        // Hacer hostil toda la facción en el WorldMap
        worldMap->MakeAllIslandsHostile(island.GetIslandFaction());
    }

    Party* allied = alliedParty;

    //eliminate island scene from stack 
    Engine::GetInstance().scene->PopScene();

    //push adds combat scene at the top of the stack (inGameScene is still alive)
    Engine::GetInstance().scene->PushScene(new CombatScene(allied));
}

void IslandScene::OnResume()
{
    CreateUI();
}

void IslandScene::OnPause()
{
    Engine::GetInstance().uiManager->CleanUp();
}

void IslandScene::CreateUI()
{
    if (island.GetType() == IslandType::HOSTILE)
    {
        //update:: pop scene combat
    }
    else
    {
        LOG("Create buttons in island Scene");
        //enter
        SDL_Rect enterBtnBounds = { 580, 400, 125, 72 };
        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON, 1, "Enter", enterBtnBounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, enterButton, 0, enterBtnBounds.w, enterBtnBounds.h
        );

        //attack
        SDL_Rect attackBtnBounds = { 580, 540, 125, 72 };
        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON, 2, "Attack", attackBtnBounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, pillageButon, 0, attackBtnBounds.w, attackBtnBounds.h
        );

        //back button
        SDL_Rect backBounds = { 20, 20, 72, 72 };
        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON, 0, "", backBounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, exitButton, 0, backBounds.w, backBounds.h
        );
    }
}