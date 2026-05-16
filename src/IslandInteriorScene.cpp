#include "IslandInteriorScene.h"
#include "ShopScene.h"
#include "HostelScene.h"
#include "DockyardScene.h"
#include "Engine.h"
#include "Audio.h"
#include "Textures.h"
#include "Scene.h"
#include "UIManager.h"
#include "Render.h"
#include "Log.h"
#include "Ship.h"
#include <algorithm>
#include <random>
#include "ItemManager.h"
#include "Party.h"
#include "Character.h"

#pragma region POSITIONS
#pragma region DOCK
const SDL_Rect IslandInteriorScene::DOCK_HUMAN_BOUNDS = { 780,  0,  507,  720 };
const SDL_Rect IslandInteriorScene::DOCK_BIRD_BOUNDS = { 985,  608,  138,  108 };
const SDL_Rect IslandInteriorScene::DOCK_SIREN_BOUNDS = { 0,  249, 265, 471 };
const SDL_Rect IslandInteriorScene::DOCK_REPTILE_BOUNDS = { 653, 17, 510, 514 };
#pragma endregion

#pragma region SHOP
const SDL_Rect IslandInteriorScene::SHOP_HUMAN_BOUNDS = { 20,  470,  270,  204 };
const SDL_Rect IslandInteriorScene::SHOP_BIRD_BOUNDS = { 524,  392,  88,  64 };
const SDL_Rect IslandInteriorScene::SHOP_SIREN_BOUNDS = { 903,  312, 292, 265 };
const SDL_Rect IslandInteriorScene::SHOP_REPTILE_BOUNDS = { 297, 170, 201, 204 };
#pragma endregion

#pragma region HOSTEL
const SDL_Rect IslandInteriorScene::HOSTEL_HUMAN_BOUNDS = { 206,  55,  492,  435 };
const SDL_Rect IslandInteriorScene::HOSTEL_BIRD_BOUNDS = { 574,  8,  144,  150 };
const SDL_Rect IslandInteriorScene::HOSTEL_SIREN_BOUNDS = { 516,  340, 174, 249 };
const SDL_Rect IslandInteriorScene::HOSTEL_REPTILE_BOUNDS = { 373, 301, 214, 230 };
#pragma endregion

#pragma region CHEST
const SDL_Rect IslandInteriorScene::CHEST_HUMAN_BOUNDS = { 784,  154,  93,  90 };
const SDL_Rect IslandInteriorScene::CHEST_BIRD_BOUNDS = { 112,  412,  102,  142 };
const SDL_Rect IslandInteriorScene::CHEST_SIREN_BOUNDS = { 228,  468,  102,  117 };
const SDL_Rect IslandInteriorScene::CHEST_REPTILE_BOUNDS = { 626,  486,  54,  42 };

const SDL_Rect IslandInteriorScene::CHEST_ITEM_POSITIONS = { 549, 507, 182, 72 };
#pragma endregion
#pragma endregion

IslandInteriorScene::IslandInteriorScene(Island* island, Party* allied, Ship* _ship)
    : island(island)
    , alliedParty(allied)
    , ship(_ship)
    , background(nullptr)
    , dockyardbutton(nullptr)
    , shopButton(nullptr)
    , hostelButton(nullptr)
    , chestButton(nullptr)
    , exitButton(nullptr)
    , showChest(false)
    , goldCounter(0, "Assets/Textures/Animations/coin.png", 1160, 62)
    , chestSpritesheet(nullptr)
    , chestOpened(false)
    , chestPopped(false)
{
    sceneName = "islandInterior";
}

IslandInteriorScene::~IslandInteriorScene() {}


void IslandInteriorScene::Load()
{
    LoadTextures();
    LoadAnimation();
    LoadSound();
    CreateUI();
}

