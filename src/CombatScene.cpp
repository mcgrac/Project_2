#include "CombatScene.h"
#include "CharacterFactory.h"
#include "Character.h"
#include "Scene.h"
#include "UIManager.h"
#include "Engine.h"
#include "Log.h"
#include "Textures.h"
#include "Render.h"

CombatScene::CombatScene(Party* _allied, int _shipLevel)
    : alliedParty(_allied)
    , enemyParty(nullptr)
    , combat(nullptr)
    , combatFinished(false)
    , background(nullptr)
    , uiState(CombatUIState::HIDDEN)
    , selectedSkillIdx(-1)
    , laneInputConsumed (false)
    , shipLevel (_shipLevel)
{
    sceneName = "CombatScene";
}

CombatScene::~CombatScene()
{
    DestroyEnemyParty();
}

void CombatScene::Load()
{
    LOG("CombatScene: cargando...");

    LoadTextures();

    // ---------Testing------------
    for (Character* c : alliedParty->GetMembers())
    {
        c->PrintDebugInfo();
    }
    // -----------------------------

    CreateEnemyParty();

    if (enemyParty == nullptr)
    {
        LOG("CombatScene: ERROR — enemyParty es nullptr tras CreateEnemyParty");
        return;
    }

    LOG("CombatScene: enemyParty tiene %d miembros:", enemyParty->GetMemberCount());
    for (Character* c : enemyParty->GetMembers())
    {
        LOG("  enemy -> %s", c->GetName().c_str());
    }

    // Combat is created after lane assignments are confirmed — see FinalizeLaneAssignments()
    // Start the lane selection phase immediately
    laneAssignmentCursor = 0;
    laneAssignments.clear();
    ShowLaneSelectionFor(laneAssignmentCursor);
}

void CombatScene::Update(float dt)
{
    // Dibujar background cada frame
    Engine::GetInstance().render->DrawTexture(background, 0, 0);

    // Reset the click guard every frame so the next real click is accepted
    laneInputConsumed = false;

    // Lane selection is still ongoing — don't run combat yet
    if (uiState == CombatUIState::SELECTING_LANE)
    {
        return;
    }

    // Testing: F1 = victory, F2 = defeat
    if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
    {
        LOG("CombatScene [TEST]: forzando VICTORIA.");
        combat->ForceVictory();
    }
    if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
    {
        LOG("CombatScene [TEST]: forzando DERROTA.");
        combat->ForceDefeat();
    }

    for (Character* c : combat->GetAllCombatants()) {
        //call update of every character (animations)
        c->Update(dt);
        //if (c->GetIsAlive()) {
        //    c->Update(dt);
        //}
    }

    // Gestionar UI si es turno del jugador
    UpdateCombatUI();
    ShowCurrentHP();

    if (!combat->GetWaitingForInput())
    {
        LOG("Combat RUN");
        combat->Run();
    }

    // Al acabar el combate volvemos a InGameScene (que quedó suspendida)
    if(combat->CombatIsFinished())
    {
        HideCombatUI();
        Engine::GetInstance().scene->PopScene();
    }
}

void CombatScene::PostUpdate(float dt) 
{

}

void CombatScene::Unload()
{
    LOG("CombatScene: descargando...");
    HideCombatUI();

    //textures
    Engine::GetInstance().textures->UnLoad(background);
    Engine::GetInstance().textures->UnLoad(abilityIcons);

    delete combat;
    combat = nullptr;

    DestroyEnemyParty();
}

void CombatScene::LoadTextures()
{
    abilityIcons = Engine::GetInstance().textures->Load("Assets/Textures/CombatScene/AbilityIcons.png");
    background = Engine::GetInstance().textures->Load("Assets/Textures/Backgrounds/BattleBackground.png");
}

