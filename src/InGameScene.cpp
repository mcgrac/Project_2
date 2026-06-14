#include "InGameScene.h"
#include "CombatScene.h"
#include "islandScene.h"
#include "CharacterFactory.h"
#include "PauseScene.h"
#include "Character.h"
#include "Scene.h"
#include "Engine.h"
#include "Audio.h"
#include "Map.h"
#include "UIManager.h"
#include "Log.h"
#include "Textures.h"
#include "Render.h"
#include "SaveLoad.h"
#include "DialogueScene.h"
#include <queue>
#include "Window.h"
#include "PartyScene.h"
#include "MainMenuScene.h"
#include "ItemManager.h"
#include "EquippableItem.h"
#include "QuestManager.h"
#include "QuestScene.h"

// First button id reserved for combat button; island buttons start from this offset
static const int ISLAND_BUTTON_ID_OFFSET = 100;

InGameScene::InGameScene(std::vector<Character*> _prebuiltCharacters, WorldMap* _worldMap, bool _isContinue, int _loadedShiplevel)
    : prebuiltCharacters(_prebuiltCharacters)
    , worldMap(_worldMap)
    , alliedParty(nullptr)
    , background(nullptr)
    , isContinue(_isContinue)
    , ship(nullptr)
    , skullTex(nullptr)
    , teamButton(nullptr)
    , spritesheet(nullptr)
    , islandHumanTex(nullptr)
    , islandReptileTex(nullptr)
    , shipPanelTex(nullptr)
    , goldCounter(0, "Assets/Textures/Animations/coin.png", 1144, 62)
    , pendingStartIsland(false)
    , loadedShipLevel(_loadedShiplevel)
{
    sceneName = "InGameScene";
}

InGameScene::~InGameScene()
{
    DestroyParty();
}

