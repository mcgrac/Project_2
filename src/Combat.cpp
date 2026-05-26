#include "Combat.h"

//test
#include "Engine.h"
#include "Input.h"
//test

#include "Party.h"
#include "Skill.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <random>
#include <math.h>

#include "Render.h"
#include "Textures.h"

#include "Log.h"

//  Posiciones predefinidas en pantalla
//  0-2: aliados (izquierda)  |  3-5: enemigos (derecha)
const Vector2D Combat::defaultPositions[6] = {
    Vector2D(153.0f, 273.0f),   // aliado 0
    Vector2D(278.0f, 184.0f),   // aliado 1
    Vector2D(419.0f, 273.0f),   // aliado 2
    Vector2D(756.0f, 273.0f),   // enemigo 0
    Vector2D(897.0f, 184.0f),   // enemigo 1
    Vector2D(1022.0f, 273.0f)    // enemigo 2
};

Combat::Combat(Party* allied, Party* enemy, int _shipLevel)
    : alliedParty(allied)
    , enemyParty(enemy)
    , state(CombatState::START_COMBAT)
    , result(CombatResult::NONE)
    , currentActor(nullptr)
    , runningCombat(true)
    , shipLevel (_shipLevel)
{
    //assign lanes types
    backLane.type = LaneType::BACK;
    sideLane.type = LaneType::SIDE;
    frontLane.type = LaneType::FRONT;

    enemyBackLane.type = LaneType::BACK;
    enemySideLane.type = LaneType::SIDE;
    enemyFrontLane.type = LaneType::FRONT;
}

Combat::~Combat()
{
}

#pragma region LANE LOGIC
//-------------------- Lane bonus calculations -----------------------------------
int Lane::GetPowerBonus(int shipLevel) const
{
    if (type == LaneType::BACK)
    {
        return BASE_BACK_POWER * shipLevel;
    }
    if (type == LaneType::SIDE)
    {
        return BASE_SIDE_POWER * shipLevel;
    }
    return 0;

    // FRONT gives no power bonus
}

int Lane::GetSpeedBonus(int shipLevel) const
{
    if (type == LaneType::SIDE)
    {
        return BASE_SIDE_SPEED * shipLevel;
    }

    if (type == LaneType::FRONT)
    {
        return BASE_FRONT_SPEED * shipLevel;
    }
    return 0;

    // BACK gives no speed bonus
}

int Lane::GetHealthBonus(int shipLevel) const
{
    if (type == LaneType::FRONT)
    {
        return BASE_FRONT_HEALTH * shipLevel;
    }
    return 0;

    // BACK and SIDE give no health bonus
}

// ---------- Lane assignment (called by CombatScene) --------------------
void Combat::AssignLane(Character* character, LaneType laneType)
{
    if (laneType == LaneType::BACK)
    {
        backLane.occupant = character;
        LOG("Combat: %s assigned to BACK lane", character->GetName().c_str());
    }
    else if (laneType == LaneType::SIDE)
    {
        sideLane.occupant = character;
        LOG("Combat: %s assigned to SIDE lane", character->GetName().c_str());
    }
    else
    {
        frontLane.occupant = character;
        LOG("Combat: %s assigned to FRONT lane", character->GetName().c_str());
    }
}

// ------ Apply lane bonuses to each occupant ------------------------------
void Combat::ApplyLaneBonuses()
{
    Lane* lanes[3] = { &backLane, &sideLane, &frontLane };

    for (int i = 0; i < 3; i++)
    {
        Lane* lane = lanes[i];

        if (lane->occupant == nullptr)
        {
            continue;
        }

        Character* c = lane->occupant;

        int powerBonus = lane->GetPowerBonus(shipLevel);
        int speedBonus = lane->GetSpeedBonus(shipLevel);
        int healthBonus = lane->GetHealthBonus(shipLevel);

        if (powerBonus != 0)
        {
            c->ModifyBonusPower(powerBonus);
            LOG("Combat: %s lane bonus +%d power", c->GetName().c_str(), powerBonus);
        }

        if (speedBonus != 0)
        {
            c->ModifyBonusSpeed(speedBonus);
            LOG("Combat: %s lane bonus +%d speed", c->GetName().c_str(), speedBonus);
        }

        if (healthBonus != 0)
        {
            c->ModifyMaxHealth(healthBonus);
            LOG("Combat: %s lane bonus +%d max health", c->GetName().c_str(), healthBonus);
        }

        // Recalculate totals after applying bonuses
        c->SetTotalPower();
        c->SetTotalSpeed();
    }
}

#pragma endregion

