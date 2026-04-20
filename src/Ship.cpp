#include "Ship.h"
#include "Engine.h"
#include "Textures.h"
#include "Render.h"

Ship::Ship()
    : position(Vector2D(0.0f, 0.0f)),
      currentHp(100),
      maxHp(100),
      level(1)
{
    //load animations
    std::unordered_map<int, std::string> aliases = { {21,"up"},{22,"idle"},{23,"down"} };
    anims.LoadFromTSX("Assets/Textures/Animations/Boat.tsx", aliases);
    anims.SetCurrent("idle");

    //load texture
    spritesheet = Engine::GetInstance().textures->Load("Assets/Textures/Animations/Boat.png");
}

Ship::Ship(Vector2D position, int maxHp, int level)
    : position(position),
      currentHp(maxHp),
      maxHp(maxHp),
      level(level)
{
    //load animations
    std::unordered_map<int, std::string> aliases = { {21,"up"},{22,"idle"},{23,"down"} };
    anims.LoadFromTSX("Assets/Textures/Animations/Boat.tsx", aliases);
    anims.SetCurrent("idle");

    //load texture
    spritesheet = Engine::GetInstance().textures->Load("Assets/Textures/Animations/Boat.png");
}

Ship::~Ship()
{
    Engine::GetInstance().textures->UnLoad(spritesheet);
}

void Ship::Update(float dt) {

    Draw(dt);

}

void Ship::Draw(float dt) {

    anims.Update(dt);

    const SDL_Rect& animFrame = anims.GetCurrentFrame();

    int drawX = (int)position.getX() - animFrame.w / 2;
    int drawY = (int)position.getY() - animFrame.h / 2;

    Engine::GetInstance().render->DrawTexture(
        spritesheet,
        drawX,
        drawY,
        &animFrame
    );

}

bool Ship::IsAlive() const
{
    return currentHp >= 0;
}

void Ship::TakeDamage(int amount)
{
    if (amount < 0)
    {
        return;
    }

    currentHp -= amount;

    if (currentHp < 0)
    {
        currentHp = 0;
    }
}

void Ship::Heal(int amount)
{
    if (amount < 0)
    {
        return;
    }

    currentHp += amount;

    if (currentHp > maxHp)
    {
        currentHp = maxHp;
    }
}

void Ship::LevelUp()
{
    level += 1;
    maxHp += HP_PER_LEVEL;
    currentHp += HP_PER_LEVEL;
}

void Ship::Draw()
{
}
