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

    FILL_QUEUE,
    PROCESS_QUEUE,

    ATTACK_START,
    WAITING_FOR_PLAYER_INPUT,

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

// ------------------- Lanes -------------------------------------
// Each lane holds a reference to the character assigned to it and
// computes bonuses that scale with the ship level.
//
// Base bonuses (level 1):
//   Back  → +15 power
//   Side  → +10 power, +10 speed
//   Front → +10 speed, +10 max health
//
// Every stat is multiplied by shipLevel, so level 2 doubles them, etc.
// Maximum ship level is 3.

enum class LaneType
{
    BACK,
    SIDE,
    FRONT
};

struct Lane
{
    LaneType type;
    Character* occupant = nullptr;

    // Base bonus values (level 1)
    static const int BASE_BACK_POWER = 15;
    static const int BASE_SIDE_POWER = 10;
    static const int BASE_SIDE_SPEED = 10;
    static const int BASE_FRONT_SPEED = 10;
    static const int BASE_FRONT_HEALTH = 10;

    int GetPowerBonus(int shipLevel) const;
    int GetSpeedBonus(int shipLevel) const;
    int GetHealthBonus(int shipLevel) const;
};

class Combat
{
public:

    Combat(Party* allied, Party* enemy, int _shipLevel);
    ~Combat();

    // Whole combat cycle
    void Run();
    bool CombatIsFinished() const;
    std::vector<Character*> GetAllCombatants();

    std::unordered_map<Character*, Character::PreCombatValues> preCombatValues;

    //getter
    inline bool GetWaitingForInput() const { return state == CombatState::WAITING_FOR_PLAYER_INPUT; }
    inline bool IsWaitingAnimation() const { return state == CombatState::ATTACK_ANIMATION; }
    inline Character* GetCurrentActor() const { return currentActor; }
    inline std::vector<Character*> GetAliveEnemies() { return GetAliveMembers(enemyParty); }
    inline std::vector<Character*> GetAliveAllies() { return GetAliveMembers(alliedParty); }

    // Lane assignment — called by CombatScene during lane selection phase
    void AssignLane(Character* character, LaneType laneType);

    void SubmitPlayerChoice(int skillIndex, int targetIndex);

    //testing
    void ForceVictory();
    void ForceDefeat();

    float GetLaneDamageMultiplier(Character* c);

private:

    Party* alliedParty;
    Party* enemyParty;

    CombatState state;
    CombatResult result;

    Character* currentActor = nullptr;    //one with most iniciative (attacking)
    Skill* currentSkill = nullptr;
    Character* currentTarget = nullptr;

    int shipLevel;

    // Three fixed lanes for allied characters
    Lane backLane;
    Lane sideLane;
    Lane frontLane;

    void ApplyLaneBonuses();

    // Queue of actors that have reached >= 100 initiative this round,
    // sorted highest-first. Drained before the next initiative tick.
    std::vector<Character*> actorsQueue;

    // ── Posiciones predefinidas ───────────────
    // Índice 0-2: aliados  |  Índice 3-5: enemigos
    // Puedes cambiar el tipo a sf::Vector2f si usas SFML, etc.
    Vector2D position; 
    static const Vector2D defaultPositions[6];

    bool runningCombat; //control

    void ResetBonusStats(Character* c);
    void StartCombat();
    bool CalculateInitiative();
    void FillQueue();            // build actorsQueue from everyone >= 100, sorted desc
    void ProcessQueue();         // pop next actor; if empty -> CALCULATE_INITIATIVE

    //Attack
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