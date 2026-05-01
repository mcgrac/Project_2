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
#include "DialogueManager.h"
#include "DialogueScene.h"
#include <queue>
#include "Window.h"
#include "PartyScene.h"

// First button id reserved for combat button; island buttons start from this offset
static const int ISLAND_BUTTON_ID_OFFSET = 100;

InGameScene::InGameScene(std::vector<std::string> _characterNames, bool _isContinue)
    : characterNames(_characterNames)
    , alliedParty(nullptr)
    , background(nullptr)
    , isContinue(_isContinue)
    , ship(nullptr)
    , skullTex(nullptr)
    , teamButton(nullptr)
    , spritesheet(nullptr)
{
    sceneName = "InGameScene";
}

InGameScene::~InGameScene()
{
    DestroyParty();
}

void InGameScene::Load()
{
    //load dialogues 
    DialogueManager::LoadDialogues("dialogues.xml");

    // Construir la party aliada con los 3 personajes seleccionados
    alliedParty = new Party("Aliados");
    for (const std::string& name : characterNames)
    {
        LOG("Creando personaje: '%s'", name.c_str());
        Character* c = CharacterFactory::Create(name);
        if (c != nullptr)
        {
            alliedParty->AddMember(c);
        }
        else
        {
            LOG("InGameScene::Load — no se pudo crear el personaje '%s'.", name.c_str());
        }
    }

    LOG("InGameScene cargada, %d miembros en party.", alliedParty->GetMemberCount());

#if _DEBUG
    for (Character* c : alliedParty->GetMembers())
    {
        c->PrintDebugInfo();
    }
#endif

    //load textures
    LoadTextures();

    //load world
    worldMap.LoadWorld("Assets/Maps/world.xml");

    //connect visuals 
    
    const auto& islands = worldMap.GetAllIslands();
    for (auto& pair : islands)
    {
        Island* island = pair.second;

        // Sprite según facción
        if (island->GetIslandFaction() == IslandFaction::HUMANS)
        {
            island->SetSprite(islandHumanTex);
        }
        else if (island->GetIslandFaction() == IslandFaction::REPTILES)
        {
            island->SetSprite(islandReptileTex);
        }

        // Todas tienen acceso a la calavera
        island->SetSkullSprite(skullTex);
    }

    //callback when the player arrives to an island->world map notify ingameScene
    worldMap.arrivalIsland = [this](Island* island) {
        Engine::GetInstance().scene->PushScene(new IslandScene(island, &worldMap, alliedParty, ship));
    };

    if (isContinue)
    {
        SaveData data = SaveLoad::Load();
        if (data.exists)
        {
            RestoreFromSave(data);
            worldMap.SetCurrentIsland(data.currentIslandId);
            LOG("InGameScene: partida restaurada — isla %d.", data.currentIslandId);
        }
    }

    //ship
    ship = new Ship();

    //set ship ay island 0
    int island0CenterX = 224;
    int islandOffsetX = 125;
    int island0CenterY = 189;
    ship->SetPosition(Vector2D((float)(island0CenterX + islandOffsetX), (float)(island0CenterY)));

    CreateUI();
}

void InGameScene::Update(float dt)
{
    // in first frame launch initial dialogue
    static bool firstFrame = true;

    if (firstFrame && !isContinue)
    {
        firstFrame = false;

        //Engine::GetInstance().scene->PushScene(
        //    new DialogueScene("intro_boss", [this]() {
        //        LOG("Intro terminada");
        //        //can de anything here
        //    })
        //);

        PushSceneFromInGame(new DialogueScene("intro_boss", [this]() {
            LOG("Intro terminada");
            }));
        return;
    }

    //render background
    Engine::GetInstance().render->DrawTexture(background, 0, 0);

    //detect pause menu
    if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_P) == KEY_DOWN)
    {
        //Engine::GetInstance().scene->PushScene(
        //    new PauseScene(alliedParty, worldMap.GetCurrentIslandId())
        //);

        PushSceneFromInGame(new PauseScene(alliedParty, worldMap.GetCurrentIslandId()));
        return;
    }

    worldMap.Update(dt);

    //render ship
    ship->Update(dt);

    //draw hp ship
    std::string text = "HP: " + std::to_string(ship->GetCurrentHp()) + "/" + std::to_string(ship->GetMaxHp());
    Engine::GetInstance().render->DrawText(text.c_str(), 600, 20, 0, 0, { 255,255,255,255 });
    
}

