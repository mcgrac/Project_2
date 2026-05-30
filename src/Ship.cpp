#include "Ship.h"
#include "Engine.h"
#include "Audio.h"
#include "Textures.h"
#include "Render.h"
#include "Log.h"

const float Ship::MOVE_SPEED = 0.1f; // pixels per second

Ship::Ship()
    : position(Vector2D(100.0f, 100.0f)),
    currentHp(100),
    maxHp(100),
    level(1),
    sinked(false)
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
    level(level),
    sinked(false)
{
    //load animations
    std::unordered_map<int, std::string> aliases = { {21,"up"},{22,"idle"},{23,"down"} };
    anims.LoadFromTSX("Assets/Textures/Animations/Boat.tsx", aliases);
    anims.SetCurrent("idle");

    //load texture
    spritesheet = Engine::GetInstance().textures->Load("Assets/Textures/Animations/Boat.png");

    //load audio
    
}

Ship::~Ship()
{
    Engine::GetInstance().textures->UnLoad(spritesheet);
}

void Ship::Update(float dt) {

    if (isMoving)
    {
        UpdateMovement(dt);
    }
    UpdateSound();
    UpdateCamera();
    anims.Update(dt);

    if (pendingArrival)
    {
        pendingArrival = false;
        if (onArrivalCallback)
        {
            std::function<void()> callback = onArrivalCallback;
            onArrivalCallback = nullptr;
            callback();
        }
    }
}

void Ship::Draw(float dt) {

    const SDL_Rect& animFrame = anims.GetCurrentFrame();

    int drawX = (int)position.getX() - animFrame.w / 2;
    int drawY = (int)position.getY() + VISUAL_OFFSET_Y - animFrame.h / 2;

    Engine::GetInstance().render->DrawTexture(
        spritesheet,
        drawX,
        drawY,
        &animFrame
    );

}

#pragma region MOVEMENT SHIP
// -----------------------------------------------------------------------
// MoveToIsland
// Builds one or two movement segments based on the requested ShipMovement
// and stores the onArrival callback for when the last segment ends.
//
// All distances are in pixels on the world-map background:
//   HORIZONTAL_DIST = 198   (full column spacing)
//   HALF_H          = 99    (half of the above)
//   VERT_SMALL      = 76    (one row of vertical travel)
//   VERT_LARGE      = 152   (two rows of vertical travel)
// -----------------------------------------------------------------------
void Ship::MoveToIsland(ShipMovement movement, std::function<void()> onArrival)
{
    static const float HORIZONTAL_DIST = 198.0f;
    static const float HALF_H = 99.0f;
    static const float VERT_SMALL = 76.0f;
    static const float VERT_LARGE = 152.0f;

    onArrivalCallback = onArrival;

    float startX = position.getX();
    float startY = position.getY();

    if (movement == ShipMovement::STRAIGHT)
    {
        // Single segment: move straight right by HORIZONTAL_DIST
        segCount = 1;
        segStart[0] = Vector2D(startX, startY);
        segEnd[0] = Vector2D(startX + HORIZONTAL_DIST, startY);
    }
    else
    {
        // Two segments:
        //   Segment 0: advance HALF_H horizontally + rise/fall vertically
        //   Segment 1: advance the remaining HALF_H horizontally

        float yDelta = 0.0f;

        if (movement == ShipMovement::UP)
        {
            yDelta = -VERT_SMALL;
        }
        else if (movement == ShipMovement::DOWN)
        {
            yDelta = VERT_SMALL;
        }
        else if (movement == ShipMovement::UP2)
        {
            yDelta = -VERT_LARGE;
        }
        else if (movement == ShipMovement::DOWN2)
        {
            yDelta = VERT_LARGE;
        }

        float midX = startX + HALF_H;
        float midY = startY + yDelta;

        segCount = 2;
        segStart[0] = Vector2D(startX, startY);
        segEnd[0] = Vector2D(midX, midY);

        segStart[1] = Vector2D(midX, midY);
        segEnd[1] = Vector2D(midX + HALF_H, midY);
    }

    currentSeg = 0;
    segT = 0.0f;
    isMoving = true;

    // Set animation based on direction
    if (movement == ShipMovement::UP || movement == ShipMovement::UP2)
    {
        anims.SetCurrent("up");
    }
    else if (movement == ShipMovement::DOWN || movement == ShipMovement::DOWN2)
    {
        anims.SetCurrent("down");
    }
    else
    {
        anims.SetCurrent("idle");
    }
}

