#pragma once
#include "BaseScene.h"
#include "Island.h"
#include "Party.h"


class IslandInteriorScene : public BaseScene
{
public:

    IslandInteriorScene(Island* island, Party* allied);
    ~IslandInteriorScene();

    void Load() override;
    void Update(float dt) override;
    void PostUpdate(float dt) override;
    void Unload() override;
    void LoadTextures() override;

    bool OnUIMouseClickEvent(UIElement* uiElement) override;

private:
    Island* island;
    Party* alliedParty;

    static constexpr int SHOP_BUTTON_ID = 1;
    static constexpr int HOSTEL_BUTTON_ID = 2;
    static constexpr int DOCKYARD_BUTTON_ID = 3;
    static constexpr int LEAVE_BUTTON_ID = 4;
};
