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

#include "Engine.h"
#include "Render.h"
#include "Textures.h"

#include "Log.h"

//  Posiciones predefinidas en pantalla
//  0-2: aliados (izquierda)  |  3-5: enemigos (derecha)
const Vector2D Combat::defaultPositions[6] = {
    Vector2D(200.f, 300.f),   // aliado 0
    Vector2D(200.f, 450.f),   // aliado 1
    Vector2D(200.f, 150.f),   // aliado 2
    Vector2D(800.f, 300.f),   // enemigo 0
    Vector2D(800.f, 450.f),   // enemigo 1
    Vector2D(800.f, 150.f)    // enemigo 2
};

Combat::Combat(Party* allied, Party* enemy)
    : alliedParty(allied)
    , enemyParty(enemy)
    , state(CombatState::START_COMBAT)
    , result(CombatResult::NONE)
    , currentActor(nullptr)
    , runningCombat(true)
{
}

Combat::~Combat()
{
}


void Combat::Run()
{
    if (!runningCombat) return;

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

//  START_COMBAT
void Combat::StartCombat()
{
    std::cout << "\n════════════════════=═════════════════\n";
    std::cout << "          COMBATE INICIADO            \n";
    std::cout << "══════════════════════════════════════\n";


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

    //save previous states in combat
    for (Character* c : alliedParty->GetMembers()) {
        preCombatValues[c] = c->TakePreCombatValues();
    }

    auto allCombatants = GetAllCombatants();

    // Asignar posiciones y resetear iniciativa acumulada
    for (int i = 0; i < static_cast<int>(allCombatants.size()); ++i)
    {
        Character* c = allCombatants[i];
        c->SetPosition(defaultPositions[i].getX(), defaultPositions[i].getY());
        c->ResetCurrentInitiative();   // currentInitiative = 0
        c->ClearStatusEffects();       // limpia veneno/quemadura del combate anterior si es necesario
    }
}

//  CALCULATE_INITIATIVE
//  Devuelve true cuando hay al menos un actor con >= 100
bool Combat::CalculateInitiative()
{
    for (Character* c : GetAllCombatants())
    {
        if (c->GetIsAlive())
        {
            int before = c->GetCurrentInitiative();

            int bonus = 50 + c->GetSpeed();
            c->AddInitiative(bonus);

            int after = c->GetCurrentInitiative();

            std::cout << "  " << c->GetName()
                << " | antes: " << before
                << " + " << bonus
                << " = " << after;

            if (after >= 100)
            {
                std::cout << "  [PUEDE ACTUAR]";
            }

            std::cout << "\n";
        }
    }

    currentActor = GetHighestInitiativeActor();

    if (currentActor != nullptr)
    {
        std::cout << "  >> Turno para: " << currentActor->GetName()
            << " (iniciativa: " << currentActor->GetCurrentInitiative() << ")\n";
    }

    return currentActor != nullptr;
}

//  ATTACK
void Combat::AttackStart()
{
    if (currentActor == nullptr) return;

    std::cout << "\n──────────────────────────────────────\n";
    std::cout << "│ TURNO DE: " << currentActor->GetName() << "\n";
    std::cout << "│ HP: " << currentActor->GetCurrentHP()
        << " | Iniciativa: " << currentActor->GetCurrentInitiative()
        << " | Power: " << currentActor->GetPower() << "\n";
    std::cout << "──────────────────────────────────────\n";

    if (IsAllied(currentActor))
    {
        PlayerTurn();
    }
    else
    {
        EnemyTurn();
    }

    //play animation
    std::string anim = currentSkill->GetAnimationId();
    currentActor->PlayAnimation(anim);

    state = CombatState::ATTACK_ANIMATION;
}

void Combat::AttackAnimation()
{
    if(currentActor->GetAnimationFinished())
    {
        state = CombatState::ATTACK_RESOLVE;
    }
}

void Combat::AttackResolve()
{
    ExecuteSkill(currentActor, *currentSkill, currentTarget);
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

            std::cout << "  [VENENO] " << c->GetName()
                << " sufre " << poisonDmg << " de daño por veneno."
                << " HP: " << hpBefore << " -> " << hpAfter;

            if (!c->GetIsAlive())
            {
                std::cout << "  [MUERTO]";
            }

            std::cout << "\n";
        }

        if (c->IsBurning())
        {
            //int burnDmg = c->GetBurnDamage();
            //c->TakeBurnDamage();
            //std::cout << c->GetName() << " sufre " << burnDmg << " de daño por quemadura.\n";

            anyModifier = true;
            int burnDmg = c->GetBurnDamage();
            int hpBefore = c->GetCurrentHP();
            c->TakeBurnDamage();
            int hpAfter = c->GetCurrentHP();

            std::cout << "  [QUEMADURA] " << c->GetName()
                << " sufre " << burnDmg << " de daño por quemadura."
                << " HP: " << hpBefore << " -> " << hpAfter;

            if (!c->GetIsAlive())
            {
                std::cout << "  [MUERTO]";
            }

            std::cout << "\n";
        }
        
        if (!anyModifier)
        {
            std::cout << "  [MODIFICADORES] Ningun efecto activo.\n";
        }
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

            skill.Use(user, c);

            int targetHpAfter = target->GetCurrentHP();
            int damageDone = targetHpBefore - targetHpAfter;

            std::cout << user->GetName() << " usa " << skill.GetName()
                << " -> " << target->GetName() << "\n";

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
        }

        // Restar el coste de iniciativa al usuario
        user->AddInitiative(-(skill.GetInitiativeCost()));
        std::cout << "  | Iniciativa restante: " << user->GetCurrentInitiative() << "\n";
    }
    else {

        //NO AREA EFFECT
        skill.Use(user, target);

        int targetHpAfter = target->GetCurrentHP();
        int damageDone = targetHpBefore - targetHpAfter;

        // Restar el coste de iniciativa al usuario
        user->AddInitiative(-(skill.GetInitiativeCost()));

        std::cout << user->GetName() << " usa " << skill.GetName()
            << " -> " << target->GetName()
            << "  | Iniciativa restante: " << user->GetCurrentInitiative() << "\n";

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