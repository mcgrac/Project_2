#pragma once
#include "BaseScene.h"
#include "Hostel.h"
#include "Party.h"

class HostelScene : public BaseScene
{
public:
    HostelScene(Hostel hostel, Party* allied);
    ~HostelScene();

    void Load() override;
    void Update(float dt) override;
    void PostUpdate(float dt) override;
    void Unload() override;
    void LoadTextures() override;

    bool OnUIMouseClickEvent(UIElement* uiElement) override;

private:
    Hostel hostel;         
    Party* alliedParty;   

    static constexpr int BACK_BUTTON_ID = 1;

};
