#pragma once
#include "BaseScene.h"
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include "WorldMap.h"
#include "SaveLoad.h"

class Character;
struct SDL_Texture;

class LoadingScene : public BaseScene
{
public:
    LoadingScene(std::vector<std::string> _characterNames, bool _isContinue);

    void Load() override;
    void Update(float dt) override;
    void PostUpdate(float dt) override;
    void Unload() override;
    void LoadTextures() override;
    bool OnUIMouseClickEvent(UIElement* uiElement) override;
    void ApplySaveDataToCharacters();

private:
    std::vector<std::string> characterNames;
    bool isContinue;

    std::vector<Character*> loadedCharacters;
    std::thread loadThread;
    std::atomic<bool> loadingDone;

    SDL_Texture* background;

    void DoBackgroundLoad();
    void UnloadTextures();

    int framesRendered;
    float elapsedTime;

    //animation
    float dotTimer;
    int dotCount;

    WorldMap* loadedWorldMap;
    SaveData loadedSaveData;
};