bool CombatScene::OnUIMouseClickEvent(UIElement* uiElement)
{
    switch (uiElement->id)
    {
        // ---------- SKILLS ----------
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    {
        selectedSkillIdx = uiElement->id - 1;
        ShowTargetPanel();
        break;
    }

    // ---------- TARGETS ----------
    case 10:
    case 11:
    case 12:
    {
        int targetIndex = uiElement->id - 10;

        combat->SubmitPlayerChoice(selectedSkillIdx, targetIndex);

        HideCombatUI();

        selectedSkillIdx = -1;

        break;
    }

    // ---------- BACK ----------
    case 20:
    {
        ShowSkillButtons();
        break;
    }

    // ---- LANE SELECTION ----------
    case 30: // Back lane
    case 31: // Side lane
    case 32: // Front lane
    {
        if (laneInputConsumed)
        {
            break;
        }
        laneInputConsumed = true;

        LaneType chosen;
        if (uiElement->id == 30)
        {
            chosen = LaneType::BACK;
        }
        else if (uiElement->id == 31)
        {
            chosen = LaneType::SIDE;
        }
        else
        {
            chosen = LaneType::FRONT;
        }

        Character* c = alliedParty->GetMembers()[laneAssignmentCursor];
        laneAssignments[c] = chosen;

        LOG("CombatScene: %s -> lane %d", c->GetName().c_str(), (int)chosen);

        laneAssignmentCursor++;

        if (laneAssignmentCursor < (int)alliedParty->GetMemberCount())
        {
            ShowLaneSelectionFor(laneAssignmentCursor);
        }
        else
        {
            FinalizeLaneAssignments();
        }
        break;
    }
    default:
        break;
    }

    return true;
}

#pragma region LANE SELECTION
// ------------ Lane selection ---------------------------
void CombatScene::ShowLaneSelectionFor(int characterIndex)
{
    HideCombatUI();
    uiState = CombatUIState::SELECTING_LANE;

    Character* c = alliedParty->GetMembers()[characterIndex];

    LOG("CombatScene: selecting lane for %s", c->GetName().c_str());

    // Three lane buttons stacked vertically in the center of the screen
    // Label shows the lane name and its bonus so the player can make an informed choice
    struct LaneOption
    {
        int id;
        LaneType type;
        const char* label;
    };

    LaneOption options[3] = {
        { 30, LaneType::BACK,  "Back  (+15 Power)" },
        { 31, LaneType::SIDE,  "Side  (+10 Power +10 Speed)" },
        { 32, LaneType::FRONT, "Front (+10 Speed +10 HP)"   }
    };

    for (int i = 0; i < 3; i++)
    {
        // Skip lanes already assigned to a previous character
        if (IsLaneTaken(options[i].type))
        {
            continue;
        }

        SDL_Rect bounds;
        bounds.x = 540;
        bounds.y = 220 + i * 80;
        bounds.w = 200;
        bounds.h = 60;

        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON,
            options[i].id,
            options[i].label,
            bounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); },
            {}, abilityIcons, 0, bounds.w, bounds.h
        );
    }
}

void CombatScene::FinalizeLaneAssignments()
{
    HideCombatUI();

    // Now that every character has a lane, create the Combat object
    combat = new Combat(alliedParty, enemyParty, shipLevel);

    for (auto& pair : laneAssignments)
    {
        combat->AssignLane(pair.first, pair.second);
    }

    LOG("CombatScene: all lanes assigned, starting combat.");
}

bool CombatScene::IsLaneTaken(LaneType laneType) const
{
    for (const auto& pair : laneAssignments)
    {
        if (pair.second == laneType)
        {
            return true;
        }
    }
    return false;
}
#pragma endregion

//  CreateEnemyParty
void CombatScene::CreateEnemyParty()
{
    enemyParty = new Party("Enemigos");

    //names of the enemies
    const char* enemyNames[] = { "Raptor", "Rex", "Chaman" };

    for (int i = 0; i < 3; ++i)
    {
        Character* c = CharacterFactory::Create(enemyNames[i]);
        if (c != nullptr)
        {
            enemyParty->AddMember(c);
        }
        else
        {
            LOG("CombatScene: no se pudo crear el enemigo '%s'.", enemyNames[i]);
        }
    }

    // Recompensas del combate
    enemyParty->SetXPReward(50);
    enemyParty->SetGoldReward(20);

    LOG("CombatScene: party enemiga creada con %d miembros.", enemyParty->GetMemberCount());
}

void CombatScene::DestroyEnemyParty()
{
    if (enemyParty == nullptr) return;

    for (Character* c : enemyParty->GetMembers())
    {
        delete c;
    }

    delete enemyParty;
    enemyParty = nullptr;
}

void CombatScene::UpdateCombatUI()
{
    if (combat->GetWaitingForInput())
    {
        if (uiState == CombatUIState::HIDDEN)
        {
            ShowSkillButtons();
        }
    }
    else
    {
        if (uiState != CombatUIState::HIDDEN)
        {
            HideCombatUI();
        }
    }
}

