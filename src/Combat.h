#pragma once
#include <vector>
#include <string>
#include"Vector2D.h"

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

    // Whole combat cycle
    void Run(SDL_Texture* background);

    inline bool GetRunningCombat() { return runningCombat; }

private:

    Party* alliedParty;
    Party* enemyParty;

    CombatState state;
    CombatResult result;

    Character* currentActor;    //one with most iniciative (attacking)

    // ── Posiciones predefinidas ───────────────
    // Índice 0-2: aliados  |  Índice 3-5: enemigos
    // Puedes cambiar el tipo a sf::Vector2f si usas SFML, etc.
    Vector2D position; 
    static const Vector2D defaultPositions[6];

    bool runningCombat; //control

    // ─────────────────────────────────────────
    //  Fases del combate
    // ─────────────────────────────────────────

    // Inicializa posiciones y stats temporales
    void StartCombat();

    // Acumula iniciativa y decide quién actúa
    // Devuelve false si nadie llega aún a 100 (hace otro tick)
    bool CalculateInitiative();

    // Fase de ataque: jugador o IA según el actor
    void Attack();

    // Aplica daño de veneno/quemadura a todos los afectados
    void ApplyModifiers();

    // Comprueba si una party entera está derrotada
    void CheckDefeat();

    // Distribuye XP y recompensas (victoria) o lanza Game Over (derrota)
    void EndCombat();


    // Jugador elige habilidad (0-4) y target entre enemigos vivos
    void PlayerTurn();

    // Enemigo elige habilidad aleatoria (0-1) y target aliado vivo aleatorio
    void EnemyTurn();

    // Ejecuta la habilidad sobre el target y resta su coste de iniciativa
    void ExecuteSkill(Character* user, Skill& skill, Character* target);


    // Devuelve puntero al personaje con más iniciativa >= 100, o nullptr
    Character* GetHighestInitiativeActor();

    // Todos los personajes de combate (aliados + enemigos)
    std::vector<Character*> GetAllCombatants();

    // Personajes vivos de una party
    std::vector<Character*> GetAliveMembers(Party* party);

    // True si todos los miembros de la party están muertos
    bool IsPartyDefeated(Party* party);

    // Devuelve true si el personaje pertenece al equipo aliado
    bool IsAllied(Character* character);
};