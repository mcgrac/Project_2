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
#include "Combat.h"
#include "Ship.h"


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

    if (shipImproved)
    {
        Engine::GetInstance().render->DrawTexture(chartImproved, 50, 100);
    }
    else
    {
        Engine::GetInstance().render->DrawTexture(chartNormal, 50, 100);
    }
}

void DockyardScene::PostUpdate(float dt)
{
    Engine::GetInstance().render->DrawText(
        "ASTILLERO", 520, 50, 240, 40, { 255, 255, 255, 255 }
    );

    DrawChartStats();
}

void DockyardScene::Unload()
{
    Engine::GetInstance().textures->UnLoad(exitButton);
    Engine::GetInstance().textures->UnLoad(background);
    Engine::GetInstance().textures->UnLoad(chartNormal);
    Engine::GetInstance().textures->UnLoad(chartImproved);
    Engine::GetInstance().textures->UnLoad(improveShip);

    Engine::GetInstance().uiManager->CleanUp();
}

void DockyardScene::LoadTextures()
{
    exitButton = Engine::GetInstance().textures->Load("Assets/Textures/HumanIsland/BackButton.png");
    background = Engine::GetInstance().textures->Load("Assets/Textures/HumanIsland/DocksMennu.png");
    chartNormal = Engine::GetInstance().textures->Load("Assets/Textures/DockyardScene/Chart1.png");
    chartImproved = Engine::GetInstance().textures->Load("Assets/Textures/DockyardScene/Chart2.png");
    improveShip = Engine::GetInstance().textures->Load("Assets/Textures/DockyardScene/UpgradeBaotButton.png");
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
            LOG("Dockyard: NPC es nullptr");
            break;
        }
        else {
            LOG("Dockyard: NPC correcto");
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
        if (alliedParty->GetGold() >= COST_IMPROVE_SHIP) {
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

    SDL_Rect improveBounds = { IMPROVE_SHIP_X, IMPROVE_SHIP_Y, IMPROVE_SHIP_W, IMPROVE_SHIP_H };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, IMPROVE_SHIP, "", improveBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, improveShip, 0, improveBounds.w, improveBounds.h
    );

    SDL_Rect talkBounds = { NPC_X, NPC_Y, NPC_W, NPC_H };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, START_DIALOGUE, "", talkBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, dockyard->GetOwner()->GetTexture(), 0, talkBounds.w, talkBounds.h
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

    Engine::GetInstance().render->DrawText(backPowerStr.c_str(), CHART_X + CHART_STAT_OFFSET_X_OLD, CHART_Y + CHART_BACK_POWER_OFFSET_Y, CHART_STAT_W, CHART_STAT_H, { 255, 255, 255, 255 });
    Engine::GetInstance().render->DrawText(sidePowerStr.c_str(), CHART_X + CHART_STAT_OFFSET_X_OLD, CHART_Y + CHART_SIDE_POWER_OFFSET_Y, CHART_STAT_W, CHART_STAT_H, { 255, 255, 255, 255 });
    Engine::GetInstance().render->DrawText(sideSpeedStr.c_str(), CHART_X + CHART_STAT_OFFSET_X_OLD, CHART_Y + CHART_SIDE_SPEED_OFFSET_Y, CHART_STAT_W, CHART_STAT_H, { 255, 255, 255, 255 });
    Engine::GetInstance().render->DrawText(frontSpeedStr.c_str(), CHART_X + CHART_STAT_OFFSET_X_OLD, CHART_Y + CHART_FRONT_SPEED_OFFSET_Y, CHART_STAT_W, CHART_STAT_H, { 255, 255, 255, 255 });
    Engine::GetInstance().render->DrawText(frontHpStr.c_str(), CHART_X + CHART_STAT_OFFSET_X_OLD, CHART_Y + CHART_FRONT_HP_OFFSET_Y, CHART_STAT_W, CHART_STAT_H, { 255, 255, 255, 255 });
    Engine::GetInstance().render->DrawText(leftLevelStr.c_str(), CHART_X + CHART_LEVEL_X, CHART_Y + CHART_LEVEL_Y, CHART_LEVEL_W, CHART_LEVEL_H, { 255, 255, 255, 255 });

    // Columna derecha: stats nuevos tras la mejora (solo si se ha mejorado)
    if (shipImproved)
    {
        std::string backPowerNewStr = "+" + std::to_string(Lane::BASE_BACK_POWER * shipLevel);
        std::string sidePowerNewStr = "+" + std::to_string(Lane::BASE_SIDE_POWER * shipLevel);
        std::string sideSpeedNewStr = "+" + std::to_string(Lane::BASE_SIDE_SPEED * shipLevel);
        std::string frontSpeedNewStr = "+" + std::to_string(Lane::BASE_FRONT_SPEED * shipLevel);
        std::string frontHpNewStr = "+" + std::to_string(Lane::BASE_FRONT_HEALTH * shipLevel);
        std::string rightLevelStr = std::to_string(shipLevel);

        Engine::GetInstance().render->DrawText(backPowerNewStr.c_str(), CHART_X + CHART_STAT_OFFSET_X_NEW, CHART_Y + CHART_BACK_POWER_OFFSET_Y, CHART_STAT_W, CHART_STAT_H, { 100, 255, 100, 255 });
        Engine::GetInstance().render->DrawText(sidePowerNewStr.c_str(), CHART_X + CHART_STAT_OFFSET_X_NEW, CHART_Y + CHART_SIDE_POWER_OFFSET_Y, CHART_STAT_W, CHART_STAT_H, { 100, 255, 100, 255 });
        Engine::GetInstance().render->DrawText(sideSpeedNewStr.c_str(), CHART_X + CHART_STAT_OFFSET_X_NEW, CHART_Y + CHART_SIDE_SPEED_OFFSET_Y, CHART_STAT_W, CHART_STAT_H, { 100, 255, 100, 255 });
        Engine::GetInstance().render->DrawText(frontSpeedNewStr.c_str(), CHART_X + CHART_STAT_OFFSET_X_NEW, CHART_Y + CHART_FRONT_SPEED_OFFSET_Y, CHART_STAT_W, CHART_STAT_H, { 100, 255, 100, 255 });
        Engine::GetInstance().render->DrawText(frontHpNewStr.c_str(), CHART_X + CHART_STAT_OFFSET_X_NEW, CHART_Y + CHART_FRONT_HP_OFFSET_Y, CHART_STAT_W, CHART_STAT_H, { 100, 255, 100, 255 });
        Engine::GetInstance().render->DrawText(rightLevelStr.c_str(), CHART_X + CHART_LEVEL_NEW_X, CHART_Y + CHART_LEVEL_Y, CHART_LEVEL_W, CHART_LEVEL_H, { 100, 255, 100, 255 });
    }
}
