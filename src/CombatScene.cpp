#include "CombatScene.h"
#include "CharacterFactory.h"
#include "Character.h"
#include "Scene.h"
#include "UIManager.h"
#include "Engine.h"
#include "Log.h"
#include "Textures.h"
#include "Render.h"

CombatScene::CombatScene(Party* _allied)
    : alliedParty(_allied)
    , enemyParty(nullptr)
    , combat(nullptr)
    , combatFinished(false)
    , background(nullptr)
    , uiState(CombatUIState::HIDDEN)
    , selectedSkillIdx(-1)
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


    combat = new Combat(alliedParty, enemyParty);
}

void CombatScene::Update(float dt)
{
    // Dibujar background cada frame
    Engine::GetInstance().render->DrawTexture(background, 0, 0);

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
        if (c->GetIsAlive()) {
            c->Update(dt);
        }
    }

    // Gestionar UI si es turno del jugador
    UpdateCombatUI();

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
    }

    return true;
}

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
