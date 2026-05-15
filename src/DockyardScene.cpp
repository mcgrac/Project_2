#include "DockyardScene.h"
#include "Engine.h"
#include "Audio.h"
#include "Scene.h"
#include "UIManager.h"
#include "Render.h"
#include "Textures.h"
#include "Log.h"
#include "DialogueScene.h"
#include "DialogueManager.h"
#include "Log.h"
#include "NPC.h"
#include "Combat.h"
#include "Ship.h"
#include "SceneUtils.h"


#pragma endregion Chart

const SDL_Rect DockyardScene::CHART_BOUNDS = { 191, 71, 669, 428 };

#pragma endregion
#pragma endregion LEVEL

const SDL_Rect DockyardScene::LEVEL1_BOUNDS = { 624, 92, 16, 28 };
const SDL_Rect DockyardScene::LEVEL2_BOUNDS = { 780, 92, 16, 28 };

#pragma endregion
#pragma endregion STATS

const SDL_Rect DockyardScene::BACK_POWER_BOUNDS = { 498, 196, 23, 19 };

const SDL_Rect DockyardScene::SIDE_POWER_BOUND = { 498, 272, 23, 19 };
const SDL_Rect DockyardScene::SIDE_SPEED_BOUND = { 498, 309, 23, 19 };

const SDL_Rect DockyardScene::FRONT_HEALTH_BOUND = { 498, 385, 23, 19 };
const SDL_Rect DockyardScene::FRONT_SPEED_BOUND = { 498, 423, 23, 19 };

#pragma endregion
#pragma endregion STATS2

const SDL_Rect DockyardScene::BACK_POWER_BOUNDS2 = { 704, 196, 23, 19 };

const SDL_Rect DockyardScene::SIDE_POWER_BOUND2 = { 704, 272, 23, 19 };
const SDL_Rect DockyardScene::SIDE_SPEED_BOUND2 = { 704, 309, 23, 19 };

const SDL_Rect DockyardScene::FRONT_HEALTH_BOUND2 = { 704, 385, 23, 19 };
const SDL_Rect DockyardScene::FRONT_SPEED_BOUND2 = { 704, 423, 23, 19 };

#pragma endregion
#pragma endregion NPC

const SDL_Rect DockyardScene::HUMAN_NPC_BOUNDS = { 534, 107, 549, 660 };
const SDL_Rect DockyardScene::BIRD_NPC_BOUNDS = { 216, 444, 100, 124 };

#pragma endregion
#pragma endregion BackButton

const SDL_Rect DockyardScene::BACK_BOUNDS = { 45, 45, 72, 72 };
const SDL_Rect DockyardScene::BACK_BUTTON_BOUNDS = { 134, 58, 72, 72 };

#pragma endregion
#pragma endregion UpgradeButton

const SDL_Rect DockyardScene::UPGRADE_BUTTON_BOUNDS = { 860, 239, 183, 63 };

#pragma endregion
#pragma endregion Gold Counter

const SDL_Rect DockyardScene::GOLD_COUNTER_BOUNDS = { 534, 107, 549, 660 };
const SDL_Rect DockyardScene::GOLD_COUNTER2_BOUNDS = { 860, 164, 119, 64 };

#pragma endregion
#pragma endregion

DockyardScene::DockyardScene(Dockyard* dockyard, Party* allied)
    : dockyard(dockyard), alliedParty(allied), background(nullptr), exitButton(nullptr), ownerSprite(nullptr), showChart(false)
{
    sceneName = "DockyardScene";
}

DockyardScene::~DockyardScene() {}

void DockyardScene::Load()
{
    LOG("DockyardScene: cargando astillero.");
    LoadTextures();
    CreateUI();

    IslandFaction faction = dockyard->GetIsland()->GetIslandFaction();
    if (faction == IslandFaction::SIRENS || faction == IslandFaction::REPTILES)
    {
        pendingDialogue = true;
    }
}

