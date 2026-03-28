#include "Combat.h"

#include "Character.h"
#include "Party.h"
#include "Skill.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <random>

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
{
}

//  Run() — bucle principal del combate
void Combat::Run()
{
    while (state != CombatState::END_COMBAT)
    {
        switch (state)
        {
        case CombatState::START_COMBAT:
            StartCombat();
            state = CombatState::CALCULATE_INITIATIVE;
            break;

        case CombatState::CALCULATE_INITIATIVE:
            // Acumula ticks hasta que alguien supere 100
            while (!CalculateInitiative()) { /* tick */ }
            state = CombatState::ATTACK;
            break;

        case CombatState::ATTACK:
            Attack();
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
            break;
        }
    }
}

//  START_COMBAT
void Combat::StartCombat()
{
    std::cout << "=== COMBATE INICIADO ===\n";

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
            c->AddInitiative(50 + c->GetSpeed());
        }
    }

    currentActor = GetHighestInitiativeActor();
    return currentActor != nullptr;
}

//  ATTACK
void Combat::Attack()
{
    if (currentActor == nullptr) return;

    std::cout << "\n>> Turno de: " << currentActor->GetName() << "\n";

    if (IsAllied(currentActor))
    {
        PlayerTurn();
    }
    else
    {
        EnemyTurn();
    }
}

//  MODIFIERS — veneno y quemadura
void Combat::ApplyModifiers()
{
    for (Character* c : GetAllCombatants())
    {
        if (!c->GetIsAlive()) continue;

        if (c->IsPoisoned())
        {
            int poisonDmg = c->GetPoisonDamage();
            c->TakePoisonDamage();
            std::cout << c->GetName() << " sufre " << poisonDmg << " de daño por veneno.\n";
        }

        if (c->IsBurning())
        {
            int burnDmg = c->GetBurnDamage();
            c->TakeBurnDamage();
            std::cout << c->GetName() << " sufre " << burnDmg << " de daño por quemadura.\n";
        }

        // Reducir duración de los estados (si usas turnos como contador)
        //c->TickStatusEffects();
    }
}

//  CHECK_DEFEAT
void Combat::CheckDefeat()
{
    if (IsPartyDefeated(enemyParty))
    {
        result = CombatResult::VICTORY;
        state = CombatState::END_COMBAT;
    }
    else if (IsPartyDefeated(alliedParty))
    {
        result = CombatResult::DEFEAT;
        state = CombatState::END_COMBAT;
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
        std::cout << "\n=== VICTORIA ===\n";

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
        std::cout << "\n=== GAME OVER ===\n";
        // go to the main map scene
        // p.ej: GameManager::GetInstance().LoadScene(SceneID::MAIN_MAP);
    }
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

    ExecuteSkill(currentActor, chosenSkill, aliveEnemies[targetChoice]);
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
    std::uniform_int_distribution<int> targetDist(0, static_cast<int>(aliveAllies.size()) - 1);
    Character* target = aliveAllies[targetDist(rng)];

    std::cout << currentActor->GetName() << " usa " << chosenSkill.GetName()
        << " sobre " << target->GetName() << ".\n";

    ExecuteSkill(currentActor, chosenSkill, target);
}

//  EXECUTE SKILL
void Combat::ExecuteSkill(Character* user, Skill& skill, Character* target)
{
    // Aplica el efecto de la habilidad (daño, heal, status…)
    // La lógica concreta vive en Skill::Apply() o similar
    skill.Use(user, target);

    // Restar el coste de iniciativa al usuario
    user->AddInitiative(-(skill.GetInitiativeCost()));

    std::cout << user->GetName() << " usa " << skill.GetName()
        << " -> " << target->GetName()
        << "  | Iniciativa restante: " << user->GetCurrentInitiative() << "\n";
}

//  HELPERS
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
            alive.push_back(c);
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