void InGameScene::Load()
{
    // Construir la party aliada con los 3 personajes seleccionados
    alliedParty = new Party("Aliados");

    //initi quest manager
    QuestManager::GetInstance().Init(alliedParty);
       
    if (!prebuiltCharacters.empty())
    {
        // Venimos de LoadingScene — personajes ya creados con stats y visuals
        for (Character* c : prebuiltCharacters)
        {
            if (c != nullptr)
            {
                alliedParty->AddMember(c);
            }
        }
        prebuiltCharacters.clear();

       // Si venimos de un Continue, restaurar los recursos de la party
       // (oro, consumibles e items equipados en el inventario).
       // Los stats de los personajes ya están aplicados desde LoadingScene.
        if (isContinue)
        {
            SaveData data = SaveLoad::Load();
            if (data.exists)
            {
                alliedParty->AddGold(data.partyGold);
                alliedParty->GetInventory().AddItem("consumable", data.consumables);

                for (const auto& charSave : data.characters)
                {
                    for (const std::string& itemName : charSave.equippedItems)
                    {
                        Item* found = Engine::GetInstance().itemManager->GetItemByName(itemName);
                        if (found == nullptr)
                        {
                            LOG("InGameScene: item '%s' no encontrado.", itemName.c_str());
                            continue;
                        }

                        EquippableItem* eq = dynamic_cast<EquippableItem*>(found);
                        if (eq == nullptr)
                        {
                            LOG("InGameScene: item '%s' no es equippable.", itemName.c_str());
                            continue;
                        }

                        // Solo registrar en el slot del inventario — el efecto
                        // ya fue aplicado al personaje en LoadingScene
                        alliedParty->GetInventory().EquipItem(charSave.name, eq);
                    }
                }

                worldMap->SetCurrentIsland(data.currentIslandId);
                LOG("InGameScene: recursos restaurados — oro %d, isla %d.",
                    data.partyGold, data.currentIslandId);

                //load quest information
                QuestManager::GetInstance().LoadProgress(SaveLoad::SAVE_PATH);
            }
        }
    }

    LOG("InGameScene cargada, %d miembros en party.", alliedParty->GetMemberCount());

    //load textures
    LoadTextures();

    //callback when the player arrives to an island->world map notify ingameScene
    worldMap->arrivalIsland = [this](Island* island) {
        Engine::GetInstance().scene->PushScene(new IslandScene(island, worldMap, alliedParty, ship, this));
    };

    //ship
    ship = new Ship();
    if (isContinue && loadedShipLevel > 1)
    {
        int levelsToGain = loadedShipLevel - 1;
        for (int i = 0; i < levelsToGain; i++)
        {
            ship->LevelUp();
        }
        LOG("InGameScene: barco restaurado a nivel %d.", ship->GetLevel());
    }

    if (isContinue)
    {
        SaveData data = SaveLoad::Load();
        if (data.exists && data.shipCurrentHp > 0)
        {
            ship->SetHealth(data.shipCurrentHp);
            LOG("InGameScene: HP barco restaurado a %d/%d.", ship->GetCurrentHp(), ship->GetMaxHp());
        }
    }

    if (isContinue)
    {
        SaveData data = SaveLoad::Load();
        if (data.exists)
        {
            int savedIslandId = data.currentIslandId;
            worldMap->SetCurrentIsland(savedIslandId);

            IslandLayout layout = BuildIslandLayout(worldMap->GetTree());

            int col = layout.islandColumn[savedIslandId];
            int row = layout.islandRow[savedIslandId];
            int centerX = 224 + 448 * col;
            int centerY = GetIslandCenterY(row, layout.colCount[col]);

            Vector2D savedPos = Vector2D((float)(centerX + 125), (float)centerY);
            ship->SetPosition(savedPos);
            worldMap->SetIslandShipPosition(savedIslandId, savedPos);
        }
    }
    else
    {
        //set ship ay island 0
        Vector2D startPos = Vector2D(224.0f + 125.0f, 189.0f);
        ship->SetPosition(startPos);
        worldMap->SetIslandShipPosition(0, startPos);
    }

    //CreateUI();
    UpdateTutorialUI();
    LoadAudio();

    //play music
    Engine::GetInstance().audio->PlayMusic(mainMusic.c_str());
}

void InGameScene::Update(float dt)
{
    //check music
    if(!Engine::GetInstance().audio->IsMusicPlaying()) { Engine::GetInstance().audio->PlayMusic(mainMusic.c_str()); }

    // Si hay pantalla de fin de partida activa, solo dibujarla
    if (gameOverActive || gameWonActive)
    {
        Engine::GetInstance().render->camera.x = 0;
        DrawEndScreen();
        return;
    }

    if (pendingGameOver)
    {
        pendingGameOver = false;
        ShowEndScreen(false);
        return;
    }
    if (pendingGameWon)
    {
        pendingGameWon = false;
        ShowEndScreen(true);
        return;
    }

    if (firstFrame && !isContinue)
    {
        firstFrame = false;
        PushSceneFromInGame(new DialogueScene("intro_boss", [this]() {
            LOG("Intro terminada - lanzando llegada a isla 0");
            pendingStartIsland = true;  // flag nuevo, no llamar PushScene aquí directamente
            }));
        return;
    }

    // Procesar el flag en el mismo Update, fuera del firstFrame
    if (pendingStartIsland)
    {
        pendingStartIsland = false;
        Island* startIsland = worldMap->GetIslandById(0);
        if (startIsland != nullptr)
        {
            worldMap->SetCurrentIsland(0);
            Engine::GetInstance().scene->PushScene(
                new IslandScene(startIsland, worldMap, alliedParty, ship, this)
            );
        }
    }

    //detect pause menu
    if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_P) == KEY_DOWN)
    {
        Engine::GetInstance().audio->PlayFx(buttonPress);
        PushSceneFromInGame(new PauseScene(alliedParty, worldMap->GetCurrentIslandId(), ship->GetLevel(), ship->GetCurrentHp()));
        return;
    }