void Combat::AssignEnemyLanes()
{
    auto& members = enemyParty->GetMembers();
    if (members.empty()) { return; }

    // Encontrar el tanque: mayor durabilidad, en caso de empate mayor vida
    Character* tankCandidate = nullptr;
    Character* damageCandidate = nullptr;
    Character* sideCandidate = nullptr;

    int highestDurability = -1;
    int highestPower = -1;

    for (Character* c : members)
    {
        if (c->GetTotalDurability() > highestDurability ||
            (c->GetTotalDurability() == highestDurability &&
                tankCandidate != nullptr &&
                c->GetMaxHP() > tankCandidate->GetMaxHP()))
        {
            highestDurability = c->GetTotalDurability();
            tankCandidate = c;
        }
    }

    for (Character* c : members)
    {
        if (c == tankCandidate) { continue; }

        if (c->GetTotalPower() > highestPower)
        {
            highestPower = c->GetTotalPower();
            damageCandidate = c;
        }
    }

    // El que queda va al side
    for (Character* c : members)
    {
        if (c != tankCandidate && c != damageCandidate)
        {
            sideCandidate = c;
            break;
        }
    }

    // Asignar lanes
    enemyFrontLane.occupant = tankCandidate;
    enemyBackLane.occupant = damageCandidate;
    enemySideLane.occupant = sideCandidate;

    LOG("Combat: enemy lanes — FRONT(tank): %s | SIDE: %s | BACK(dmg): %s",
        tankCandidate ? tankCandidate->GetName().c_str() : "none",
        sideCandidate ? sideCandidate->GetName().c_str() : "none",
        damageCandidate ? damageCandidate->GetName().c_str() : "none");
}

void Combat::Run(float dt)
{
    if (!runningCombat) return;

#if _DEBUG
    //--------------TEST DEBUGS-----------
    if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_W) == KEY_DOWN) {

        state = CombatState::END_COMBAT;
        result = CombatResult::VICTORY;
        EndCombat();
    }
    else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_L) == KEY_DOWN) {
        state = CombatState::END_COMBAT;
        result = CombatResult::DEFEAT;
        EndCombat();
    }
    //------------------------------------
#endif
    switch (state)
    {
    case CombatState::START_COMBAT:
        StartCombat();
        state = CombatState::CALCULATE_INITIATIVE;
        break;

    case CombatState::CALCULATE_INITIATIVE:
        // Accumulate initiative for everyone.
        // Returns true only if at least one combatant reached >= 100.
        if (!CalculateInitiative())
        {
            //dont do anything -> go to next frame
        }
        else
        {
            //state = CombatState::ATTACK;
            //state = CombatState::ATTACK_START;
            state = CombatState::FILL_QUEUE;
        }
        break;

    case CombatState::FILL_QUEUE:
        FillQueue();
        state = CombatState::PROCESS_QUEUE;
        break;

    case CombatState::PROCESS_QUEUE:
        ProcessQueue();
        // ProcessQueue sets the next state itself (ATTACK_START or CALCULATE_INITIATIVE).
        break;

    case CombatState::NEXT_ROUND_PAUSE:
        // CombatScene gestiona el timer y llama ResumeFromNextRoundPause()
        break;

    case CombatState::ATTACK_START:
        AttackStart();
        break;

    case CombatState::WAITING_FOR_PLAYER_INPUT:
        //do nothing->combatScene manages selection
        break;

    case CombatState::ATTACK_ANIMATION:
        AttackAnimation();
        break;

    case CombatState::ATTACK_RESOLVE:
        AttackResolve();
        feedbackTimer = 0.0f;
        state = CombatState::ATTACK_FEEDBACK;
        break;

    case CombatState::ATTACK_FEEDBACK:
        AttackFeedback(dt);
        break;

    case CombatState::MODIFIERS:
        ApplyModifiers();
        state = CombatState::CHECK_DEFEAT;
        break;

    case CombatState::CHECK_DEFEAT:
        CheckDefeat();
        break;

    case CombatState::END_COMBAT:
        EndCombat();
        runningCombat = false;
        break;
    }
}

bool Combat::CombatIsFinished() const
{
    bool b;
    if (state == CombatState::END_COMBAT && !runningCombat) { b = true; }
    else { b = false; }
    return b;
}

void Combat::ResetBonusStats(Character* c)
{
    //clear bonus stats ans effects
    for (Character* c : alliedParty->GetMembers()) {
        c->ClearBonusStats();
        c->ClearStatusEffects();

        LOG("BEFORE CLEAR |%s| totalPower: %d, totalSpeed: %d, totalDurability:%d", c->GetName().c_str(), c->GetTotalPower(), c->GetTotalSpeed(), c->GetTotalDurability());

        //calculate again total stats
        c->SetTotalPower();
        c->SetTotalDurability();
        c->SetTotalSpeed();

        LOG("AFTER CLEAR |%s| totalPower: %d, totalSpeed: %d, totalDurability:%d", c->GetName().c_str(), c->GetTotalPower(), c->GetTotalSpeed(), c->GetTotalDurability());

    }
}