void InGameScene::PostUpdate(float dt)
{
    worldMap.PostUpdate(dt);
}

void InGameScene::Unload()
{
    LOG("InGameScene::Unload LLAMADO — alliedParty: %p", alliedParty);

    Engine::GetInstance().textures->UnLoad(background);
    Engine::GetInstance().textures->UnLoad(spritesheet);
    Engine::GetInstance().textures->UnLoad(teamButton);

    //unload worldMap
    worldMap.UnloadWorld();

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

    islandHumanTex = Engine::GetInstance().textures->Load("Assets/Textures/Islands/island_human.png");
    islandReptileTex = Engine::GetInstance().textures->Load("Assets/Textures/Islands/island_reptile.png");
    skullTex = Engine::GetInstance().textures->Load("Assets/Textures/MainMap/EnemySymbol.png");
}

bool InGameScene::OnUIMouseClickEvent(UIElement* uiElement)
{
    if (ship->IsMoving()) return true;

    switch (uiElement->id)
    {
    // Add buttons in game
    case 1:
        LOG("InGameScene: iniciando combate...");
        // PushScene — InGameScene queda suspendida con todo su estado
        //Engine::GetInstance().scene->PushScene(new CombatScene(alliedParty, ship->GetLevel()));
        PushSceneFromInGame(new CombatScene(alliedParty, ship->GetLevel()));
        break;
    case 2:
        //Engine::GetInstance().scene->PushScene(new PartyScene(alliedParty));
        PushSceneFromInGame(new PartyScene(alliedParty));
        break;
    default:
      
        //islands
        //if id is 100+
        //if (uiElement->id >= ISLAND_BUTTON_ID_OFFSET) {
        //    int islandId = uiElement->id - ISLAND_BUTTON_ID_OFFSET;
        //    worldMap.TravelTo(islandId);
        //}

        if (uiElement->id >= ISLAND_BUTTON_ID_OFFSET)
        {
            int islandId = uiElement->id - ISLAND_BUTTON_ID_OFFSET;

            if (!worldMap.IsReachable(islandId))
            {
                break;
            }

            // --- Determine which movement the ship must perform ---
            // We need the BFS row/colCount data that CreateIslandButtons already computed.
            // Re-derive it here with the same logic so we can read centerY values.

            const auto& treeRef = worldMap.GetTree();
            std::unordered_map<int, int> islandColumn;
            std::unordered_map<int, int> islandRow;
            std::unordered_map<int, int> colCount;

            std::queue<int> bfsQueue;
            bfsQueue.push(0);
            islandColumn[0] = 0;

            while (!bfsQueue.empty())
            {
                int cur = bfsQueue.front();
                bfsQueue.pop();

                int col = islandColumn[cur];
                islandRow[cur] = colCount[col];
                colCount[col]++;

                auto it = treeRef.find(cur);
                if (it != treeRef.end())
                {
                    for (int childId : it->second)
                    {
                        if (islandColumn.find(childId) == islandColumn.end())
                        {
                            islandColumn[childId] = col + 1;
                            bfsQueue.push(childId);
                        }
                    }
                }
            }

            int fromId = worldMap.GetCurrentIslandId();
            int fromCY = GetIslandCenterY(islandRow[fromId], colCount[islandColumn[fromId]]);
            int toCY = GetIslandCenterY(islandRow[islandId], colCount[islandColumn[islandId]]);
            int destCol = islandColumn[islandId];

            ShipMovement movement = DetermineShipMovement(fromCY, toCY);

            ship->MoveToIsland(movement, [this, islandId, toCY, destCol]()
                {
                    // Reposition ship to the right offset of the destination island
                    // so the next departure starts correctly
                    int destCenterX = 224.0f + 448.0f * (float)destCol;
                    ship->SetPosition(Vector2D(destCenterX + 125.0f, (float)toCY));

                    Engine::GetInstance().render->camera.x = 0;
                    worldMap.TravelTo(islandId);
                    //worldMap.TravelTo(islandId);
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

void InGameScene::RestoreFromSave(const SaveData& data)
{
    for (const auto& charSave : data.characters)
    {
        for (Character* c : alliedParty->GetMembers())
        {
            if (c->GetName() == charSave.name)
            {
                c->RestorePreCombatValues({ charSave.health, charSave.isAlive });
                LOG("SaveLoad: restaurado %s — HP:%d isAlive:%d",
                    charSave.name.c_str(), charSave.health, charSave.isAlive);
                break;
            }
        }
    }
}

void InGameScene::CreateIslandButtons()
{
    static const int BUTTON_W = 176;
    static const int BUTTON_H = 68;

    int screenW = 0;
    int screenH = 0;
    Engine::GetInstance().window->GetWindowSize(screenW, screenH);

    const auto& islands = worldMap.GetAllIslands();
    const auto& tree = worldMap.GetTree();

    // BFS to assign a column index to every island
    // Column 0 = starting island, column N = N hops from the root
    std::unordered_map<int, int> islandColumn; // islandId -> column
    std::unordered_map<int, int> islandRow;    // islandId -> row within its column

    // Count how many islands fall in each column so we can distribute rows
    std::unordered_map<int, int> colCount;     // column -> number of islands so far

    std::queue<int> bfsQueue;
    bfsQueue.push(0);
    islandColumn[0] = 0;

    while (!bfsQueue.empty())
    {
        int currentId = bfsQueue.front();
        bfsQueue.pop();

        int col = islandColumn[currentId];

        // Assign row within this column
        int row = colCount[col];
        islandRow[currentId] = row;
        colCount[col]++;

        auto it = tree.find(currentId);
        if (it != tree.end())
        {
            for (int childId : it->second)
            {
                if (islandColumn.find(childId) == islandColumn.end())
                {
                    islandColumn[childId] = col + 1;
                    bfsQueue.push(childId);
                }
            }
        }
    }

    if (islandColumn.empty())
    {
        return;
    }

    // --- Create one button per island ---
    for (auto& pair : islands)
    {
        int     islandId = pair.first;
        Island* island = pair.second;

        int col = islandColumn[islandId];
        int row = islandRow[islandId];
        int islandsInCol = colCount[col];

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
        if (faction == IslandFaction::HUMANS)
        {
            label = "Humans";
        }
        else if (faction == IslandFaction::REPTILES)
        {
            label = "Reptiles";
        }
        else if (faction == IslandFaction::BIRD)
        {
            label = "Bird";
        }
        else if (faction == IslandFaction::SIRENS)
        {
            label = "Sirens";
        }
        else if (faction == IslandFaction::JELLYFISH)
        {
            label = "Jellyfish";
        }
        else if (faction == IslandFaction::FISH)
        {
            label = "Fish";
        }
        else if (faction == IslandFaction::TRIBAL)
        {
            label = "Tribal";
        }
        else
        {
            label = island->GetName();
        }

        int buttonId = ISLAND_BUTTON_ID_OFFSET + islandId;

        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON, buttonId, label.c_str(), bounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, spritesheet, 0, bounds.w, bounds.h
        );

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

void InGameScene::OnResume()
{
    CreateUI();
}

void InGameScene::OnPause()
{
    Engine::GetInstance().render->camera.x = 0;
    Engine::GetInstance().uiManager->CleanUp();
}

void InGameScene::CreateUI()
{
    //Botón de iniciar combate
    SDL_Rect combatBtnBounds = { 20, 20, 154, 60 };
    auto CombatBtn = Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 1, "Start Combat", combatBtnBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, spritesheet, 0, combatBtnBounds.w, combatBtnBounds.h
    );
    CombatBtn->isHUD = true;//fixed on screen

    //Botón de party
    SDL_Rect partyBtnBounds = { 20, 600, 72, 72 };
    auto partyButon = Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 2, "Party", partyBtnBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, teamButton, 0, partyBtnBounds.w, partyBtnBounds.h
    );
    partyButon->isHUD = true; //fixed on screen

    //island buttons
    CreateIslandButtons();
}
