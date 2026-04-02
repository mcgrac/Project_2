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
    , spritesheetButtons(nullptr)
{
    LOG("IslandScene constructor: alliedParty tiene %d miembros",
        alliedParty ? alliedParty->GetMemberCount() : -1);
}

IslandScene::~IslandScene() {}

void IslandScene::Load()
{
    LoadTextures();

    if (island.GetType() == IslandType::HOSTILE)
    {
        //update:: pop scene combat
    }
    else
    {
        LOG("Create buttons in island Scene");

        SDL_Rect enterBtnBounds  = { 486, 350, 308, 119 };
        SDL_Rect attackBtnBounds = { 486, 490, 308, 119 };

        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON, 1, "Enter", enterBtnBounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, spritesheetButtons, 0, enterBtnBounds.w, enterBtnBounds.h
        );

        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON, 2, "Attack", attackBtnBounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, spritesheetButtons, 0, attackBtnBounds.w, attackBtnBounds.h
        );
    }


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
        SDL_Rect panel = { 426, 300, 428, 360 };
        Engine::GetInstance().render->DrawRectangle(panel, 20, 20, 20, 200, true, false);

        Engine::GetInstance().render->DrawText(
            island.GetName().c_str(),
            486, 310, 308, 30,
            { 255, 255, 255, 255 }
        );
    }
}

void IslandScene::PostUpdate(float dt)
{

}

void IslandScene::Unload()
{
    Engine::GetInstance().uiManager->CleanUp();
    Engine::GetInstance().textures->UnLoad(spritesheetButtons);
}

void IslandScene::LoadTextures()
{
    spritesheetButtons = Engine::GetInstance().textures->Load("Assets/Textures/IslandScene/MapButtons.png");
}

bool IslandScene::OnUIMouseClickEvent(UIElement* uiElement)
{
    switch (uiElement->id)
    {
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