//  START_COMBAT
void Combat::StartCombat()
{
    std::cout << "\n════════════════════=═════════════════\n";
    std::cout << "          COMBATE INICIADO            \n";
    std::cout << "══════════════════════════════════════\n";

#if _DEBUG
    //----------------debug--------------
    std::cout << "\033[1;32m\n[ALIADOS]\n\033[0m";
    for (Character* c : alliedParty->GetMembers())
    {
        std::cout << "  " << c->GetName()
            << " | HP: " << c->GetCurrentHP()
            << " | Power: " << c->GetTotalPower()
            << " | Speed: " << c->GetTotalPower() << "\n";
    }

    std::cout << "\n[ENEMIGOS]\n";
    for (Character* c : enemyParty->GetMembers())
    {
        std::cout << "  " << c->GetName()
            << " | HP: " << c->GetCurrentHP()
            << " | Power: " << c->GetTotalPower()
            << " | Speed: " << c->GetTotalPower() << "\n";
    }
    std::cout << "\n";
    //---------------------------------------
#endif


    for (Character* c : alliedParty->GetMembers()) {
        //save previous states in combat
        preCombatValues[c] = c->TakePreCombatValues();

        ResetBonusStats(c);
    }

    // Apply lane bonuses after resetting so they are the only bonuses active
    ApplyLaneBonuses();

    //auto allCombatants = GetAllCombatants();

    if (frontLane.occupant != nullptr)
    {
        frontLane.occupant->SetPosition(defaultPositions[2].getX(), defaultPositions[2].getY());
    }
    if (sideLane.occupant != nullptr)
    {
        sideLane.occupant->SetPosition(defaultPositions[1].getX(), defaultPositions[1].getY());
    }
    if (backLane.occupant != nullptr)
    {
        backLane.occupant->SetPosition(defaultPositions[0].getX(), defaultPositions[0].getY());
    }

    // Allies: reset initiative and animations
    for (Character* c : alliedParty->GetMembers())
    {
        c->ResetCurrentInitiative();
        c->PlayAnimation("idle");
    }

    AssignEnemyLanes();
    if (enemyFrontLane.occupant != nullptr)
    {
        enemyFrontLane.occupant->SetPosition(defaultPositions[3].getX(), defaultPositions[3].getY());
        enemyFrontLane.occupant->ResetCurrentInitiative();
    }
    if (enemySideLane.occupant != nullptr)
    {
        enemySideLane.occupant->SetPosition(defaultPositions[4].getX(), defaultPositions[4].getY());
        enemySideLane.occupant->ResetCurrentInitiative();
    }
    if (enemyBackLane.occupant != nullptr)
    {
        enemyBackLane.occupant->SetPosition(defaultPositions[5].getX(), defaultPositions[5].getY());
        enemyBackLane.occupant->ResetCurrentInitiative();
    }

#if _DEBUG
    std::cout << "\n[ALIADOS tras bonificaciones de lane]\n";
    for (Character* c : alliedParty->GetMembers())
    {
        std::cout << "  " << c->GetName()
            << " | HP: " << c->GetCurrentHP()
            << " | Power: " << c->GetTotalPower()
            << " | Speed: " << c->GetTotalSpeed() << "\n";
    }
    std::cout << "\n";
#endif
}

//  CALCULATE_INITIATIVE
bool Combat::CalculateInitiative()
{
    LOG("── CalculateInitiative tick ──");

    bool anyReady = false;

    for (Character* c : GetAllCombatants())
    {
        if (c->GetIsAlive())
        {
            int before = c->GetCurrentInitiative();
            int bonus = 50;
            int totalSpeedTemp = c->GetTotalSpeed();
            LOG("Total Speed: %d\n", totalSpeedTemp);
            if(totalSpeedTemp>1){
                bonus += 6*log2(totalSpeedTemp);
                LOG("Total Speed: %f\n", 6 * log2(totalSpeedTemp));
            }
            LOG("Total Bonus: %d\n", bonus);
            c->AddInitiative(bonus);

            int after = c->GetCurrentInitiative();

#if _DEBUG
            std::cout << "  " << c->GetName()
                << " | antes: " << before
                << " + " << bonus
                << " = " << after;
#endif
            if (after >= 100)
            {
                anyReady = true;
                LOG("  %s | %d + %d = %d  [READY TO ACT]",
                    c->GetName().c_str(), before, bonus, after);
            }
            else {
                LOG("  %s | %d + %d = %d",
                    c->GetName().c_str(), before, bonus, after);
            }

            std::cout << "\n";
        }
    }

    return anyReady;

    currentActor = GetHighestInitiativeActor();
}

void Combat::FillQueue()
{
    actorsQueue.clear();

    for (Character* c : GetAllCombatants())
    {
        if (c->GetIsAlive() && c->GetCurrentInitiative() >= 100)
        {
            actorsQueue.push_back(c);
        }
    }

    // Sort highest initiative first
    std::sort(actorsQueue.begin(), actorsQueue.end(),
        [](Character* a, Character* b)
        {
            return a->GetCurrentInitiative() > b->GetCurrentInitiative();
        });

    LOG("── FillQueue | %d actor(s) ready to act ──", static_cast<int>(actorsQueue.size()));
    for (int i = 0; i < static_cast<int>(actorsQueue.size()); i++)
    {
        Character* c = actorsQueue[i];
        LOG("  [%d] %s | Initiative: %d | HP: %d/%d | Alive: %s",
            i,
            c->GetName().c_str(),
            c->GetCurrentInitiative(),
            c->GetCurrentHP(), c->GetMaxHP(),
            c->GetIsAlive() ? "YES" : "NO");
    }
}