#if _DEBUG
    // Debug: F9 = game over, F10 = game won
    if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
    {
        ShowEndScreen(false);
        return;
    }
    if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
    {
        ShowEndScreen(true);
        return;
    }
#endif // _DEBUG



    worldMap->Update(dt);

    //render ship
    ship->Update(dt);

    //gold counter
    goldCounter.Update(alliedParty->GetGold(), dt);

}

void InGameScene::PostUpdate(float dt)
{
    if (gameOverActive || gameWonActive) { return; }

    //check if tutorial is open, if it is don't draw the scene and draw it instead.
    if (tutorialOpen) {
        Engine::GetInstance().render->camera.x = 0;
        int x = Engine::GetInstance().render->camera.x;
        int y = Engine::GetInstance().render->camera.y;
        Engine::GetInstance().render->DrawTexture(tutorials[tutorialIndex], x, y);
    }
    else {
        //render background
        Engine::GetInstance().render->DrawTexture(background, 0, 0);
        Engine::GetInstance().render->DrawTexture(goldBack, 1121, 30, nullptr, 0);
        Engine::GetInstance().render->DrawTexture(shipPanelTex, 1090, 590, nullptr, false);

        shipHpBar.Draw(ship->GetCurrentHp(), ship->GetMaxHp());
        ship->Draw(dt);
        goldCounter.Draw(dt);
        
    }

    worldMap->PostUpdate(dt, tutorialOpen);
}

void InGameScene::Unload()
{
    LOG("InGameScene::Unload LLAMADO — alliedParty: %p", alliedParty);

    Engine::GetInstance().textures->UnLoad(background);
    Engine::GetInstance().textures->UnLoad(spritesheet);
    Engine::GetInstance().textures->UnLoad(teamButton);
    Engine::GetInstance().textures->UnLoad(tutorialButton);
    Engine::GetInstance().textures->UnLoad(questButton);
    Engine::GetInstance().textures->UnLoad(islandHumanTex);
    Engine::GetInstance().textures->UnLoad(islandReptileTex);
    Engine::GetInstance().textures->UnLoad(skullTex);

    Engine::GetInstance().textures->UnLoad(gameOverTex);
    Engine::GetInstance().textures->UnLoad(gameWonTex);

    Engine::GetInstance().textures->UnLoad(emptyButton);

    //unload tutorial textures
    Engine::GetInstance().textures->UnLoad(tutorialOpenButton);
    Engine::GetInstance().textures->UnLoad(tutorialLeftButton);
    Engine::GetInstance().textures->UnLoad(tutorialRightButton);
    for (int i = 0; i < slidesNum; i++) { Engine::GetInstance().textures->UnLoad(tutorials[i]); }

    Engine::GetInstance().textures->UnLoad(shipPanelTex);
    shipHpBar.UnloadTexture();

    //unload worldMap
    worldMap->UnloadWorld();
    delete worldMap;
    worldMap = nullptr;

    DestroyParty();
    Engine::GetInstance().uiManager->CleanUp();

    //ship->CleanUp();
    delete ship;
    ship = nullptr;
}