void IslandInteriorScene::Update(float dt) {
   
    //check if music is playing
    if(isMusicChanged==false){ Engine::GetInstance().audio->PlayMusic(("Assets/Audio/Music/Island" + SceneUtils::GetFactionString(island->GetIslandFaction()) + ".wav").c_str()); }
   
    isMusicChanged = true;
    if (!Engine::GetInstance().audio->IsMusicPlaying()) {
        LOG("Play music again!");
        //Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/MainMenuScene.wav");
        Engine::GetInstance().audio->PlayMusic(("Assets/Audio/Music/Island" + SceneUtils::GetFactionString(island->GetIslandFaction()) + ".wav").c_str());
    }
 
    Draw(dt);

    UpdateSound();
    
    Engine::GetInstance().render->DrawTexture(moneyCounter, 1121, 30, nullptr, 0);
    goldCounter.Update(alliedParty->GetGold(), dt);

}

void IslandInteriorScene::PostUpdate(float dt)
{
    // Nombre de la isla
   /* Engine::GetInstance().render->DrawText(
        island->GetName().c_str(),
        440, 180, 400, 50,
        { 255, 255, 255, 255 }
    );*/
}

void IslandInteriorScene::Unload()
{
    Engine::GetInstance().textures->UnLoad(background);
    Engine::GetInstance().textures->UnLoad(dockyardbutton);
    Engine::GetInstance().textures->UnLoad(shopButton);
    Engine::GetInstance().textures->UnLoad(hostelButton);
    Engine::GetInstance().textures->UnLoad(chestButton);
    Engine::GetInstance().textures->UnLoad(exitButton);
    Engine::GetInstance().textures->UnLoad(chestBackground);

    Engine::GetInstance().uiManager->CleanUp();

    if (chestItemTexture != nullptr)
    {
        Engine::GetInstance().textures->UnLoad(chestItemTexture);
        chestItemTexture = nullptr;
    }
    Engine::GetInstance().textures->UnLoad(claimButton);
    Engine::GetInstance().textures->UnLoad(emptyButtons);
}

void IslandInteriorScene::unloadSound() {
    
}

void IslandInteriorScene::LoadTextures()
{
    exitButton = Engine::GetInstance().textures->Load("Assets/Textures/HumanIsland/BackButton.png");

    std::string backgroundPath = SceneUtils::GetIslandTexturePath( island->GetIslandFaction());
    background = Engine::GetInstance().textures->Load(backgroundPath.c_str());

    std::string dockyardPath = SceneUtils::GetBuildingTexturePath(island->GetIslandFaction(), "Dockyard");
    dockyardbutton = Engine::GetInstance().textures->Load(dockyardPath.c_str());

    std::string shopPath = SceneUtils::GetBuildingTexturePath(island->GetIslandFaction(), "Shop");
    shopButton = Engine::GetInstance().textures->Load(shopPath.c_str());

    std::string hostelPath = SceneUtils::GetBuildingTexturePath(island->GetIslandFaction(), "Hostel");
    hostelButton = Engine::GetInstance().textures->Load(hostelPath.c_str());

    chestButton = Engine::GetInstance().textures->Load(("Assets/Textures/ShopScene/chest" + SceneUtils::GetFactionString(island->GetIslandFaction()) + ".png").c_str());
    moneyCounter = Engine::GetInstance().textures->Load("Assets/Textures/ShopScene/moneyCounter.png");
    if(island->GetIslandFaction()==IslandFaction::BIRD){ backgroundSpritesheet = Engine::GetInstance().textures->Load(("Assets/Textures/Animations/BirdAnimatedBack.png")); }
    else if (island->GetIslandFaction() == IslandFaction::SIRENS) { backgroundSpritesheet = Engine::GetInstance().textures->Load(("Assets/Textures/Animations/SirenAnimatedBack.png")); }
    
    chestSpritesheet = Engine::GetInstance().textures->Load("Assets/Textures/Animations/chestAnimated.png");
    claimButton = Engine::GetInstance().textures->Load("Assets/Textures/ShopScene/claimButton.png");
    emptyButtons = Engine::GetInstance().textures->Load("Assets/Textures/ShopScene/EmptyTextButton.png");
    chestBackground = Engine::GetInstance().textures->Load("Assets/Textures/ShopScene/chestBackground.png");
    keyCounter = Engine::GetInstance().textures->Load("Assets/Textures/ShopScene/keyCounter.png");
    emptyCard = Engine::GetInstance().textures->Load("Assets/Textures/ShopScene/emptyCard.png");
    moneyCard = Engine::GetInstance().textures->Load("Assets/Textures/ShopScene/moneyCard.png");
}