void Combat::ProcessQueue()
{
    // Remove dead from the queue
    actorsQueue.erase(std::remove_if(actorsQueue.begin(), actorsQueue.end(), [](Character* c) { return !c->GetIsAlive() || c->GetPendingToDie(); }),actorsQueue.end());

    if (actorsQueue.empty())
    {
        //LOG("ProcessQueue | Queue empty -> CALCULATE_INITIATIVE");
        //state = CombatState::CALCULATE_INITIATIVE;
        LOG("ProcessQueue | Queue empty -> NEXT_ROUND_PAUSE");
        state = CombatState::NEXT_ROUND_PAUSE;
        return;
    }

    // Pop the next actor
    currentActor = actorsQueue.front();
    actorsQueue.erase(actorsQueue.begin());

    LOG("ProcessQueue | Next actor: %s | Initiative: %d | Remaining in queue: %d",
        currentActor->GetName().c_str(),
        currentActor->GetCurrentInitiative(),
        static_cast<int>(actorsQueue.size()));

    state = CombatState::ATTACK_START;
}

//  ATTACK
void Combat::AttackStart()
{
    if (currentActor == nullptr) return;
    if (!currentActor->GetIsAlive() || currentActor->GetPendingToDie())
    {
        state = CombatState::PROCESS_QUEUE;
        return;
    }

#if _DEBUG
    std::cout << "\n──────────────────────────────────────\n";
    std::cout << "│ TURNO DE: " << currentActor->GetName() << "\n";
    std::cout << "│ HP: " << currentActor->GetCurrentHP()
        << " | Iniciativa: " << currentActor->GetCurrentInitiative()
        << " | Power: " << currentActor->GetTotalPower() << "\n";
    std::cout << "──────────────────────────────────────\n";
#endif
    if (IsAllied(currentActor))
    {
        //Esperar a que CombatScene entregue la eleccion via SubmitPlayerChoice
        state = CombatState::WAITING_FOR_PLAYER_INPUT;
        return;   // salir sin avanzar
    }
    else
    {
        EnemyTurn();

        //play animation
        std::string anim = currentSkill->GetAnimationId();
        currentActor->PlayAnimation(anim);

        state = CombatState::ATTACK_ANIMATION;
    }
}

void Combat::AttackAnimation()
{
    //LOG("waiting animation...");

    if(currentActor->GetAnimationFinished())
    {
        LOG("AnimationFinished!");
        state = CombatState::ATTACK_RESOLVE;
    }
}

void Combat::AttackResolve()
{
    if (currentSkill->GetHasAreaEffect() && currentTarget == nullptr)
    {
        // false target so execute skill works and current target is not null
        auto aliveEnemies = GetAliveMembers(enemyParty);
        if (!aliveEnemies.empty())
        {
            currentTarget = aliveEnemies[0];
        }
    }

    ExecuteSkill(currentActor, *currentSkill, currentTarget);
    //reset animation idle current actor
    currentActor->PlayAnimation("idle");
}

//  MODIFIERS — veneno y quemadura
void Combat::ApplyModifiers()
{
    bool anyModifier = false;

    for (Character* c : GetAllCombatants())
    {
        //if (!c->GetIsAlive()) continue;

        if (!c->GetIsAlive()) continue; //only apply modifiers to the live characters

        if (c->IsPoisoned())
        {
            anyModifier = true;
            int poisonDmg = c->GetPoisonDamage();
            int hpBefore = c->GetCurrentHP();
            c->TakePoisonDamage();
            int hpAfter = c->GetCurrentHP();

#if _DEBUG
            std::cout << "  [VENENO] " << c->GetName()
                << " sufre " << poisonDmg << " de daño por veneno."
                << " HP: " << hpBefore << " -> " << hpAfter;

            if (!c->GetIsAlive())
            {
                std::cout << "  [MUERTO]";
            }

            std::cout << "\n";
#endif
        }

        if (c->IsBurning())
        {

            anyModifier = true;
            int burnDmg = c->GetBurnDamage();
            int hpBefore = c->GetCurrentHP();
            c->TakeBurnDamage();
            int hpAfter = c->GetCurrentHP();

#if _DEBUG
            std::cout << "  [QUEMADURA] " << c->GetName()
                << " sufre " << burnDmg << " de daño por quemadura."
                << " HP: " << hpBefore << " -> " << hpAfter;

            if (!c->GetIsAlive())
            {
                std::cout << "  [MUERTO]";
            }

            std::cout << "\n";
#endif
        }
#if _DEBUG
        if (!anyModifier)
        {
            std::cout << "  [MODIFICADORES] Ningun efecto activo.\n";
        }
#endif
    }
}