void DockyardScene::Update(float dt)
{
    if (pendingDialogue)
    {
        pendingDialogue = false;
        PushDialogue();
    }

    if (pendingPop)
    {
        pendingPop = false;
        Engine::GetInstance().scene->PopScene();
        return;
    }

    if(showChart == true){ Engine::GetInstance().render->DrawTexture(fullBack, 0, 0); Engine::GetInstance().render->DrawTexture(moneyCounter, GOLD_COUNTER2_BOUNDS.x, GOLD_COUNTER2_BOUNDS.y);
    }
    else { Engine::GetInstance().render->DrawTexture(background, 0, 0); Engine::GetInstance().render->DrawTexture(moneyCounter, GOLD_COUNTER_BOUNDS.x, GOLD_COUNTER_BOUNDS.y);
    }

    if(showChart)
    {
        if (shipImproved)
        {
            Engine::GetInstance().render->DrawTexture(chartImproved, CHART_BOUNDS.x, CHART_BOUNDS.y);
        }
        else
        {
            Engine::GetInstance().render->DrawTexture(chartNormal, CHART_BOUNDS.x, CHART_BOUNDS.y);
        }
    }
}

void DockyardScene::PostUpdate(float dt)
{
   /* Engine::GetInstance().render->DrawText(
        "ASTILLERO", 520, 50, 240, 40, { 255, 255, 255, 255 }
    );*/

    if(showChart)
    {
        DrawChartStats();
    }
}

void DockyardScene::Unload()
{
    Engine::GetInstance().textures->UnLoad(exitButton);
    Engine::GetInstance().textures->UnLoad(background);
    Engine::GetInstance().textures->UnLoad(chartNormal);
    Engine::GetInstance().textures->UnLoad(chartImproved);
    Engine::GetInstance().textures->UnLoad(improveShip);
    Engine::GetInstance().textures->UnLoad(ownerSprite);

    Engine::GetInstance().uiManager->CleanUp();
}

void DockyardScene::LoadTextures()
{
    exitButton = Engine::GetInstance().textures->Load("Assets/Textures/HumanIsland/BackButton.png");
    //background = Engine::GetInstance().textures->Load("Assets/Textures/HumanIsland/DocksMennu.png");
    chartNormal = Engine::GetInstance().textures->Load("Assets/Textures/DockyardScene/Chart1.png");
    chartImproved = Engine::GetInstance().textures->Load("Assets/Textures/DockyardScene/Chart2.png");
    improveShip = Engine::GetInstance().textures->Load("Assets/Textures/DockyardScene/UpgradeBaotButton.png");

    IslandFaction faction = dockyard->GetIsland()->GetIslandFaction();
    std::string path = "Assets/Textures/DockyardScene/" + SceneUtils::GetFactionString(faction);
    background = Engine::GetInstance().textures->Load((path + "/background.png").c_str());

    fullBack = Engine::GetInstance().textures->Load((path + "/fullBack.png").c_str());
    ownerSprite = Engine::GetInstance().textures->Load((path + "/ownerSprite.png").c_str());
    moneyCounter = Engine::GetInstance().textures->Load("Assets/Textures/ShopScene/moneyCounter.png");
}

void DockyardScene::LoadSound() {
    buttonPress = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/UIfx/button_press.wav");
    shipUpgrade = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Island_menu/upgrading_ship.wav");
}

bool DockyardScene::OnUIMouseClickEvent(UIElement* uiElement)
{
    switch (uiElement->id)
    {
    case BACK_BUTTON_ID:
        Engine::GetInstance().audio->PlayFx(buttonPress);
        showChart = false;
        Engine::GetInstance().scene->PopScene();
        break;
    case START_DIALOGUE:
    {
        PushDialogue();
        break;
    }
    case IMPROVE_SHIP:
        if (alliedParty->GetGold() >= COST_IMPROVE_SHIP) {
            Engine::GetInstance().audio->PlayFx(shipUpgrade);
            alliedParty->AddGold(-COST_IMPROVE_SHIP);
            levelBeforeImprove = dockyard->GetShip()->GetLevel();
            dockyard->ImproveShip();
            shipImproved = true;
        }
        else {
#if _DEBUG
            LOG("Dockyard: Not enough gold");
#endif // _DEBUG

        }

        break;
    default:
        break;
    }
    return true;
}