void IslandInteriorScene::Draw(float dt)
{
    IslandFaction faction = island->GetIslandFaction();

    if (chestOpened || chestPopped || showChest)
    {
        Engine::GetInstance().render->DrawTexture(chestBackground, 0, 0);
    }
    else if (faction == IslandFaction::SIRENS || faction == IslandFaction::BIRD) {

        anims.Update(dt);

        const SDL_Rect& animFrame = anims.GetCurrentFrame();

        Engine::GetInstance().render->DrawTexture(
            backgroundSpritesheet,
            0,
            0,
            &animFrame
        );
    }
    else {

        Engine::GetInstance().render->DrawTexture(background, 0, 0);
    }

    DrawChest(dt);
}

void IslandInteriorScene::DrawChest(float dt)
{
    IslandFaction faction = island->GetIslandFaction();


    if (faction == IslandFaction::SIRENS || faction == IslandFaction::REPTILES) {

        Engine::GetInstance().render->DrawTexture(moneyCounter, 990, 70);
        Engine::GetInstance().render->DrawTexture(keyCounter, 990, 147);

        //chest animation
        if (chestOpened) {
            //play animation chest
            animsChest.Update(dt);

            const SDL_Rect& animFrame = animsChest.GetCurrentFrame();

            Engine::GetInstance().render->DrawTexture(
                chestSpritesheet,
                0,
                0,
                &animFrame
            );

            if (animsChest.IsCurrentFinished()) {
                chestOpened = false;
                chestPopped = true;
                GiveReward();
            };
        }

        //chest render items
        if (chestPopped)
        {
            if (rewardAmount > 40)
            {
                // nada, emptyCard
            }
            else if (rewardAmount < 21)
            {
                // moneyCard — si tienes esa textura aquí también
            }
            else
            {
                if (chestItemTexture != nullptr)
                {
                    float texW, texH;
                    SDL_GetTextureSize(chestItemTexture, &texW, &texH);

                    SDL_Rect firstFrame;
                    firstFrame.x = 0;
                    firstFrame.y = 0;
                    firstFrame.w = (int)texW;
                    firstFrame.h = (int)texH / 3;

                    Engine::GetInstance().render->DrawTexture(chestItemTexture, 526, 185, &firstFrame);
                }
            }
        }
    }
}


