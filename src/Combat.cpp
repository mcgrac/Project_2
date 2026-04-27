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

// ---------- Lane assignment (called by CombatScene) --------------------------------
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


void Combat::Run()
{
    if (!runningCombat) return;

#if DEBUG
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
        if (!CalculateInitiative())
        {
            //dont do anything -> go to next frame
        }
        else
        {
            //state = CombatState::ATTACK;
            state = CombatState::ATTACK_START;
        }
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
        state = CombatState::MODIFIERS;
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

#if DEBUG
    //----------------debug--------------
    std::cout << "\n[ALIADOS]\n";
    for (Character* c : alliedParty->GetMembers())
    {
        std::cout << "  " << c->GetName()
            << " | HP: " << c->GetCurrentHP()
            << " | Power: " << c->GetPower()
            << " | Speed: " << c->GetSpeed() << "\n";
    }

    std::cout << "\n[ENEMIGOS]\n";
    for (Character* c : enemyParty->GetMembers())
    {
        std::cout << "  " << c->GetName()
            << " | HP: " << c->GetCurrentHP()
            << " | Power: " << c->GetPower()
            << " | Speed: " << c->GetSpeed() << "\n";
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

    auto allCombatants = GetAllCombatants();

    // Allies: position determined by lane assignment
    // defaultPositions[0] = Front (closest to enemies)
    // defaultPositions[1] = Side  (middle)
    // defaultPositions[2] = Back  (furthest from enemies)
    if (frontLane.occupant != nullptr)
    {
        frontLane.occupant->SetPosition(defaultPositions[0].getX(), defaultPositions[0].getY());
    }
    if (sideLane.occupant != nullptr)
    {
        sideLane.occupant->SetPosition(defaultPositions[1].getX(), defaultPositions[1].getY());
    }
    if (backLane.occupant != nullptr)
    {
        backLane.occupant->SetPosition(defaultPositions[2].getX(), defaultPositions[2].getY());
    }

    // Allies: reset initiative
    for (Character* c : alliedParty->GetMembers())
    {
        c->ResetCurrentInitiative();
    }
    //// Asignar posiciones y resetear iniciativa acumulada
    //for (int i = 0; i < static_cast<int>(allCombatants.size()); ++i)
    //{
    //    Character* c = allCombatants[i];
    //    c->SetPosition(defaultPositions[i].getX(), defaultPositions[i].getY());
    //    c->ResetCurrentInitiative();   // currentInitiative = 0
    //}

    // Enemies: position by index, starting at slot 3
    auto& enemies = enemyParty->GetMembers();
    for (int i = 0; i < static_cast<int>(enemies.size()); ++i)
    {
        enemies[i]->SetPosition(defaultPositions[3 + i].getX(), defaultPositions[3 + i].getY());
        enemies[i]->ResetCurrentInitiative();
    }

#if DEBUG
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
    for (Character* c : GetAllCombatants())
    {
        if (c->GetIsAlive())
        {
            int before = c->GetCurrentInitiative();

            int bonus = 50 + c->GetTotalSpeed();
            c->AddInitiative(bonus);

            int after = c->GetCurrentInitiative();

#if DEBUG
            std::cout << "  " << c->GetName()
                << " | antes: " << before
                << " + " << bonus
                << " = " << after;
#endif
            if (after >= 100)
            {
                std::cout << "  [PUEDE ACTUAR]";
            }

            std::cout << "\n";
        }
    }

    currentActor = GetHighestInitiativeActor();

#if DEBUG
    if (currentActor != nullptr)
    {
        std::cout << "  >> Turno para: " << currentActor->GetName()
            << " (iniciativa: " << currentActor->GetCurrentInitiative() << ")\n";
    }
#endif
    return currentActor != nullptr;
}

//  ATTACK
void Combat::AttackStart()
{
    if (currentActor == nullptr) return;

#if DEBUG
    std::cout << "\n──────────────────────────────────────\n";
    std::cout << "│ TURNO DE: " << currentActor->GetName() << "\n";
    std::cout << "│ HP: " << currentActor->GetCurrentHP()
        << " | Iniciativa: " << currentActor->GetCurrentInitiative()
        << " | Power: " << currentActor->GetPower() << "\n";
    std::cout << "──────────────────────────────────────\n";
#endif
    if (IsAllied(currentActor))
    {
        //Esperar a que CombatScene entregue la eleccion via SubmitPlayerChoice
        state = CombatState::WAITING_FOR_PLAYER_INPUT;
        return;   // ← salir sin avanzar

        //PlayerTurn();
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
        if (!c->GetIsAlive()) continue;

        if (c->IsPoisoned())
        {
            //int poisonDmg = c->GetPoisonDamage();
            //c->TakePoisonDamage();
            //std::cout << c->GetName() << " sufre " << poisonDmg << " de daño por veneno.\n";

            anyModifier = true;
            int poisonDmg = c->GetPoisonDamage();
            int hpBefore = c->GetCurrentHP();
            c->TakePoisonDamage();
            int hpAfter = c->GetCurrentHP();

#if DEBUG
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

#if DEBUG
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
#if DEBUG
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
        state = CombatState::CALCULATE_INITIATIVE;
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

        // Distribuir XP a los aliados vivos
        int totalXP = enemyParty->GetTotalXPReward();
        int totalGold = enemyParty->GetTotalGoldReward();
        auto lootItems = enemyParty->GetLootItems();

        for (Character* ally : GetAliveMembers(alliedParty))
        {
            ally->GainExperience(totalXP);
            std::cout << ally->GetName() << " gana " << totalXP << " XP.\n";
        }

        alliedParty->AddGold(totalGold);
        std::cout << "Recompensa: " << totalGold << " de oro.\n";

        //for (auto& item : lootItems)
        //{
        //    alliedParty->AddItem(item);
        //    std::cout << "Item obtenido: " << item->GetName() << "\n";
        //}
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
        //add damage to the ship
    }

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

    //ExecuteSkill(currentActor, chosenSkill, aliveEnemies[targetChoice]);
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

    //ExecuteSkill(currentActor, chosenSkill, target);
}

//  EXECUTE SKILL
void Combat::ExecuteSkill(Character* user, Skill& skill, Character* target)
{
    int targetHpBefore = target->GetCurrentHP();

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

            // Aplicar multiplicador específico de este objetivo según su Lane
            float multiplier = GetLaneDamageMultiplier(c);
            c->SetIncomingDamageMultiplier(multiplier);

            skill.Use(user, c);

            // Limpiar multiplicador (volver a 1.0) tras el golpe
            c->SetIncomingDamageMultiplier(1.0f);

            int targetHpAfter = target->GetCurrentHP();
            int damageDone = targetHpBefore - targetHpAfter;
#if DEBUG
            std::cout << user->GetName() << " usa " << skill.GetName()
                << " -> " << target->GetName() << "\n";
#endif

            //-----------------debug-----------------
            if (damageDone > 0)
            {
#if DEBUG
                std::cout << "    Daño: " << damageDone
                    << " | HP " << target->GetName() << ": "
                    << targetHpBefore << " -> " << targetHpAfter;

                if (!target->GetIsAlive())
                {
                    std::cout << "  [MUERTO]";
                }

                std::cout << "\n";
#endif
            }
            else if (damageDone < 0)
            {
#if DEBUG
                std::cout << "    Curación: " << (-damageDone)
                    << " | HP " << target->GetName() << ": "
                    << targetHpBefore << " -> " << targetHpAfter << "\n";
#endif
            }

            // Estado de efectos del target tras el ataque
            if (target->IsBurning())
            {
#if DEBUG
                std::cout << "    " << target->GetName()
                    << " esta QUEMADO: " << target->GetBurnDamage() << " de daño/turno\n";
#endif
            }

            if (target->IsPoisoned())
            {
#if DEBUG
                std::cout << "    " << target->GetName()
                    << " esta ENVENENADO: " << target->GetPoisonDamage() << " de daño/turno\n";
#endif
            }
#if DEBUG

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

        // Aplicar multiplicador específico de este objetivo según su Lane
        float multiplier = GetLaneDamageMultiplier(target);
        target->SetIncomingDamageMultiplier(multiplier);

        //NO AREA EFFECT
        skill.Use(user, target);

        target->SetIncomingDamageMultiplier(1.0f);

        int targetHpAfter = target->GetCurrentHP();
        int damageDone = targetHpBefore - targetHpAfter;

        // Restar el coste de iniciativa al usuario
        user->AddInitiative(-(skill.GetInitiativeCost()));

        std::cout << user->GetName() << " usa " << skill.GetName()
            << " -> " << target->GetName()
            << "  | Iniciativa restante: " << user->GetCurrentInitiative() << "\n";
#if DEBUG
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

    std::cout << "Combat state: " << (int)state << std::endl;

    if (currentActor == nullptr)
    {
        std::cout << "ERROR: currentActor is NULL" << std::endl;
        return;
    }

    std::cout << "Current actor: " << currentActor->GetName() << std::endl;

    if (state != CombatState::WAITING_FOR_PLAYER_INPUT) return;
    if (currentActor == nullptr) return;

    auto& skills = currentActor->GetSkills();
    auto aliveEnemies = GetAliveMembers(enemyParty);

    std::cout << "SkillIndex received: " << skillIndex << std::endl;
    std::cout << "TargetIndex received: " << targetIndex << std::endl;

    std::cout << "Skills available: " << skills.size() << std::endl;
    std::cout << "Alive enemies: " << aliveEnemies.size() << std::endl;

    if (skillIndex < 0 || skillIndex >= (int)skills.size()) return;
    if (targetIndex < 0 || targetIndex >= (int)aliveEnemies.size()) return;

    if (state != CombatState::WAITING_FOR_PLAYER_INPUT)
    {
        std::cout << "ERROR: Combat state is not WAITING_FOR_PLAYER_INPUT" << std::endl;
        return;
    }

    if (skillIndex < 0 || skillIndex >= (int)skills.size())
    {
        std::cout << "ERROR: Invalid skillIndex" << std::endl;
        return;
    }

    if (targetIndex < 0 || targetIndex >= (int)aliveEnemies.size())
    {
        std::cout << "ERROR: Invalid targetIndex" << std::endl;
        return;
    }

    currentSkill = &skills[skillIndex];
    currentTarget = aliveEnemies[targetIndex];

    std::cout << "Skill selected: " << currentSkill->GetName() << std::endl;
    std::cout << "Target selected: " << currentTarget->GetName() << std::endl;

    // play animation and change state
    std::string anim = currentSkill->GetAnimationId();

    std::cout << "Playing animation: " << anim << std::endl;

    currentActor->PlayAnimation(anim);
    state = CombatState::ATTACK_ANIMATION;

    std::cout << "State changed to ATTACK_ANIMATION" << std::endl;
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
    // Si el objetivo es un enemigo, no aplicamos bonos de lane (o puedes definir otros)
    if (!IsAllied(c)) return 1.0f;
    float reduction = 0.0f;

    //----test----
    int coversActive = 0;
    std::string targetLane = "";
    //------------

    if (frontLane.occupant == c)
    {
        //----test-----
        targetLane = "FRONT";
        LOG("LANE LOGIC: %s is in FRONT. No cover available. Multiplier: 1.0", c->GetName().c_str());
        //------------

        return 1.0f;
    }

    if (sideLane.occupant == c)
    {
        //// El SIDE solo tiene reducción si el FRONT está vivo
        //if (frontLane.occupant != nullptr && frontLane.occupant->GetIsAlive())
        //{
        //    reduction = 0.15f; // 15% de reducción
        //}

        targetLane = "SIDE";
        if (frontLane.occupant != nullptr && frontLane.occupant->GetIsAlive())
        {
            reduction = 0.15f;
            coversActive = 1;
        }
    }
    else if (backLane.occupant == c)
    {
        //// El BACK comprueba cuántos tiene delante vivos
        //if (frontLane.occupant != nullptr && frontLane.occupant->GetIsAlive())
        //{
        //    reduction += 0.15f;
        //}
        //if (sideLane.occupant != nullptr && sideLane.occupant->GetIsAlive())
        //{
        //    reduction += 0.15f;
        //}

        targetLane = "BACK";
        if (frontLane.occupant != nullptr && frontLane.occupant->GetIsAlive())
        {
            reduction += 0.15f;
            coversActive++;
        }
        if (sideLane.occupant != nullptr && sideLane.occupant->GetIsAlive())
        {
            reduction += 0.15f;
            coversActive++;
        }
    }

    float finalMultiplier = 1.0f - reduction;

    //------test------
    LOG("LANE LOGIC: Target %s [%s] | Coberturas activas: %d | Reduccion: %.2f | Multiplicador Final: %.2f",
        c->GetName().c_str(),
        targetLane.c_str(),
        coversActive,
        reduction,
        finalMultiplier);
    //----------------

    return finalMultiplier;
}

std::vector<Character*> Combat::GetAliveMembers(Party* party)
{
    std::vector<Character*> alive;
    for (Character* c : party->GetMembers())
    {
        if (c->GetIsAlive())
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