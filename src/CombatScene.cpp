#include "CombatScene.h"
#include "CharacterFactory.h"
#include "Character.h"
#include "Scene.h"
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
{
}

CombatScene::~CombatScene()
{
    DestroyEnemyParty();
}

void CombatScene::Load()
{
    LOG("CombatScene: cargando...");
    CreateEnemyParty();

    LoadTextures();

    // ---------Testing------------
    for (Character* c : enemyParty->GetMembers())
    {
        c->PrintDebugInfo();
    }
    // -----------------------------

    combat = new Combat(alliedParty, enemyParty);
}

void CombatScene::Update(float dt)
{
    //if (combatFinished) return;

    // Por ahora Combat::Run() gestiona su propio bucle completo.
    combat->Run(background);
    //combatFinished = true;
    LOG("Update combat scene");

    // Al acabar el combate volvemos a InGameScene (que quedó suspendida)
    if(!combat->GetRunningCombat())
    {
        Engine::GetInstance().scene->PopScene();
    }
}

void CombatScene::PostUpdate(float dt) 
{

}

void CombatScene::Unload()
{
    LOG("CombatScene: descargando...");
    //textures
    Engine::GetInstance().textures->UnLoad(background);

    delete combat;
    combat = nullptr;

    DestroyEnemyParty();
}

void CombatScene::LoadTextures()
{
    background = Engine::GetInstance().textures->Load("Assets/Textures/Backgrounds/BattleBackground.png");
}

bool CombatScene::OnUIMouseClickEvent(UIElement* uiElement)
{
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