//  CHECK_DEFEAT
void Combat::CheckDefeat()
{
    LOG("── CheckDefeat ──");

#if _DEBUG
    // Log current HP for all combatants
    for (Character* c : alliedParty->GetMembers())
    {
        LOG("  [ALLY]  %s | HP: %d/%d | Alive: %s",
            c->GetName().c_str(),
            c->GetCurrentHP(), c->GetMaxHP(),
            c->GetIsAlive() ? "YES" : "NO");
    }
    for (Character* c : enemyParty->GetMembers())
    {
        LOG("  [ENEMY] %s | HP: %d/%d | Alive: %s",
            c->GetName().c_str(),
            c->GetCurrentHP(), c->GetMaxHP(),
            c->GetIsAlive() ? "YES" : "NO");
    }
#endif // _DEBUG

    if (IsPartyDefeated(enemyParty))
    {
        result = CombatResult::VICTORY;
        state = CombatState::END_COMBAT;
        LOG("|Victory|");
    }
    else if (IsPartyDefeated(alliedParty))
    {
        result = CombatResult::DEFEAT;
        state = CombatState::END_COMBAT;
        LOG("|Defeat|");
    }
    else
    {
        // El combate continúa
        state = CombatState::PROCESS_QUEUE;
        LOG("CheckDefeat | Combat continues -> PROCESS_QUEUE");
    }
}

//  END_COMBAT
void Combat::EndCombat()
{
    //clear bonus stats ans effects
    for (Character* c : alliedParty->GetMembers()) {
        ResetBonusStats(c);
    }

    if (result == CombatResult::VICTORY)
    {
        std::cout << "\n══════════════════════════════════════\n";
        std::cout << "              VICTORY                \n";
        std::cout << "══════════════════════════════════════\n";

        //reset base Values for every character
        for (Character* c : alliedParty->GetMembers()) {
            //reset base stats to original
            auto it = preCombatValues.find(c);
            if (it != preCombatValues.end()) {
                c->RestoreBaseStats(it->second);
            }
        }
        
        //// Distribuir XP a los aliados vivos
        //int totalXP = enemyParty->GetTotalXPReward();
        //int totalGold = enemyParty->GetTotalGoldReward();
        //auto lootItems = enemyParty->GetLootItems();

        //for (Character* ally : GetAliveMembers(alliedParty))
        //{
        //    ally->GainExperience(totalXP);
        //    std::cout << ally->GetName() << " gana " << totalXP << " XP.\n";
        //}

        //alliedParty->AddGold(totalGold);
        //std::cout << "Recompensa: " << totalGold << " de oro.\n";
    }
    else // DEFEAT
    {
        std::cout << "\n══════════════════════════════════════\n";
        std::cout << "              GAME OVER               \n";
        std::cout << "══════════════════════════════════════\n";

        //reset allied party values
        for (Character* c : alliedParty->GetMembers()) {
            auto it = preCombatValues.find(c);
            if (it != preCombatValues.end()) {
                c->RestorePreCombatValues(it->second);
            }
        }
    }

    // Revivir aliados muertos con 1 HP para el siguiente combate
    for (Character* c : alliedParty->GetMembers())
    {
        if (!c->GetIsAlive())
        {
            c->ModifyCurrentHealth(1);
            // Forzar isAlive a true
            c->RestorePreCombatValues({ 1, true });
            LOG("Combat: %s revive con 1 HP para el siguiente combate.", c->GetName().c_str());
        }
    }

    //clean lanes
    enemyFrontLane.occupant = nullptr;
    enemySideLane.occupant = nullptr;
    enemyBackLane.occupant = nullptr;

    preCombatValues.clear();
    runningCombat = false;
}

//  PLAYER TURN
void Combat::PlayerTurn()
{
    auto& skills = currentActor->GetSkills(); // vector de hasta 5 Skills

    // Mostrar habilidades disponibles
    std::cout << "Elige habilidad:\n";
    for (int i = 0; i < static_cast<int>(skills.size()); ++i)
    {
        std::cout << "  [" << i << "] " << skills[i].GetName()
            << "  (coste iniciativa: " << skills[i].GetInitiativeCost() << ")\n";
    }

    // Leer elección del jugador
    int skillChoice = -1;
    while (skillChoice < 0 || skillChoice >= static_cast<int>(skills.size()))
    {
        std::cout << "Opción: ";
        std::cin >> skillChoice;

        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            skillChoice = -1;
        }
    }

    Skill& chosenSkill = skills[skillChoice];

    // Elegir target entre los enemigos vivos
    auto aliveEnemies = GetAliveMembers(enemyParty);
    std::cout << "Elige objetivo:\n";
    for (int i = 0; i < static_cast<int>(aliveEnemies.size()); ++i)
    {
        std::cout << "  [" << i << "] " << aliveEnemies[i]->GetName()
            << "  HP: " << aliveEnemies[i]->GetCurrentHP() << "\n";
    }

    int targetChoice = -1;
    while (targetChoice < 0 || targetChoice >= static_cast<int>(aliveEnemies.size()))
    {
        std::cout << "Objetivo: ";
        std::cin >> targetChoice;
        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            targetChoice = -1;
        }
    }

    currentSkill = &chosenSkill;
    currentTarget = aliveEnemies[targetChoice];
}