void Ship::UpdateMovement(float dt)
{
    // Distance to cover in this segment
    Vector2D start = segStart[currentSeg];
    Vector2D end = segEnd[currentSeg];

    float dx = end.getX() - start.getX();
    float dy = end.getY() - start.getY();
    float segLen = SDL_sqrtf(dx * dx + dy * dy);

    if (segLen < 0.001f)
    {
        // Zero-length segment — skip it immediately
        segT = 1.0f;
    }
    else
    {
        segT += (MOVE_SPEED / segLen) * dt;
    }

    if (segT >= 1.0f)
    {
        segT = 1.0f;
        position = end;

        currentSeg++;

        if (currentSeg >= segCount)
        {
            // All segments done
            isMoving = false;
            anims.SetCurrent("idle");
            pendingArrival = true;  // fire next frame, after Draw

        }
        else
        {
            anims.SetCurrent("idle");
            segT = 0.0f;
        }
    }
    else
    {
        // Interpolate position within segment
        position = Vector2D(
            start.getX() + dx * segT,
            start.getY() + dy * segT
        );
    }
}

void Ship::UpdateCamera()
{
    static const float MAP_WIDTH = 224.0f + 448.0f * 29.0f + 224.0f; // total world width in pixels

    Render* render = Engine::GetInstance().render.get();

    float camW = (float)render->camera.w;
    float shipX = position.getX() + 125.0f; // use visual center (with offset)

    float limitLeft = camW / 4.0f;
    float limitRight = MAP_WIDTH - camW * 3.0f / 4.0f;

    if (shipX - limitLeft > 0.0f && shipX < limitRight)
    {
        render->camera.x = (int)(-shipX + camW / 4.0f);
    }
    else if (shipX <= limitLeft)
    {
        render->camera.x = 0;
    }
    else
    {
        render->camera.x = (int)(-MAP_WIDTH + camW);
    }
}
#pragma endregion

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

    if (currentHp <= 0)
    {
        currentHp = 0;
        sinked = true;
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
    if(level < MAX_LEVEL_SHIP)
    {
        level += 1;
        maxHp += HP_PER_LEVEL;
        currentHp += HP_PER_LEVEL;
    }

#if _DEBUG
    LOG("Ship improved to level -> %d", level);
    LOG("maxHp improved from: %d to %d", maxHp - HP_PER_LEVEL, maxHp);
    LOG("current HP modifies from: %d to %d", currentHp - HP_PER_LEVEL, currentHp);
#endif // _DEBUG

}

void Ship::LoseBattle()
{
    TakeDamage(HP_LOST_FOR_BATTLE);
}

//used to load all the sounds into the variables
void Ship::LoadAudio() {
    movingfx = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Island_menu/Ship_moving.wav");
    dockShip = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Island_menu/ship_arriving_port.wav");
    shipLeave = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Island_menu/ship_setting_sail.wav");
}

void Ship::UpdateSound() {
    if (isMoving && movingAudioPlaying == false) {
        //start playing moving sound
        Engine::GetInstance().audio->PlayFx(movingfx);
        movingAudioPlaying = true;
    }
    lastMovState = isMoving;
    //check if the ship has stopped moving since last frame, if so, play docking sound
    if (lastMovState == true && isMoving == false) {
        //play docking sound
        Engine::GetInstance().audio->PlayFx(dockShip);
        movingAudioPlaying = false;
    }
    //check if the ship has started moving since last frame, if so, play leaving sound
    if (lastMovState == false && isMoving == true) {
        //play docking sound
        Engine::GetInstance().audio->PlayFx(shipLeave);
    }
}