void InGameScene::LoadTextures(){
    //load background
    background = Engine::GetInstance().textures->Load("Assets/Textures/MainMap/WorldMap.png");
    spritesheet = Engine::GetInstance().textures->Load("Assets/Textures/MainMap/EmptyIslandLabel.png");
    teamButton = Engine::GetInstance().textures->Load("Assets/Textures/MainMap/TeamButton.png");
    tutorialButton = Engine::GetInstance().textures->Load("Assets/Textures/MainMap/TutorialButton.png");
    questButton = Engine::GetInstance().textures->Load("Assets/Textures/MainMap/QuestButton.png");
    goldBack = Engine::GetInstance().textures->Load("Assets/Textures/MainMap/goldCount.png");

    humanButton = Engine::GetInstance().textures->Load("Assets/Textures/MainMap/OneButtonMap_Human.png");
    birdButton = Engine::GetInstance().textures->Load("Assets/Textures/MainMap/OneButtonMap_Bird.png");
    sirenButton = Engine::GetInstance().textures->Load("Assets/Textures/MainMap/OneButtonMap_Siren.png");
    reptileButton = Engine::GetInstance().textures->Load("Assets/Textures/MainMap/OneButtonMap_Reptile.png");

    jellyButton = Engine::GetInstance().textures->Load("Assets/Textures/MainMap/OneButtonMap_Jellifish.png");
    fishButton = Engine::GetInstance().textures->Load("Assets/Textures/MainMap/OneButtonMap_Fish.png");
    tribalButton = Engine::GetInstance().textures->Load("Assets/Textures/MainMap/OneButtonMap_Tribal.png");
    bossButton = Engine::GetInstance().textures->Load("Assets/Textures/MainMap/OneButtonMap_Boss.png");

    gameOverTex = Engine::GetInstance().textures->Load("Assets/Textures/MainMap/defeat.png");
    gameWonTex = Engine::GetInstance().textures->Load("Assets/Textures/MainMap/victory.png");
    emptyButton = Engine::GetInstance().textures->Load("Assets/Textures/MainMap/emptyButton.png");

    //tutorial textures
    tutorialOpenButton = Engine::GetInstance().textures->Load("Assets/Textures/MainMap/tutorialOpenButton.png");
    tutorialLeftButton = Engine::GetInstance().textures->Load("Assets/Textures/MainMap/left.png");
    tutorialRightButton = Engine::GetInstance().textures->Load("Assets/Textures/MainMap/right.png");
    //tutorial slides
    for (int i = 0; i < slidesNum; i++) {
        std::string path = "Assets/Textures/MainMap/tutorial";
        std::string index = std::to_string(i);
        tutorials.push_back(Engine::GetInstance().textures->Load((path + index + ".png").c_str()));
    }

    shipPanelTex = Engine::GetInstance().textures->Load("Assets/Textures/MainMap/BoatHealthBarEXP.png");
    shipHpBar.chunkW = 10;
    shipHpBar.chunkH = 12;
    shipHpBar.position = Vector2D(1140.0f, 608.0f); // ajusta al layout del panel
    shipHpBar.LoadTexture("Assets/Textures/CombatScene/HealthPoint.png");

}

void InGameScene::LoadAudio() {
    mainMusic = "Assets/Audio/Music/Map.wav";
    buttonPress = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/UIfx/button_press.wav");
}