void DockyardScene::OnResume()
{
    if (pendingPop) { return; }
    if (showChart)
    {
        CreateChartButtons();
    }
    else
    {
        CreateUI();
    }
}

void DockyardScene::OnPause()
{
    Engine::GetInstance().uiManager->CleanUp();
}

void DockyardScene::CreateUI()
{
    SDL_Rect backBounds = BACK_BOUNDS;
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, BACK_BUTTON_ID, "", backBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, exitButton, 0, backBounds.w, backBounds.h
    );
    SDL_Rect talkBounds = HUMAN_NPC_BOUNDS;
    if (SceneUtils::GetFactionString(dockyard->GetIsland()->GetIslandFaction()) == "Bird") {
        talkBounds = BIRD_NPC_BOUNDS;
    }
   
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, START_DIALOGUE, "", talkBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, ownerSprite, 0, talkBounds.w, talkBounds.h
    );
}

void DockyardScene::PushDialogue()
{
    NPC* npc = dockyard->GetOwner();
    if (npc == nullptr)
    {
        LOG("Dockyard: NPC es nullptr");
        return;
    }
    IslandFaction faction = dockyard->GetIsland()->GetIslandFaction();
    bool popOnLeave = (faction == IslandFaction::SIRENS || faction == IslandFaction::REPTILES);

    Engine::GetInstance().scene->PushScene(
        new DialogueScene(npc->GetDialogueId(),
            [this, popOnLeave]()
            {
                std::string action = DialogueManager::GetLastChoiceTag();

                if (action == "upgrade")
                {
                    LOG("DOCKYARD: upgrade");
                    CreateChartButtons();
                    showChart = true;
                }
                else if (popOnLeave)
                {
                    LOG("DOCK SCENE: Pop scene");
                    pendingPop = true;
                }
            }
        )
    );
}

void DockyardScene::CreateChartButtons()
{
    SDL_Rect improveBounds = { UPGRADE_BUTTON_BOUNDS };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, IMPROVE_SHIP, "", improveBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, improveShip, 0, improveBounds.w, improveBounds.h
    );

    SDL_Rect backBounds = BACK_BUTTON_BOUNDS;
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, BACK_BUTTON_ID, "", backBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, exitButton, 0, backBounds.w, backBounds.h
    );
}