bool IslandInteriorScene::OnUIMouseClickEvent(UIElement* uiElement)
{
    //play button Press when pressing a button
    Engine::GetInstance().audio->PlayFx(buttonPress);
    switch (uiElement->id)
    {
    case SHOP_BUTTON_ID:
        LOG("IslandInteriorScene: abriendo tienda.");
        Engine::GetInstance().scene->PushScene(
            new ShopScene(island->GetShop(), alliedParty)
        );
        break;

    case HOSTEL_BUTTON_ID:
        LOG("IslandInteriorScene: abriendo hostel.");
        Engine::GetInstance().scene->PushScene(
            new HostelScene(island->GetHostel(), alliedParty)
        );
        break;

    case DOCKYARD_BUTTON_ID:
        LOG("IslandInteriorScene: abriendo astillero.");
        island->GetDockyard()->AssignShip(ship);
        Engine::GetInstance().scene->PushScene(
            new DockyardScene(island->GetDockyard(), alliedParty)
        );
        break;

    case LEAVE_BUTTON_ID:
        // Volver al mapa (IslandScene → InGameScene)
        Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/Map.wav");
        Engine::GetInstance().scene->PopScene();
        break;

    case CHEST_BUTTON_ID :
        //open chest
        Engine::GetInstance().uiManager->RemoveElementsByRange(0, 10);
        OpenUIChest();
        break;

    case OPEN_BUTTON_ID:
        if (chestPopped) { break; }
        if (alliedParty->GetInventory().GetItemCount("key") < 1) { break; }
        alliedParty->GetInventory().ConsumeItem("key");
        chestOpened = true;
        animsChest.SetCurrent("idle");
        animsChest.SetLoop("idle", false);
        break;

    case CLOSE_CHEST_BUTTON_ID:
        //close chest
        if (chestPopped) { break; }
        if (selectedItem != nullptr && !selectedItem->IsPurchased())
        {
            delete selectedItem;
            selectedItem = nullptr;
        }
        if (chestItemTexture != nullptr)
        {
            Engine::GetInstance().textures->UnLoad(chestItemTexture);
            chestItemTexture = nullptr;
        }
        Engine::GetInstance().uiManager->RemoveElementsByRange(0, 300);
        chestOpened = false;
        chestPopped = false;
        showChest = false;
        CreateUI();
        break;

    case REWARD_GOLD:
        Engine::GetInstance().audio->PlayFx(buttonPress);
        alliedParty->AddGold(rewardAmount);
        rewardAmount = 0;
        chestPopped = false;
        Engine::GetInstance().uiManager->RemoveElementsByRange(10, 11);
        break;

    case REWARD_EMPTY:
        Engine::GetInstance().audio->PlayFx(buttonPress);
        chestPopped = false;
        Engine::GetInstance().uiManager->RemoveElementsByRange(10, 11);
        break;

    case 200:
    case 201:
    case 202:
    {
        int index = uiElement->id - CHARACTERS_AVAILABLE_BASE;
        Character* character = alliedParty->GetMembers()[index];

        EquippableItem* equippable = dynamic_cast<EquippableItem*>(selectedItem);
        if (!equippable) { return false; }

        if (alliedParty->GetInventory().EquipItem(character->GetName(), equippable))
        {
            selectedItem->SetPurchased(true);
            chestPopped = false;
            rewardAmount = 0;

            if (chestItemTexture != nullptr)
            {
                Engine::GetInstance().textures->UnLoad(chestItemTexture);
                chestItemTexture = nullptr;
            }

            LOG("Item equipado desde chest IslandInterior");
        }
        else
        {
            alliedParty->AddGold(10);
            chestPopped = false;
            rewardAmount = 0;
            LOG("No hay espacio, ganar 10 gold");
        }

        Engine::GetInstance().uiManager->RemoveElementsByRange(CHARACTERS_AVAILABLE_BASE, CHARACTERS_AVAILABLE_BASE + 99);
        break;
    }
    default:
        break;
    }
    return true;
}

void IslandInteriorScene::OnResume()
{
    CreateUI();
}

void IslandInteriorScene::OnPause()
{
    Engine::GetInstance().uiManager->CleanUp();
}

void IslandInteriorScene::CreateUI()
{
    LOG("IslandInteriorScene: entrando en '%s'.", island->GetName().c_str());

    SDL_Rect shopBounds = GetShopBounds();
    SDL_Rect hostelBounds = GetHostelBounds();
    SDL_Rect dockyardBounds = GetDockBounds();
    SDL_Rect chestBounds = GetChestBounds();
    SDL_Rect leaveBounds = { 20, 20, 72, 72 };

    IslandFaction faction = island->GetIslandFaction();
    if (faction == IslandFaction::SIRENS || faction == IslandFaction::REPTILES) {
        Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, CHEST_BUTTON_ID, "", chestBounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, chestButton, 0, chestBounds.w, chestBounds.h
            );
    }

    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, SHOP_BUTTON_ID, "", shopBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, shopButton, 0, shopBounds.w, shopBounds.h
    );

    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, HOSTEL_BUTTON_ID, "", hostelBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, hostelButton, 0, hostelBounds.w, hostelBounds.h
    );

    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, DOCKYARD_BUTTON_ID, "", dockyardBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, dockyardbutton, 0, dockyardBounds.w, dockyardBounds.h
    );

    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, LEAVE_BUTTON_ID, "", leaveBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, exitButton, 0, leaveBounds.w, leaveBounds.h
    );
}

