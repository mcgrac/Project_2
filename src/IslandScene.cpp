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

IslandScene::IslandScene(Island* island, WorldMap* worldMap, Party* allied, Ship* _ship)
    : island(island)
    , worldMap(worldMap)
    , alliedParty(allied)
    , ship (_ship)
    , combatLaunched(false)
    , enterButton(nullptr)
    , pillageButon(nullptr)
    , background(nullptr)
    , combatPending(false)
    , lastCombatWon(false)
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
    if (island == nullptr) { LOG("island es nullptr"); }

    if (island != nullptr) {
        if (island->GetType() == IslandType::HOSTILE && !combatLaunched)
        {
            combatLaunched = true;
            AttackIsland();

        }
        else if (island->GetType() == IslandType::FRIENDLY)
        {
            Engine::GetInstance().render->DrawTexture(background, 0, 0);

            SDL_Rect panel = { 426, 300, 428, 360 };
            Engine::GetInstance().render->DrawRectangle(panel, 20, 20, 20, 200, true, false);

            Engine::GetInstance().render->DrawText(
                island->GetName().c_str(),
                486, 310, 308, 50,
                { 255, 255, 255, 255 }
            );
        }
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

    //delete island;
    //island = nullptr;

}

void IslandScene::LoadTextures()
{
    enterButton = Engine::GetInstance().textures->Load("Assets/Textures/BeforeIslandScene/EnterButton.png");
    pillageButon = Engine::GetInstance().textures->Load("Assets/Textures/BeforeIslandScene/PillageButton.png");

    //-----island background--------
    std::string islandFaction;
    switch (island->GetIslandFaction())
    {
    case IslandFaction::HUMANS:
        islandFaction = "human";
        break;
    case IslandFaction::BIRD:
        islandFaction = "bird";
        break;
    case IslandFaction::SIRENS:
        islandFaction = "siren";
        break;
    case IslandFaction::REPTILES:
        islandFaction = "reptile";
        break;
    default:
        break;
    }
    std::string path = "Assets/Textures/BeforeIslandScene/" + islandFaction + ".png";
    background = Engine::GetInstance().textures->Load(path.c_str());
    //-------------------------------

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
    LOG("IslandScene: entrando en '%s'.", island->GetName().c_str());

    worldMap->ConfirmTravel();  // confirm travel

    //save data
    SaveLoad::Save(alliedParty, worldMap->GetCurrentIslandId());

    Engine::GetInstance().scene->PushScene(new IslandInteriorScene(island, alliedParty, ship));
}

void IslandScene::AttackIsland()
{
    LOG("AttackIsland: alliedParty tiene %d miembros",
        alliedParty ? alliedParty->GetMemberCount() : -1);

    //save data
    SaveLoad::Save(alliedParty, worldMap->GetCurrentIslandId());

    // Guardar posición del barco antes del combate
    shipPositionBeforeCombat = ship->GetPosition();

    if (island->GetType() == IslandType::FRIENDLY)
    {
        // Hacer hostil toda la facción en el WorldMap
        worldMap->MakeAllIslandsHostile(island->GetIslandFaction());
    }

    //Party* allied = alliedParty;
    //int levelShip = ship->GetLevel();
    //lastCombat = new CombatScene(alliedParty, ship->GetLevel());
    //eliminate island scene from stack 
    //Engine::GetInstance().scene->PopScene();

    combatPending = true;
    lastCombatWon = false;

    CombatScene* combat = new CombatScene(alliedParty, ship->GetLevel());

    combat->onCombatEnd = [this](bool won)
    {
            lastCombatWon = won;
    };

    Engine::GetInstance().scene->PushScene(combat);
    //Engine::GetInstance().scene->PushScene(new CombatScene(alliedParty, ship->GetLevel()));

    //push adds combat scene at the top of the stack (inGameScene is still alive)
    //Engine::GetInstance().scene->PushScene(new CombatScene(allied, levelShip));
    //LOG("AttackIsland: PushScene sin PopScene — lastCombat: %p", lastCombat);
    //Engine::GetInstance().scene->PushScene(lastCombat);
}

void IslandScene::OnResume()
{

    if (combatPending)
    {
        combatPending = false;

        LOG("IslandScene::OnResume — lastCombatWon: %d — currentIslandId: %d — pendingIslandId: %d",
            (int)lastCombatWon, worldMap->GetCurrentIslandId(), worldMap->GetPendingIslandId());

        if (lastCombatWon)
        {
            worldMap->ConfirmTravel();
        }
        else 
        {
            // Derrota: devolver el barco a la isla anterior
            ship->SetPosition(worldMap->GetShipReturnPosition());
        }

        // Defeat, Pop scene and don't confirm the travel, player remains in that island
        Engine::GetInstance().scene->PopScene();
        return;
    }

    CreateUI();
}

void IslandScene::OnPause()
{
    LOG("IslandScene::OnPause LLAMADO");
    Engine::GetInstance().uiManager->CleanUp();
}

void IslandScene::CreateUI()
{
    if (island->GetType() == IslandType::HOSTILE)
    {
        //update:: pop scene combat
    }
    else
    {
        LOG("Create buttons in island Scene");
        //enter
        SDL_Rect enterBtnBounds = { 580, 400, 125, 72 };
        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON, 1, "", enterBtnBounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, enterButton, 0, enterBtnBounds.w, enterBtnBounds.h
        );

        //attack
        SDL_Rect attackBtnBounds = { 580, 540, 125, 72 };
        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON, 2, "", attackBtnBounds,
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