void DockyardScene::DrawChartStats()
{
    int shipLevel = dockyard->GetShip()->GetLevel();
    int leftLevel = shipImproved ? levelBeforeImprove : shipLevel;

    // Columna izquierda: stats previos a la mejora (o actuales si no se ha mejorado)
    std::string backPowerStr = "+" + std::to_string(Lane::BASE_BACK_POWER * leftLevel);
    std::string sidePowerStr = "+" + std::to_string(Lane::BASE_SIDE_POWER * leftLevel);
    std::string sideSpeedStr = "+" + std::to_string(Lane::BASE_SIDE_SPEED * leftLevel);
    std::string frontSpeedStr = "+" + std::to_string(Lane::BASE_FRONT_SPEED * leftLevel);
    std::string frontHpStr = "+" + std::to_string(Lane::BASE_FRONT_HEALTH * leftLevel);
    std::string leftLevelStr = std::to_string(leftLevel);

    Engine::GetInstance().render->DrawText(backPowerStr.c_str(), BACK_POWER_BOUNDS.x, BACK_POWER_BOUNDS.y, BACK_POWER_BOUNDS.w, BACK_POWER_BOUNDS.h, { 255, 255, 255, 255 });
    Engine::GetInstance().render->DrawText(sidePowerStr.c_str(), SIDE_POWER_BOUND.x, SIDE_POWER_BOUND.y, SIDE_POWER_BOUND.w, SIDE_POWER_BOUND.h, { 255, 255, 255, 255 });
    Engine::GetInstance().render->DrawText(sideSpeedStr.c_str(), SIDE_SPEED_BOUND.x, SIDE_SPEED_BOUND.y, SIDE_SPEED_BOUND.w, SIDE_SPEED_BOUND.h, { 255, 255, 255, 255 });
    Engine::GetInstance().render->DrawText(frontSpeedStr.c_str(), FRONT_SPEED_BOUND.x, FRONT_SPEED_BOUND.y, FRONT_SPEED_BOUND.w, FRONT_SPEED_BOUND.h, { 255, 255, 255, 255 });
    Engine::GetInstance().render->DrawText(frontHpStr.c_str(), FRONT_HEALTH_BOUND.x, FRONT_HEALTH_BOUND.y, FRONT_HEALTH_BOUND.w, FRONT_HEALTH_BOUND.h, { 255, 255, 255, 255 });
    Engine::GetInstance().render->DrawText(leftLevelStr.c_str(), LEVEL1_BOUNDS.x, LEVEL1_BOUNDS.y, LEVEL1_BOUNDS.w, LEVEL1_BOUNDS.h, { 255, 255, 255, 255 });

    // Columna derecha: stats nuevos tras la mejora (solo si se ha mejorado)
    if (shipImproved)
    {
        std::string backPowerNewStr = "+" + std::to_string(Lane::BASE_BACK_POWER * shipLevel);
        std::string sidePowerNewStr = "+" + std::to_string(Lane::BASE_SIDE_POWER * shipLevel);
        std::string sideSpeedNewStr = "+" + std::to_string(Lane::BASE_SIDE_SPEED * shipLevel);
        std::string frontSpeedNewStr = "+" + std::to_string(Lane::BASE_FRONT_SPEED * shipLevel);
        std::string frontHpNewStr = "+" + std::to_string(Lane::BASE_FRONT_HEALTH * shipLevel);
        std::string rightLevelStr = std::to_string(shipLevel);

        Engine::GetInstance().render->DrawText(backPowerNewStr.c_str(), BACK_POWER_BOUNDS2.x, BACK_POWER_BOUNDS2.y, BACK_POWER_BOUNDS2.w, BACK_POWER_BOUNDS2.h, { 100, 255, 100, 255 });
        Engine::GetInstance().render->DrawText(sidePowerNewStr.c_str(), SIDE_POWER_BOUND2.x, SIDE_POWER_BOUND2.y, SIDE_POWER_BOUND2.w, SIDE_POWER_BOUND2.h, { 100, 255, 100, 255 });
        Engine::GetInstance().render->DrawText(sideSpeedNewStr.c_str(), SIDE_SPEED_BOUND2.x, SIDE_SPEED_BOUND2.y, SIDE_SPEED_BOUND2.w, SIDE_SPEED_BOUND2.h, { 100, 255, 100, 255 });
        Engine::GetInstance().render->DrawText(frontSpeedNewStr.c_str(), FRONT_SPEED_BOUND2.x, FRONT_SPEED_BOUND2.y, FRONT_SPEED_BOUND2.w, FRONT_SPEED_BOUND2.h, { 100, 255, 100, 255 });
        Engine::GetInstance().render->DrawText(frontHpNewStr.c_str(), FRONT_HEALTH_BOUND2.x, FRONT_HEALTH_BOUND2.y, FRONT_HEALTH_BOUND2.w, FRONT_HEALTH_BOUND2.h, { 100, 255, 100, 255 });
        Engine::GetInstance().render->DrawText(rightLevelStr.c_str(), LEVEL2_BOUNDS.x, LEVEL2_BOUNDS.y, LEVEL2_BOUNDS.w, LEVEL2_BOUNDS.h, { 100, 255, 100, 255 });
    }
}
