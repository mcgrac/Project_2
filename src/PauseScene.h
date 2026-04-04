#pragma once
#include "BaseScene.h"
#include "Party.h"

class PauseScene : public BaseScene
{
public:
    PauseScene(Party* allied, int currentIslandId);
    ~PauseScene();

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
    Party* alliedParty;  
    int currentIslandId;

    static constexpr int CONTINUE_BUTTON_ID = 1;
    static constexpr int OPTIONS_BUTTON_ID = 2;
    static constexpr int MAIN_MENU_BUTTON_ID = 3;
};
