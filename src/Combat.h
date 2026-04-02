#pragma once
#include <vector>
#include <string>
#include "Vector2D.h"
#include "Character.h"
#include <unordered_map>

// Forward declarations
class Character;
class Party;
class Skill;

struct SDL_Texture;

enum class CombatState
{
    START_COMBAT,
    CALCULATE_INITIATIVE,

    ATTACK,

    ATTACK_START,
    ATTACK_ANIMATION,
    ATTACK_RESOLVE,

    MODIFIERS,
    CHECK_DEFEAT,
    END_COMBAT
};

enum class CombatResult
{
    NONE,
    VICTORY,
    DEFEAT
};

class Combat
{
public:

    Combat(Party* allied, Party* enemy);
    ~Combat();

    // Whole combat cycle
    void Run();
    bool CombatIsFinished() const;
    std::vector<Character*> GetAllCombatants();

    std::unordered_map<Character*, Character::PreCombatValues> preCombatValues;

private:

    Party* alliedParty;
    Party* enemyParty;

    CombatState state;
    CombatResult result;

    Character* currentActor;    //one with most iniciative (attacking)

    Skill* currentSkill = nullptr;
    Character* currentTarget = nullptr;

    // ── Posiciones predefinidas ───────────────
    // Índice 0-2: aliados  |  Índice 3-5: enemigos
    // Puedes cambiar el tipo a sf::Vector2f si usas SFML, etc.
    Vector2D position; 
    static const Vector2D defaultPositions[6];

    bool runningCombat; //control

    void StartCombat();

    bool CalculateInitiative();

    void Attack();
    void AttackStart();
    void AttackAnimation();
    void AttackResolve();


    void ApplyModifiers();

    void CheckDefeat();

    void EndCombat();

    void PlayerTurn();

    void EnemyTurn();

    void ExecuteSkill(Character* user, Skill& skill, Character* target);

    Character* GetHighestInitiativeActor();

    std::vector<Character*> GetAliveMembers(Party* party);

    bool IsPartyDefeated(Party* party);

    bool IsAllied(Character* character);
};