//  ENEMY TURN — habilidad y target aleatorios
void Combat::EnemyTurn()
{
    static std::mt19937 rng(std::random_device{}());

    auto& skills = currentActor->GetSkills(); // los enemigos tienen 2

    // Habilidad aleatoria entre las disponibles (máximo 2)
    int maxSkills = std::min(static_cast<int>(skills.size()), 2);
    std::uniform_int_distribution<int> skillDist(0, maxSkills - 1);
    Skill& chosenSkill = skills[skillDist(rng)];

    // Target aliado vivo aleatorio
    auto aliveAllies = GetAliveMembers(alliedParty);
    if (aliveAllies.empty()) return; //avoid crash if aliveAllies is empty
    std::uniform_int_distribution<int> targetDist(0, static_cast<int>(aliveAllies.size()) - 1);
    Character* target = aliveAllies[targetDist(rng)];

    std::cout << currentActor->GetName() << " usa " << chosenSkill.GetName()
        << " sobre " << target->GetName() << ".\n";

    currentSkill = &chosenSkill;
    currentTarget = target;

    if (onPlaySound)
    {
        onPlaySound(chosenSkill.GetAnimationId());
    }
}

//  EXECUTE SKILL
void Combat::ExecuteSkill(Character* user, Skill& skill, Character* target)
{
    LOG("EXECUTE SKILL");

    // Check if the ability need access to the whole party
    if (skill. GetHasAreaEffect()) {
        //if the effect is for all the party, choose between enemy or allied party
        Party* targetParty = nullptr;

        if (IsAllied(user)) { //caster is ally
            if (skill.GetAreaEffectTargetAllies()) {
                targetParty = alliedParty; //buffs to allied party
            }
            else {
                targetParty = enemyParty; // attack to all enemy party
            }
        }
        else { //caster is enemy
            if (!skill.GetAreaEffectTargetAllies()) { //effect dont go to allies
                targetParty = enemyParty; //buffs to the whole enemy party
            }
            else {
                targetParty = alliedParty; //attack to the whole allied party
            }
        }

        std::cout << "  [AOE] " << user->GetName()
            << " usa [" << skill.GetName()
            << "] sobre todos los enemigos!\n";

        for (Character* c : GetAliveMembers(targetParty)) {

            int targetHpBefore = target->GetCurrentHP();

            // Aplicar multiplicador específico de este objetivo según su Lane
            float multiplier = GetLaneDamageMultiplier(c);
            c->SetIncomingDamageMultiplier(multiplier);

            skill.Use(user, c);

            // Limpiar multiplicador (volver a 1.0) tras el golpe
            c->SetIncomingDamageMultiplier(1.0f);

            int targetHpAfter = c->GetCurrentHP();
            int damageDone = targetHpBefore - targetHpAfter;

            //add feedback
            if (damageDone > 0)
            {
                LOG("Added damage pop up to target -> %s", c->GetName().c_str());
                c->AddDamagePopup(damageDone);
                c->OnHit();
            }
#if _DEBUG
            std::cout << user->GetName() << " usa " << skill.GetName()
                << " -> " << c->GetName() << "\n";
#endif

            //-----------------debug-----------------
            if (damageDone > 0)
            {
#if _DEBUG
                std::cout << "    Daño: " << damageDone
                    << " | HP " << c->GetName() << ": "
                    << targetHpBefore << " -> " << targetHpAfter;

                if (!c->GetIsAlive())
                {
                    std::cout << "  [MUERTO]";
                }

                std::cout << "\n";
#endif
            }
            else if (damageDone < 0)
            {
#if _DEBUG
                std::cout << "    Curación: " << (-damageDone)
                    << " | HP " << c->GetName() << ": "
                    << targetHpBefore << " -> " << targetHpAfter << "\n";
#endif
            }

            // Estado de efectos del target tras el ataque
            if (c->IsBurning())
            {
#if _DEBUG
                std::cout << "    " << c->GetName()
                    << " esta QUEMADO: " << c->GetBurnDamage() << " de daño/turno\n";
#endif
            }

            if (c->IsPoisoned())
            {
#if _DEBUG
                std::cout << "    " << c->GetName()
                    << " esta ENVENENADO: " << c->GetPoisonDamage() << " de daño/turno\n";
#endif
            }
#if _DEBUG

            std::cout << "    Iniciativa restante de " << user->GetName()
                << ": " << user->GetCurrentInitiative() << "\n";
            //------------------------------
#endif
        }

        // Restar el coste de iniciativa al usuario
        user->AddInitiative(-(skill.GetInitiativeCost()));
        std::cout << "  | Iniciativa restante: " << user->GetCurrentInitiative() << "\n";
    }
    else {

        int targetHpBefore = target->GetCurrentHP();

        // Aplicar multiplicador específico de este objetivo según su Lane
        float multiplier = GetLaneDamageMultiplier(target);
        target->SetIncomingDamageMultiplier(multiplier);

        //NO AREA EFFECT
        skill.Use(user, target);

        target->SetIncomingDamageMultiplier(1.0f);

        int targetHpAfter = target->GetCurrentHP();
        int damageDone = targetHpBefore - targetHpAfter;

        //add feedback
        if (damageDone > 0)
        {
            LOG("Added damage popo Up 1 single target");
            target->AddDamagePopup(damageDone);
            target->OnHit();
        }

        // Restar el coste de iniciativa al usuario
        user->AddInitiative(-(skill.GetInitiativeCost()));

        std::cout << user->GetName() << " usa " << skill.GetName()
            << " -> " << target->GetName()
            << "  | Iniciativa restante: " << user->GetCurrentInitiative() << "\n";
#if _DEBUG
        //-----------------debug-----------------
        if (damageDone > 0)
        {
            std::cout << "    Daño: " << damageDone
                << " | HP " << target->GetName() << ": "
                << targetHpBefore << " -> " << targetHpAfter;

            if (!target->GetIsAlive())
            {
                std::cout << "  [MUERTO]";
            }

            std::cout << "\n";
        }
        else if (damageDone < 0)
        {
            std::cout << "    Curación: " << (-damageDone)
                << " | HP " << target->GetName() << ": "
                << targetHpBefore << " -> " << targetHpAfter << "\n";
        }

        // Estado de efectos del target tras el ataque
        if (target->IsBurning())
        {
            std::cout << "    " << target->GetName()
                << " esta QUEMADO: " << target->GetBurnDamage() << " de daño/turno\n";
        }

        if (target->IsPoisoned())
        {
            std::cout << "    " << target->GetName()
                << " esta ENVENENADO: " << target->GetPoisonDamage() << " de daño/turno\n";
        }

        std::cout << "    Iniciativa restante de " << user->GetName()
            << ": " << user->GetCurrentInitiative() << "\n";
        //------------------------------
#endif
    }
    
}

