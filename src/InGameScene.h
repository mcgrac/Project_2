#pragma once
#include "BaseScene.h"
#include "Party.h"
#include "WorldMap.h"
#include <vector>
#include <string>
#include "SaveLoad.h"


struct SDL_Texture;
class IslandScene;

class InGameScene : public BaseScene
{
public:
    InGameScene(std::vector<std::string> _characterNames, bool _isContinue);
    ~InGameScene();

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

    Party* GetAlliedParty() { return alliedParty; }

private:
    std::vector<std::string> characterNames;

    // InGameScene es owner de la party y los characters — los crea y destruye
    Party* alliedParty;
    void DestroyParty();

    WorldMap worldMap;

    //textures
    SDL_Texture* background;
    SDL_Texture* spritesheet;

    bool isContinue;
    void RestoreFromSave(const SaveData& data);
};
