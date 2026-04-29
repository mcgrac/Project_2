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

    SDL_Texture* exitButton;
    SDL_Texture* background;
    SDL_Texture* ownerSprite;

    bool showRestPanel;
    bool showSelectCharaPanel;

    bool pendingRefresh;
};