// HELPERS
Character* Combat::GetHighestInitiativeActor()
{
    Character* best = nullptr;
    int bestInit = 99; // umbral mínimo para actuar es 100

    for (Character* c : GetAllCombatants())
    {
        if (c->GetIsAlive() && c->GetCurrentInitiative() > bestInit)
        {
            bestInit = c->GetCurrentInitiative();
            best = c;
        }
    }
    return best; // nullptr si nadie supera 100
}

std::vector<Character*> Combat::GetAllCombatants()
{
    std::vector<Character*> all;

    for (Character* c : alliedParty->GetMembers())
    {
        all.push_back(c);
    }

    for (Character* c : enemyParty->GetMembers())
    {
        all.push_back(c);
    }

    return all;
}

void Combat::SubmitPlayerChoice(int skillIndex, int targetIndex)
{
    std::cout << "---- SubmitPlayerChoice ----" << std::endl;

    if (state != CombatState::WAITING_FOR_PLAYER_INPUT) { return; }
    if (currentActor == nullptr) { return; }

    // --- PASS ------------------------------
    if (skillIndex == -1)
    {
        LOG("SubmitPlayerChoice | %s PASS (no initiative cost)", currentActor->GetName().c_str());
        currentSkill = nullptr;
        currentTarget = nullptr;
        currentActor->PlayAnimation("idle");
        state = CombatState::MODIFIERS;
        return;
    }

    // --- NORMAL ACTION ----------------------
    auto& skills = currentActor->GetSkills();
    //auto  aliveEnemies = GetAliveMembers(enemyParty);

    if (skillIndex < 0 || skillIndex >= (int)skills.size()) { return; }
    //if (targetIndex < 0 || targetIndex >= (int)aliveEnemies.size()) { return; }

    // --- CHECK INITIATIVE COST --------------
    int cost = skills[skillIndex].GetInitiativeCost();
    if (currentActor->GetCurrentInitiative() < cost)
    {
        LOG("SubmitPlayerChoice | %s cannot use %s — not enough initiative (%d < %d)",
            currentActor->GetName().c_str(),
            skills[skillIndex].GetName().c_str(),
            currentActor->GetCurrentInitiative(),
            cost);
        return; // rechaza la elección, CombatScene sigue en WAITING_FOR_PLAYER_INPUT
    }

    currentSkill = &skills[skillIndex];
    //currentTarget = aliveEnemies[targetIndex];

    if (currentSkill->GetHasAreaEffect())
    {
        // Skill de area: no necesita target individual
        // ExecuteSkill ignorará currentTarget y aplicará a toda la party
        currentTarget = nullptr;
    }
    else
    {
        auto aliveEnemies = GetAliveMembers(enemyParty);
        if (targetIndex < 0 || targetIndex >= (int)aliveEnemies.size()) { return; }
        currentTarget = aliveEnemies[targetIndex];
    }

    std::string anim = currentSkill->GetAnimationId();
    currentActor->PlayAnimation(anim);
    state = CombatState::ATTACK_ANIMATION;

    LOG("SubmitPlayerChoice | %s uses %s -> ATTACK_ANIMATION",
        currentActor->GetName().c_str(),
        currentSkill->GetName().c_str());
}

void Combat::ForceVictory()
{
    result = CombatResult::VICTORY;
    state = CombatState::END_COMBAT;
    runningCombat = false;
}