bool InGameScene::OnUIMouseClickEvent(UIElement* uiElement)
{
    if (ship->IsMoving()) return true;

    switch (uiElement->id)
    {
    case 1:
        PushSceneFromInGame(new QuestScene());
        break;
    case 2:
        //Engine::GetInstance().scene->PushScene(new PartyScene(alliedParty));
        Engine::GetInstance().audio->PlayFx(buttonPress);
        PushSceneFromInGame(new PartyScene(alliedParty));
        break;
    case 3:
        //open tutorial
        tutorialOpen = !tutorialOpen;
        //update tutorial UI
        UpdateTutorialUI();
        break;
    case 4:
        //flip tutorial right
        if (tutorialIndex < slidesNum - 1) { tutorialIndex++; }
        break;
    case 5:
        //flip tutorial left
        if (tutorialIndex > 0) { tutorialIndex--; }
        break;


    case MAIN_MENU_BUTTON_ID:
        SaveLoad::ClearSave();
        Engine::GetInstance().render->camera.x = 0;
        Engine::GetInstance().scene->ReplaceScene(new MainMenuScene);
        break;
    default:
      
        if (uiElement->id >= ISLAND_BUTTON_ID_OFFSET)
        {
            int islandId = uiElement->id - ISLAND_BUTTON_ID_OFFSET;

            if (!worldMap->IsReachable(islandId))
            {
                break;
            }

            // Guardar posición actual del barco ANTES de moverse
            Vector2D posBeforeMove = ship->GetPosition();

            IslandLayout layout = BuildIslandLayout(worldMap->GetTree());

            int fromId = worldMap->GetCurrentIslandId();
            int fromCY = GetIslandCenterY(layout.islandRow[fromId], layout.colCount[layout.islandColumn[fromId]]);
            int toCY = GetIslandCenterY(layout.islandRow[islandId], layout.colCount[layout.islandColumn[islandId]]);
            int destCol = layout.islandColumn[islandId];

            ShipMovement movement = DetermineShipMovement(fromCY, toCY);

            ship->MoveToIsland(movement, [this, islandId, toCY, destCol, posBeforeMove]()
            {
                    int destCenterX = 224.0f + 448.0f * (float)destCol;
                    Vector2D destPos = Vector2D(destCenterX + 125.0f, (float)toCY);
                    ship->SetPosition(destPos);
                    worldMap->SetIslandShipPosition(islandId, destPos); // guardar posición

                    Engine::GetInstance().render->camera.x = 0;
                    // Guardar posición de origen en WorldMap para restaurarla si hay derrota
                    worldMap->SetShipReturnPosition(posBeforeMove);
                    worldMap->TravelTo(islandId);
            });
        }
        break;
    }
    return true;
}

void InGameScene::DestroyParty()
{
    if (alliedParty == nullptr) return;

    for (Character* c : alliedParty->GetMembers())
    {
        delete c;
    }

    delete alliedParty;
    alliedParty = nullptr;
}

