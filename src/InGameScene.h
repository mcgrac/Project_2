#pragma once
#include "BaseScene.h"
#include "Party.h"
#include "WorldMap.h"
#include <vector>
#include <string>
#include "SaveLoad.h"
#include "Ship.h"
#include "SceneUtils.h"


struct SDL_Texture;
class IslandScene;
class Character;

class InGameScene : public BaseScene
{
public:
    InGameScene(std::vector<Character*> _prebuiltCharacters, WorldMap* _worldMap, bool _isContinue);
    ~InGameScene();

    void Load() override;
    void Update(float dt) override;
    void PostUpdate(float dt) override;
    void Unload() override;
    void LoadTextures() override;
    void LoadAudio();

    bool OnUIMouseClickEvent(UIElement* uiElement) override;

    //helpers
    void OnResume() override;
    void OnPause() override;
    void CreateUI();

    Party* GetAlliedParty() { return alliedParty; }

    void SetPendingGameOver(bool b) { pendingGameOver = b; }
    void SetPendingGameWon(bool b) { pendingGameWon = b; }

private:
    std::vector<Character*> prebuiltCharacters;

    //gold counter
    GoldCounter goldCounter;

    // InGameScene es owner de la party y los characters — los crea y destruye
    Party* alliedParty;
    void DestroyParty();

    WorldMap* worldMap;

#pragma region TEXTURES
    //textures
    SDL_Texture* background;
    SDL_Texture* spritesheet;
    SDL_Texture* teamButton;
    SDL_Texture* tutorialButton;
    SDL_Texture* questButton;
    SDL_Texture* goldBack;

    SDL_Texture* humanButton;
    SDL_Texture* birdButton;
    SDL_Texture* sirenButton;
    SDL_Texture* reptileButton;

    SDL_Texture* jellyButton;
    SDL_Texture* fishButton;
    SDL_Texture* bossButton;
    SDL_Texture* tribalButton;
    SDL_Texture* emptyButton;

    SDL_Texture* shipPanelTex;

    // Island sprites — drawn below each island button
    SDL_Texture* islandHumanTex;
    SDL_Texture* islandReptileTex;
    SDL_Texture* skullTex;

    //tutorial button textures
    SDL_Texture* tutorialOpenButton; //open/close tutorial
    SDL_Texture* tutorialLeftButton; //buttons to change page of the tutorial
    SDL_Texture* tutorialRightButton;

    std::vector<SDL_Texture*> tutorials; //vector withe the tutorial slides' textures
#pragma endregion


    int tutorialIndex = 0; 
    int slidesNum = 7;
    bool tutorialOpen = true; //checks if tutorial is open or not

    void UpdateTutorialUI();

    DynamicBar shipHpBar;

    bool isContinue;

    void CreateIslandButtons();   // builds one button per island using screen-space layout

    //ship
    Ship* ship;

    static int GetIslandCenterY(int row, int islandsInCol);
    static ShipMovement DetermineShipMovement(int fromCenterY, int toCenterY);
    void PushSceneFromInGame(BaseScene* scene);

    //audio variables
    std::string mainMusic;
    int buttonPress;
    int startCombat;
    bool pendingStartIsland;
    bool isMusicPlayed = false;
    bool firstFrame = true;
    bool pendingGameOver = false;
    bool pendingGameWon = false;

    // Game over / victory screen
    bool gameOverActive = false;
    bool gameWonActive = false;
    SDL_Texture* gameOverTex = nullptr;
    SDL_Texture* gameWonTex = nullptr;
    static constexpr int MAIN_MENU_BUTTON_ID = 10;

    void ShowEndScreen(bool won);
    void DrawEndScreen();

    //BFS
    struct IslandLayout
    {
        std::unordered_map<int, int> islandColumn;
        std::unordered_map<int, int> islandRow;
        std::unordered_map<int, int> colCount;
    };
    static IslandLayout BuildIslandLayout(const std::unordered_map<int, std::vector<int>>& tree);
};