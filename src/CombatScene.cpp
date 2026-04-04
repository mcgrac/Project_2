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

    for (Character* c : combat->GetAllCombatants()) {
        //call update of every character (animations)
        if (c->GetIsAlive()) {
            c->Update(dt);
        }
    }

    combat->Run();

    // Al acabar el combate volvemos a InGameScene (que quedó suspendida)
    if(combat->CombatIsFinished())
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