void InGameScene::CreateIslandButtons()
{
    static const int BUTTON_W = 176;
    static const int BUTTON_H = 68;

    int screenW = 0;
    int screenH = 0;
    Engine::GetInstance().window->GetWindowSize(screenW, screenH);

    const auto& islands = worldMap->GetAllIslands();
    IslandLayout layout = BuildIslandLayout(worldMap->GetTree());

    // --- Create one button per island ---
    for (auto& pair : islands)
    {
        int islandId = pair.first;
        Island* island = pair.second;

        int col = layout.islandColumn[islandId];
        int row = layout.islandRow[islandId];
        int islandsInCol = layout.colCount[col];

        //--------Dynamic drawing------------
        //// Horizontal center of this column
        //int centerX = col * COL_SPACING + COL_SPACING / 2;

        //// Distribute vertically: divide the screen height into (islandsInCol + 1) slots
        //int slotH = screenH / (islandsInCol + 1);
        //int centerY = (row + 1) * slotH;

        //int btnX = centerX - BUTTON_W / 2;
        //int btnY = centerY - BUTTON_H / 2;
        //------------------------------------
        
        // --- POSICIÓN FIJA BASADA EN EL MAPA ---
        // X centrado en columna
        int centerX = 224 + (448 * col);

        // Y según número de islas en la columna
        int centerY = 0;
        if (islandsInCol == 1)
        {
            centerY = 189;
        }
        else if (islandsInCol == 2)
        {
            if (row == 0)
                centerY = 85;
            else
                centerY = 273;
        }

        // Convertir a top-left del botón
        int btnX = centerX - BUTTON_W / 2;
        int btnY = centerY - BUTTON_H / 2;

        //set position islands
        island->SetRenderPos((float)btnX, (float)btnY, (float)BUTTON_W, (float)BUTTON_H);

        SDL_Rect bounds = { btnX, btnY, BUTTON_W, BUTTON_H };

        // Label: faction name
        std::string label;
        IslandFaction faction = island->GetIslandFaction();

        int buttonId = ISLAND_BUTTON_ID_OFFSET + islandId;

        if (faction == IslandFaction::HUMANS)
        {
            Engine::GetInstance().uiManager->CreateUIElement(
                UIElementType::BUTTON, buttonId, label.c_str(), bounds,
                [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, humanButton, 0, bounds.w, bounds.h
            );
        }
        else if (faction == IslandFaction::REPTILES)
        {
            Engine::GetInstance().uiManager->CreateUIElement(
                UIElementType::BUTTON, buttonId, label.c_str(), bounds,
                [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, reptileButton, 0, bounds.w, bounds.h
            );
        }
        else if (faction == IslandFaction::BIRD)
        {
            Engine::GetInstance().uiManager->CreateUIElement(
                UIElementType::BUTTON, buttonId, label.c_str(), bounds,
                [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, birdButton, 0, bounds.w, bounds.h
            );
        }
        else if (faction == IslandFaction::SIRENS)
        {
            Engine::GetInstance().uiManager->CreateUIElement(
                UIElementType::BUTTON, buttonId, label.c_str(), bounds,
                [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, sirenButton, 0, bounds.w, bounds.h
            );
        }
        else if (faction == IslandFaction::JELLYFISH)
        {
            Engine::GetInstance().uiManager->CreateUIElement(
                UIElementType::BUTTON, buttonId, label.c_str(), bounds,
                [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, jellyButton, 0, bounds.w, bounds.h
            );
        }
        else if (faction == IslandFaction::FISH)
        {
           Engine::GetInstance().uiManager->CreateUIElement(
                UIElementType::BUTTON, buttonId, label.c_str(), bounds,
                [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, fishButton, 0, bounds.w, bounds.h
            );
        }
        else if (faction == IslandFaction::TRIBAL)
        {
            Engine::GetInstance().uiManager->CreateUIElement(
                UIElementType::BUTTON, buttonId, label.c_str(), bounds,
                [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, tribalButton, 0, bounds.w, bounds.h
            );
        }
        else if (faction == IslandFaction::BOSS)
        {
            Engine::GetInstance().uiManager->CreateUIElement(
                UIElementType::BUTTON, buttonId, label.c_str(), bounds,
                [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, bossButton, 0, bounds.w, bounds.h
            );
        }
        else
        {
            label = island->GetName();
        }

        

 

        LOG("InGameScene: island button created |id=%d label='%s' col=%d row=%d x=%d y=%d|",
            islandId, label.c_str(), col, row, btnX, btnY);
    }

}

int InGameScene::GetIslandCenterY(int row, int islandsInCol)
{
    if (islandsInCol == 1)
    {
        return 189;
    }
    // islandsInCol == 2
    if (row == 0)
    {
        return 85;
    }
    return 273;
}

ShipMovement InGameScene::DetermineShipMovement(int fromCenterY, int toCenterY)
{
    int delta = toCenterY - fromCenterY;

    if (delta == 0)
    {
        return ShipMovement::STRAIGHT;
    }
    else if (delta < 0 && delta > -150)
    {
        return ShipMovement::UP;
    }
    else if (delta > 0 && delta < 150)
    {
        return ShipMovement::DOWN;
    }
    else if (delta <= -150)
    {
        return ShipMovement::UP2;
    }
    // delta >= 150
    return ShipMovement::DOWN2;
}

void InGameScene::PushSceneFromInGame(BaseScene* scene)
{
    Engine::GetInstance().render->camera.x = 0;
    Engine::GetInstance().scene->PushScene(scene);
}

void InGameScene::ShowEndScreen(bool won)
{
    SaveLoad::ClearSave();
    Engine::GetInstance().render->camera.x = 0;  // resetear cámara

    if (won)
    {
        gameWonActive = true;
        Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/GameOverWin.wav");
    }
    else
    {
        gameOverActive = true;
        Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/GameOverLose.wav");
    }

    Engine::GetInstance().uiManager->CleanUp();

    //return main menu
    SDL_Rect btnBounds = {539, 509, 202, 63};
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, MAIN_MENU_BUTTON_ID, "",
        btnBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); },
        {}, emptyButton, 0, btnBounds.w, btnBounds.h
    );
}

void InGameScene::DrawEndScreen()
{
    if (gameWonActive)
    {
        Engine::GetInstance().render->DrawTexture(gameWonTex, 0, 0);
    }
    else if (gameOverActive)
    {
        Engine::GetInstance().render->DrawTexture(gameOverTex, 0, 0);
    }
}

InGameScene::IslandLayout InGameScene::BuildIslandLayout(const std::unordered_map<int, std::vector<int>>& tree)
{
    IslandLayout layout;

    std::queue<int> bfsQueue;
    bfsQueue.push(0);
    layout.islandColumn[0] = 0;

    while (!bfsQueue.empty())
    {
        int cur = bfsQueue.front();
        bfsQueue.pop();

        int col = layout.islandColumn[cur];
        layout.islandRow[cur] = layout.colCount[col];
        layout.colCount[col]++;

        auto it = tree.find(cur);
        if (it != tree.end())
        {
            for (int childId : it->second)
            {
                if (layout.islandColumn.find(childId) == layout.islandColumn.end())
                {
                    layout.islandColumn[childId] = col + 1;
                    bfsQueue.push(childId);
                }
            }
        }
    }

    return layout;
}



void InGameScene::OnResume()
{
    if (pendingGameOver || pendingGameWon) { return; }
    //CreateUI();
    UpdateTutorialUI();
}

void InGameScene::OnPause()
{
    Engine::GetInstance().render->camera.x = 0;
    Engine::GetInstance().uiManager->CleanUp();
}

void InGameScene::CreateUI()
{
    //Botón de party
    SDL_Rect partyBtnBounds = { 20, 600, 72, 72 };
    auto partyButon = Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 2, "", partyBtnBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, teamButton, 0, partyBtnBounds.w, partyBtnBounds.h
    );
    partyButon->isHUD = true; //fixed on screen

    //button to open tutorial
    SDL_Rect tutorialBtnBounds = { 180, 600, 72, 72 };
    auto tutorialButton = Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 3, "", tutorialBtnBounds, [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, tutorialOpenButton, 0, tutorialBtnBounds.w, tutorialBtnBounds.h
    );
    tutorialButton->isHUD = true;

    //button quests
    SDL_Rect questsBtnBounds = { 100, 600, 72, 72 };
    auto questsBtn = Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 1, "", questsBtnBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, questButton, 0, questsBtnBounds.w, questsBtnBounds.h
    );
    questsBtn->isHUD = true; //fixed on screen

    //island buttons
    if (tutorialOpen == false) {
        CreateIslandButtons();
    }
    
}

void InGameScene::UpdateTutorialUI() {

    Engine::GetInstance().uiManager->CleanUp();

    if (tutorialOpen == true) {

        // tutorial button close
        SDL_Rect tutorialBtnBounds = { 20, 600, 72, 72 };
        auto tutorialButton = Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON, 3, "", tutorialBtnBounds, [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, tutorialOpenButton, 0, tutorialBtnBounds.w, tutorialBtnBounds.h
        );
        tutorialButton->isHUD = true;

        //flip tutorial page right
        SDL_Rect rightBtnBounds = { 1209, 339, 42, 42 };
        auto rightButton = Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON, 4, "", rightBtnBounds, [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, tutorialRightButton, 0, rightBtnBounds.w, rightBtnBounds.h
        );
        rightButton->isHUD = true;

        //flip tutorail page left
        SDL_Rect leftBtnBounds = { 29, 339, 42, 42 };
        auto leftButton = Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON, 5, "", leftBtnBounds, [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, tutorialLeftButton, 0, leftBtnBounds.w, leftBtnBounds.h
        );
        leftButton->isHUD = true;
    }
    else {

        CreateUI();
    }
}