void CombatScene::ShowSkillButtons()
{
    HideCombatUI();
    uiState = CombatUIState::SELECTING_SKILL;

    Character* actor = combat->GetCurrentActor();
    if (!actor) return;

    auto& skills = actor->GetSkills();

    for (int i = 0; i < (int)skills.size(); ++i)
    {
        SDL_Rect bounds;
        bounds.x = 20;
        bounds.y = 200 + i * 70;
        bounds.w = 64;
        bounds.h = 64;

        std::string label = skills[i].GetName();

        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON,
            i + 1, // IDs 1..5
            label.c_str(),
            bounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, abilityIcons, 0 + i, bounds.w, bounds.h
        );
    }
}

void CombatScene::ShowTargetPanel()
{
    HideCombatUI();
    uiState = CombatUIState::SELECTING_TARGET;

    auto enemies = combat->GetAliveEnemies();

    for (int i = 0; i < enemies.size(); i++)
    {
        SDL_Rect bounds;
        bounds.x = 260;
        bounds.y = 200 + i * 70;
        bounds.w = 64;
        bounds.h = 64;

        std::string label = enemies[i]->GetName();

        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON,
            10 + i, // IDs 10..12
            label.c_str(),
            bounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, abilityIcons, 0 + i, bounds.w, bounds.h
        );
    }

    SDL_Rect backBounds = { 260, 450, 64, 64 };

    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON,
        20,
        "< Back",
        backBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {},  abilityIcons, 0, backBounds.w, backBounds.h
    );
}

void CombatScene::HideCombatUI()
{
    for (auto e : Engine::GetInstance().uiManager->UIElementsList)
    {
        e->CleanUp();
    }

    uiState = CombatUIState::HIDDEN;
}

void CombatScene::ShowCurrentHP()
{
    //allies
    for (int i = 0; i < alliedParty->GetMemberCount(); i++) {

        Character* c = alliedParty->GetMembers()[i];

        std::string name = c->GetName();
        int currentHp = c->GetCurrentHP();
        int maxHp = c->GetMaxHP();
        std::string text = "Character " + name + "|" + " Current hp " + std::to_string(currentHp) + "/" + std::to_string(maxHp);
        Engine::GetInstance().render->DrawText(text.c_str(), 20, 600 + (50*i), 200, 40, { 255, 255, 255, 255 });
    }

    //enemies
    for (int i = 0; i < enemyParty->GetMemberCount(); i++) {

        Character* c = enemyParty->GetMembers()[i];

        std::string name = c->GetName();
        int currentHp = c->GetCurrentHP();
        int maxHp = c->GetMaxHP();
        std::string text = "Character " + name + "|" + " Current hp " + std::to_string(currentHp) + "/" + std::to_string(maxHp);
        Engine::GetInstance().render->DrawText(text.c_str(), 500, 600 + (50 * i), 200, 40, { 255, 255, 255, 255 });
    }
}

void CombatScene::OnResume()
{
    CreateUI();
}

void CombatScene::OnPause()
{
    Engine::GetInstance().uiManager->CleanUp();
}

void CombatScene::CreateUI()
{
    if (uiState == CombatUIState::SELECTING_TARGET) {
        HideCombatUI();

        auto enemies = combat->GetAliveEnemies();

        for (int i = 0; i < enemies.size(); i++)
        {
            SDL_Rect bounds;
            bounds.x = 260;
            bounds.y = 200 + i * 70;
            bounds.w = 64;
            bounds.h = 64;

            std::string label = enemies[i]->GetName();

            Engine::GetInstance().uiManager->CreateUIElement(
                UIElementType::BUTTON,
                10 + i, // IDs 10..12
                label.c_str(),
                bounds,
                [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, abilityIcons, 0 + i, bounds.w, bounds.h
            );
        }

        SDL_Rect backBounds = { 260, 450, 64, 64 };

        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON,
            20,
            "< Back",
            backBounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, abilityIcons, 0, backBounds.w, backBounds.h
        );
    }
    else if (uiState == CombatUIState::SELECTING_SKILL) {
        HideCombatUI();

        Character* actor = combat->GetCurrentActor();
        if (!actor) return;

        auto& skills = actor->GetSkills();

        for (int i = 0; i < (int)skills.size(); ++i)
        {
            SDL_Rect bounds;
            bounds.x = 20;
            bounds.y = 200 + i * 70;
            bounds.w = 64;
            bounds.h = 64;

            std::string label = skills[i].GetName();

            Engine::GetInstance().uiManager->CreateUIElement(
                UIElementType::BUTTON,
                i + 1, // IDs 1..5
                label.c_str(),
                bounds,
                [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, abilityIcons, 0 + i, bounds.w, bounds.h
            );
        }
    }
    else if (uiState == CombatUIState::SELECTING_LANE)
    {
        ShowLaneSelectionFor(laneAssignmentCursor);
    }
}