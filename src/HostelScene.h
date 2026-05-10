#pragma once
#include "BaseScene.h"
#include "Hostel.h"
#include "Party.h"

struct SDL_Texture;

class HostelScene : public BaseScene
{
public:
    HostelScene(Hostel* hostel, Party* allied);
    ~HostelScene();

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

    void OpenRestPanel();
    void OpenSelectCharaPanel();

private:
    Hostel* hostel;         
    Party* alliedParty;   

    static constexpr int BACK_BUTTON_ID = 1;
    static constexpr int START_DIALOGUE = 10;

#pragma region TEXTURES
    SDL_Texture* exitButton;
    SDL_Texture* background;
    SDL_Texture* ownerSprite;
    SDL_Texture* restButton;
    SDL_Texture* mealButton;
    SDL_Texture* emptyButtons;
#pragma endregion


#pragma region POSITIONS
    static constexpr int REST_BUTTON_X = 100;
    static constexpr int REST_BUTTON_Y = 50;    
    static constexpr int REST_BUTTON_W = 414;
    static constexpr int REST_BUTTON_H = 414;

    static constexpr int MEAL_BUTTON_X = 500;
    static constexpr int MEAL_BUTTON_Y = 50;
    static constexpr int MEAL_BUTTON_W = 414;
    static constexpr int MEAL_BUTTON_H = 414;

    static constexpr int CHARA_SELECT_X = 200;
    static constexpr int CHARA_SELECT_Y = 500;
    static constexpr int CHARA_SELECT_BUTTON_W = 202;
    static constexpr int CHARA_SELECT_BUTTON_H = 63;
#pragma endregion

    bool showRestPanel;
    bool showSelectCharaPanel;
    bool pendingRefresh;
};