void Combat::ForceDefeat()
{
    result = CombatResult::DEFEAT;
    state = CombatState::END_COMBAT;
    runningCombat = false;
}

float Combat:: GetLaneDamageMultiplier(Character* c)
{
    float reduction = 0.0f;
    bool isAlly = IsAllied(c);

    //----test----
    int coversActive = 0;
    std::string targetLane = "";
    //------------

    LaneType characterLaneType;
    bool found = false;

    if (isAlly)
    {
        if (frontLane.occupant == c) { characterLaneType = LaneType::FRONT; found = true; }
        else if (sideLane.occupant == c) { characterLaneType = LaneType::SIDE;  found = true; }
        else if (backLane.occupant == c) { characterLaneType = LaneType::BACK;  found = true; }
    }
    else
    {
        if (enemyFrontLane.occupant == c) { characterLaneType = LaneType::FRONT; found = true; }
        else if (enemySideLane.occupant == c) { characterLaneType = LaneType::SIDE; found = true; }
        else if (enemyBackLane.occupant == c) { characterLaneType = LaneType::BACK; found = true; }
    }

    if (!found) { return 1.0f; }
    if (characterLaneType == LaneType::FRONT) { return 1.0f; }

    Character* front = nullptr;
    Character* side = nullptr;

    if (isAlly)
    {
        front = frontLane.occupant;
        side = sideLane.occupant;
    }
    else
    {
        front = enemyFrontLane.occupant;
        side = enemySideLane.occupant;
    }

    if (characterLaneType == LaneType::SIDE)
    {
        if (front != nullptr && front->GetIsAlive())
        {
            reduction = damageReductionLane;
        }
    }
    else if (characterLaneType == LaneType::BACK)
    {
        if (front != nullptr && front->GetIsAlive())
        {
            reduction += damageReductionLane;
        }
        if (side != nullptr && side->GetIsAlive())
        {
            reduction += damageReductionLane;
        }
    }

    //if (frontLane.occupant == c)
    //{
    //    //----test-----
    //    targetLane = "FRONT";
    //    LOG("LANE LOGIC: %s is in FRONT. No cover available. Multiplier: 1.0", c->GetName().c_str());
    //    //------------

    //    return 1.0f;
    //}

    //if (sideLane.occupant == c)
    //{
    //    targetLane = "SIDE";
    //    if (frontLane.occupant != nullptr && frontLane.occupant->GetIsAlive())
    //    {
    //        reduction = 0.15f;
    //        coversActive = 1;
    //    }
    //}
    //else if (backLane.occupant == c)
    //{
    //    targetLane = "BACK";
    //    if (frontLane.occupant != nullptr && frontLane.occupant->GetIsAlive())
    //    {
    //        reduction += 0.15f;
    //        coversActive++;
    //    }
    //    if (sideLane.occupant != nullptr && sideLane.occupant->GetIsAlive())
    //    {
    //        reduction += 0.15f;
    //        coversActive++;
    //    }
    //}

    float finalMultiplier = 1.0f - reduction;

#if _DEBUG
    std::string laneStr = "";
    if (characterLaneType == LaneType::FRONT)
    {
        laneStr = "FRONT";
    }
    else if (characterLaneType == LaneType::SIDE)
    {
        laneStr = "SIDE";
    }
    else if (characterLaneType == LaneType::BACK)
    {
        laneStr = "BACK";
    }

    std::string bandoStr = "";
    if (isAlly)
    {
        bandoStr = "ALLY";
    }
    else
    {
        bandoStr = "ENEMY";
    }

    LOG("LANE LOGIC: Target %s [%s][%s] | Reduccion: %.2f | Multiplicador: %.2f",
        c->GetName().c_str(),
        bandoStr.c_str(),
        laneStr.c_str(),
        reduction,
        finalMultiplier);

#endif // _DEBUG

    return finalMultiplier;
}

void Combat::ResumeFromNextRoundPause()
{
    if (state == CombatState::NEXT_ROUND_PAUSE)
    {
        state = CombatState::CALCULATE_INITIATIVE;
    }
}

std::vector<Character*> Combat::GetAliveMembers(Party* party)
{
    std::vector<Character*> alive;
    for (Character* c : party->GetMembers())
    {
        if (c->GetIsAlive() && !c->GetPendingToDie())
        {
            alive.push_back(c);
        }
    }
    return alive;
}

bool Combat::IsPartyDefeated(Party* party)
{
    return GetAliveMembers(party).empty();
}

bool Combat::IsAllied(Character* character)
{
    for (Character* c : alliedParty->GetMembers())
    {
        if (c == character) return true;
    }
    return false;
}

void Combat::AttackFeedback(float dt)
{
    feedbackTimer += dt;

    if (feedbackTimer >= feedbackDuration)
    {
        //delete and reset popUps
        for (Character* c : GetAllCombatants())
        {
            c->DeletePopUps();
            c->ClearHitFlash();
        }

        feedbackTimer = 0.0f;
        state = CombatState::MODIFIERS;
    }
}
