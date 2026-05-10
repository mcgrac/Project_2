#pragma once
#include "BaseScene.h"
#include "Island.h"
#include "Party.h"

struct SDL_Texture;
class Ship;

class IslandInteriorScene : public BaseScene
{
public:
    IslandInteriorScene(Island* island, Party* allied, Ship* _ship);
    ~IslandInteriorScene();

    void Load() override;
    void Update(float dt) override;
    void PostUpdate(float dt) override;
    void Unload() override;
    void LoadTextures() override;

    bool OnUIMouseClickEvent(UIElement* uiElement) override;

    //helpers
    void OnResume() override;
    void OnPause() override;
    void CreateUI();

private:
    Island* island;
    Party* alliedParty;
    Ship* ship;

    static constexpr int SHOP_BUTTON_ID = 1;
    static constexpr int HOSTEL_BUTTON_ID = 2;
    static constexpr int DOCKYARD_BUTTON_ID = 3;
    static constexpr int LEAVE_BUTTON_ID = 4;

    SDL_Texture* background;
    SDL_Texture* dockyardbutton;
    SDL_Texture* shopButton;
    SDL_Texture* hostelButton;
    SDL_Texture* exitButton;

    //audio variables
    int ambiance;
    void LoadSound();
    void unloadSound();
    void UpdateSound();

    //bool musicPlaying = false;

    //ambiance directory
    const char* humanAmb = "Assets/Audio/Music/Ambiance/crowd_talking.wav";
    const char* sirenAmb = "Assets/Audio/Music/Ambiance/ambiance_waves.wav";
    const char* reptileAmb = "Assets/Audio/Music/Ambiance/jungle_sounds.wav";

    //fx variables
    int buttonPress;
};