void IslandInteriorScene::LoadAnimation()
{
    // load animation backgrounds
    if(island->GetIslandFaction()==IslandFaction::BIRD){
        std::unordered_map<int, std::string> aliases = { {0,"idle"} };
        anims.LoadFromTSX("Assets/Textures/Animations/BirdAnimatedBack.tsx", aliases);
        anims.SetCurrent("idle");
    }
    else if (island->GetIslandFaction() == IslandFaction::SIRENS) {
        std::unordered_map<int, std::string> aliases = { {0,"idle"} };
        anims.LoadFromTSX("Assets/Textures/Animations/SirenAnimatedBack.tsx", aliases);
        anims.SetCurrent("idle");
    }

    //chest
    if (island->GetIslandFaction() == IslandFaction::REPTILES || island->GetIslandFaction() == IslandFaction::SIRENS) {

        std::unordered_map<int, std::string> aliases = { {0,"idle"} };
        animsChest.LoadFromTSX("Assets/Textures/Animations/chestAnimated.tsx", aliases);
        animsChest.SetLoop("idle", false);
    }
}

//helpers
SDL_Rect IslandInteriorScene::GetDockBounds() const
{
    switch (island->GetIslandFaction())
    {
    case IslandFaction::HUMANS:   return DOCK_HUMAN_BOUNDS;
    case IslandFaction::BIRD:     return DOCK_BIRD_BOUNDS;
    case IslandFaction::SIRENS:   return DOCK_SIREN_BOUNDS;
    case IslandFaction::REPTILES: return DOCK_REPTILE_BOUNDS;
    default:                      return DOCK_HUMAN_BOUNDS;
    }
}

SDL_Rect IslandInteriorScene::GetShopBounds() const
{
    switch (island->GetIslandFaction())
    {
    case IslandFaction::HUMANS:   return SHOP_HUMAN_BOUNDS;
    case IslandFaction::BIRD:     return SHOP_BIRD_BOUNDS;
    case IslandFaction::SIRENS:   return SHOP_SIREN_BOUNDS;
    case IslandFaction::REPTILES: return SHOP_REPTILE_BOUNDS;
    default:                      return SHOP_HUMAN_BOUNDS;
    }
}

SDL_Rect IslandInteriorScene::GetHostelBounds() const
{
    switch (island->GetIslandFaction())
    {
    case IslandFaction::HUMANS:   return HOSTEL_HUMAN_BOUNDS;
    case IslandFaction::BIRD:     return HOSTEL_BIRD_BOUNDS;
    case IslandFaction::SIRENS:   return HOSTEL_SIREN_BOUNDS;
    case IslandFaction::REPTILES: return HOSTEL_REPTILE_BOUNDS;
    default:                      return HOSTEL_HUMAN_BOUNDS;
    }
}

SDL_Rect IslandInteriorScene::GetChestBounds() const
{
        switch (island->GetIslandFaction())
        {
        case IslandFaction::HUMANS:   return CHEST_HUMAN_BOUNDS;
        case IslandFaction::BIRD:     return CHEST_BIRD_BOUNDS;
        case IslandFaction::SIRENS:   return CHEST_SIREN_BOUNDS;
        case IslandFaction::REPTILES: return CHEST_REPTILE_BOUNDS;
        default:                      return CHEST_HUMAN_BOUNDS;
        }
}

void IslandInteriorScene::LoadSound() {
    buttonPress = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/UIfx/button_press.wav");
}

