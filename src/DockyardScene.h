#pragma once
#include "BaseScene.h"
#include "Dockyard.h"
#include "Party.h"

class DockyardScene : public BaseScene
{
public:
    DockyardScene(Dockyard dockyard, Party* allied);
    ~DockyardScene();

    void Load() override;
    void Update(float dt) override;
    void PostUpdate(float dt) override;
    void Unload() override;
    void LoadTextures() override;

    bool OnUIMouseClickEvent(UIElement* uiElement) override;

private:
    Dockyard dockyard; 
    Party* alliedParty;  

    static constexpr int BACK_BUTTON_ID = 1;

};
