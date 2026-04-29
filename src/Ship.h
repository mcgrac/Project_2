#pragma once
#include "Vector2D.h"
#include "Animation.h"
#include <functional>

struct SDL_Texture;

enum class ShipMovement
{
    STRAIGHT,
    UP,
    DOWN,
    UP2,
    DOWN2
};

class Ship
{
public:
    Ship();
    Ship(Vector2D position, int maxHp, int level);
    ~Ship();

    void Update(float dt);
    void Draw(float dt);

    // Getters
    Vector2D GetPosition() const { return position; }
    int GetCurrentHp() const { return currentHp; }
    int GetMaxHp() const { return maxHp; }
    int GetLevel() const { return level; }
    bool IsAlive() const;
    bool IsMoving() const { return isMoving; }

    void SetPosition(Vector2D newPosition) { position = newPosition; }
    void TakeDamage(int amount);
    void Heal(int amount);
    void LevelUp();
    void LoseBattle();

    // Starts the ship animation toward the destination island.
    // onArrival is called once the ship reaches the final position.
    void MoveToIsland(ShipMovement movement, std::function<void()> onArrival);

private:
    void UpdateMovement(float dt);

    Vector2D position;
    int currentHp;
    int maxHp;
    int level;
    bool sinked;

    // --- movement state ---
    bool isMoving = false;

    // A movement is split into at most two linear segments.
    // Segment 0: horizontal half + vertical rise/fall
    // Segment 1: remaining horizontal
    // For STRAIGHT there is only one segment (full horizontal, no Y delta).

    Vector2D segStart[2];   // start position of each segment
    Vector2D segEnd[2];     // end position of each segment
    int segCount = 0;  // 1 or 2
    int currentSeg = 0;
    float segT = 0.0f;   // normalised progress [0,1] within current segment

    static const float MOVE_SPEED; // units per second — tune to taste

    std::function<void()> onArrivalCallback;
    bool pendingArrival = false;

    //animations
    AnimationSet anims;
    SDL_Texture* spritesheet;

    static const int HP_PER_LEVEL = 20;
    static const int HP_LOST_FOR_BATTLE = 25;

    static const int VISUAL_OFFSET_Y = 111; // 300 - 189 = 111
};