void IslandInteriorScene::UpdateSound() {
    //ckeck for type of island to put ambience
    if (!Engine::GetInstance().audio->IsMusicPlaying()) {
        LOG("Play music again!");
        switch (island->GetIslandFaction()) {
        case IslandFaction::HUMANS:
            //play crowd sounds
            Engine::GetInstance().audio->PlayMusic(humanAmb, 0);
            break;
        case IslandFaction::SIRENS:
            //play wave sounds
            Engine::GetInstance().audio->PlayMusic(sirenAmb, 0);
            break;
        case IslandFaction::REPTILES:
            //play jungle sounds
            Engine::GetInstance().audio->PlayMusic(reptileAmb, 0);
            break;
        }
    }
}
void IslandInteriorScene::GiveReward()
{
    srand(time(NULL));
    rewardAmount = rand() % 50;

    LOG("reward Amount: %d", rewardAmount);
    SDL_Rect claimBounds = CHEST_ITEM_POSITIONS;

    if (rewardAmount > 40) {
        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON, REWARD_EMPTY, "", claimBounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, claimButton, 0, claimBounds.w, claimBounds.h
        );
    }
    else if (rewardAmount < 21) {
        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON, REWARD_GOLD, "", claimBounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, claimButton, 0, claimBounds.w, claimBounds.h
        );
    }
    else {

        Faction faction = Faction::UNDEFINED;
        IslandFaction islandFaction = island->GetIslandFaction();
        if (islandFaction == IslandFaction::HUMANS) { faction = Faction::HUMAN; }
        else if (islandFaction == IslandFaction::BIRD) { faction = Faction::BIRD; }
        else if (islandFaction == IslandFaction::SIRENS) { faction = Faction::SIREN; }
        else if (islandFaction == IslandFaction::REPTILES) { faction = Faction::REPTILE; }

        std::vector<EquippableItem*> equippables = Engine::GetInstance().itemManager->GetEquippablesByFaction(faction);

        if (!equippables.empty())
        {
            //random item
            std::mt19937 rng(std::random_device{}());
            std::uniform_int_distribution<int> dist(0, (int)equippables.size() - 1);
            EquippableItem* chosen = equippables[dist(rng)];

            selectedItem = chosen->Clone();

            //load texture item
            std::string factionFolder = SceneUtils::GetFactionString(islandFaction);
            std::string texturePath = "Assets/Textures/ShopScene/Items/" + factionFolder + "/" + chosen->GetName() + ".png";
            chestItemTexture = Engine::GetInstance().textures->Load(texturePath.c_str());

            LOG("Chest reward: item equippable -> %s", chosen->GetName().c_str());
            CreateCharacterSelectionUI();
        }
    }
}

void IslandInteriorScene::CreateCharacterSelectionUI()
{
    Engine::GetInstance().uiManager->RemoveElementsByRange(CHARACTERS_AVAILABLE_BASE, CHARACTERS_AVAILABLE_BASE + 99);

    for (int i = 0; i < alliedParty->GetMemberCount(); i++)
    {
        SDL_Rect rect = { 190, 455 + i * 63, 202, 63 };

        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON,
            CHARACTERS_AVAILABLE_BASE + i,
            alliedParty->GetMembers()[i]->GetName().c_str(),
            rect,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, emptyButtons, 0, rect.w, rect.h
        );
    }
}
void IslandInteriorScene::OpenUIChest()
{
    chestOpened = false;
    chestPopped = false;
    showChest = true;

    SDL_Rect crossBounds = { 45, 45, 72, 72 };
    SDL_Rect openBounds = { 990, 250, 202, 63 };

    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, CLOSE_CHEST_BUTTON_ID, "", crossBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, exitButton, 0, crossBounds.w, crossBounds.h
    );
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, OPEN_BUTTON_ID, "", openBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, claimButton, 0, openBounds.w, openBounds.h
    );
}

