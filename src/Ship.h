#pragma once

#include "Vector2D.h"
#include "Animation.h"

struct SDL_Texture;

class Ship
{
public:
    Ship();
    Ship(Vector2D position, int maxHp, int level);
    ~Ship();

    // Getters
    Vector2D GetPosition() const { return position; }
    int GetCurrentHp() const { return currentHp; }
    int GetMaxHp() const { return maxHp; }
    int GetLevel() const { return level; }
    bool IsAlive() const;

    void SetPosition(Vector2D newPosition) { position = newPosition; }
    void TakeDamage(int amount);
    void Heal(int amount);
    void LevelUp();
    void Draw();

private:
    Vector2D position;
    int currentHp;
    int maxHp;
    int level;

    //animations
    AnimationSet anims;
    SDL_Texture* spritesheet;

    static const int HP_PER_LEVEL = 20;
};
