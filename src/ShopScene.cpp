#include "ShopScene.h"
#include "Engine.h"
#include "Audio.h"
#include "Scene.h"
#include "UIManager.h"
#include "Render.h"
#include "Log.h"
#include "Textures.h"
#include "Party.h"
#include "Character.h"
#include "DialogueScene.h"
#include "DialogueManager.h"
#include "Log.h"
#include "NPC.h"
#include "EquippableItem.h"
#include "ConsumableItem.h"
#include "KeyItem.h"
#include "Party.h"
#include "ItemManager.h"
#include <algorithm>
#include <random>
#include "QuestManager.h"
#include "Inventory.h"

#pragma region POSITIONS
#pragma region CHEST
const SDL_Rect ShopScene::CHEST_HUMAN_BOUNDS = { 784,  154,  93,  90 };
const SDL_Rect ShopScene::CHEST_BIRD_BOUNDS = { 112,  405,  192,  210 };
const SDL_Rect ShopScene::CHEST_SIREN_BOUNDS = { 228,  468,  102,  117 };
const SDL_Rect ShopScene::CHEST_REPTILE_BOUNDS = { 626,  486,  54,  42 };
#pragma endregion
#pragma region NPC
const SDL_Rect ShopScene::HUMAN_CHARA_SELECT_BOUNDS = { 484, 256, 315, 192 };
const SDL_Rect ShopScene::BIRD_CHARA_SELECT_BOUNDS = { 308, 257, 241, 270 };
const SDL_Rect ShopScene::SIREN_CHARA_SELECT_BOUNDS = { 903,  312, 292, 265 };
const SDL_Rect ShopScene::REPTILE_CHARA_SELECT_BOUNDS = { 297, 170, 201, 204 };
#pragma endregion
#pragma region Sprite
const SDL_Rect ShopScene::HUMAN_SPRITE_BOUNDS = { 424, 380, 764, 276 };
const SDL_Rect ShopScene::BIRD_SPRITE_BOUNDS = { 424, 364, 790, 312 };
const SDL_Rect ShopScene::SIREN_SPRITE_BOUNDS = { 424, 293, 931, 663 };
const SDL_Rect ShopScene::REPTILE_SPRITE_BOUNDS = { 424, 290, 856, 366 };
#pragma endregion
#pragma region CROSS
const SDL_Rect ShopScene::CROSS_BOUNDS = { 45, 45, 72, 72 };
#pragma endregion
#pragma region CROSS2
const SDL_Rect ShopScene::CROSS_BOUNDS2 = { 118, 63, 72, 72 };
#pragma endregion
#pragma region OPEN_BUTTON
const SDL_Rect ShopScene::OPEN_BUTTON_BOUNDS = { 990, 250, 202, 63 };
#pragma endregion
#pragma region CHEST_ITEM_POSITION
const SDL_Rect ShopScene::CHEST_ITEM_POSITIONS = { 549, 507, 182, 72 };
#pragma endregion
#pragma region Rewards
const SDL_Rect ShopScene::REWARD_BOUNDS = { 526, 185, 229, 304 };
#pragma endregion
#pragma endregion

ShopScene::ShopScene(Shop* shop, Party* allied)
    : shop(shop),
    alliedParty(allied), 
    ownerSprite(nullptr), 
    goldCounter(0, "Assets/Textures/Animations/coin.png", 1160, 62),
    chestItemTexture(nullptr)
{
    sceneName = "ShopScene";
    
}

ShopScene::~ShopScene() {}

void ShopScene::Load()
{
    LOG("ShopScene: cargando tienda.");
    LoadTextures();
    LoadSound();
    CreateUI();

    IslandFaction faction = shop->GetIsland()->GetIslandFaction();
    if (faction == IslandFaction::SIRENS || faction == IslandFaction::REPTILES)
    {
        pendingDialogue = true;
    }

    goldCounter.SetPosition(1160, 62);
}

void ShopScene::Update(float dt)
{
    

    if (pendingDialogue)
    {
        pendingDialogue = false;
        PushDialogue();
    }

    if (pendingPop)
    {
        pendingPop = false;
        Engine::GetInstance().scene->PopScene();
        return;
    }

    if(chestOpen == true){ 
        Engine::GetInstance().render->DrawTexture(chestBackground, 0, 0); 
        if (animationPlaying) { PlayAnimation(dt); }
        Engine::GetInstance().render->DrawTexture(moneyCounter, 990, 70);
        Engine::GetInstance().render->DrawTexture(keyCounter, 990, 147);

        SDL_Color White = { 255, 255, 255 };
        int keys = alliedParty->GetInventory().GetItemCount("key");
        std::string key = std::to_string(keys);
        keyNum = 1;
        if (keys >= 10) { keyNum += 1; }
        Engine::GetInstance().render->DrawText((key).c_str(), 1114, 164, 19*keyNum, 33, White);

        SDL_Rect rewardBounds = REWARD_BOUNDS;

        if (chestPopped) {
            if (rewardAmount > 40) {

                Engine::GetInstance().render->DrawTexture(emptyCard, REWARD_BOUNDS.x, REWARD_BOUNDS.y);

            }
            else if (rewardAmount < 21) {

                Engine::GetInstance().render->DrawTexture(moneyCard, REWARD_BOUNDS.x, REWARD_BOUNDS.y);
                
                SDL_Color White = { 255, 255, 255 };
                potNum = 0;
                if (rewardAmount >= 10) { potNum += 1; }

                std::string price = std::to_string(rewardAmount);
                Engine::GetInstance().render->DrawText((price).c_str(), 629-(12*potNum), 431, 23 + (20*potNum), 26, White);
            }
            else 
            {
                if (chestItemTexture != nullptr)
                {
                    float texH;
                    float texW;
                    SDL_GetTextureSize(chestItemTexture, &texW, &texH);

                    SDL_Rect firstFrame;
                    firstFrame.x = 0;
                    firstFrame.y = 0;
                    firstFrame.w = (int)texW;
                    firstFrame.h = (int)texH / 3;

                    Engine::GetInstance().render->DrawTexture(chestItemTexture, REWARD_BOUNDS.x, REWARD_BOUNDS.y, &firstFrame);
                }
            }
        }
    }
    else if (shopOpen == true) {

        IslandFaction faction = shop->GetIsland()->GetIslandFaction();
        SDL_Rect CHARACTER_BOUNDS = GetSpriteBounds();

        Engine::GetInstance().render->DrawTexture(fullBackground, 0, 0);
        Engine::GetInstance().render->DrawTexture(moneyCounter, 938, 296);
        Engine::GetInstance().render->DrawTexture(otherCounter, 961, 135);
        Engine::GetInstance().render->DrawTexture(otherCounter, 961, 207);
        Engine::GetInstance().render->DrawTexture(characterSprite, CHARACTER_BOUNDS.x, CHARACTER_BOUNDS.y);

        SDL_Color White = { 255, 255, 255 };
        int consumable = alliedParty->GetInventory().GetItemCount("consumable");
        potNum = 1;
        if(consumable>= 10){ potNum += 1; }
       
        std::string potions = std::to_string(consumable);
        Engine::GetInstance().render->DrawText((potions).c_str(), 1087, 222, 19*potNum, 33, White);

        int keys = alliedParty->GetInventory().GetItemCount("key");
        std::string key = std::to_string(keys);
        keyNum = 1;
        if (keys >= 10) { keyNum += 1; }
        Engine::GetInstance().render->DrawText((key).c_str(), 1087, 151, 19*keyNum, 33, White);
    }
    else { 
        Engine::GetInstance().render->DrawTexture(background, 0, 0); 
        Engine::GetInstance().render->DrawTexture(moneyCounter, 1121, 30, nullptr, 0);
    }

    //debug->add 1000 gold pressing F12
    if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_0) == KEY_DOWN)
    {
        LOG("Adding Gold to party");
        alliedParty->AddGold(1000);
        LOG("Party Gold is: %d", alliedParty->GetGold());
    }

    goldCounter.Update(alliedParty->GetGold(), dt);
   
}

void ShopScene::PostUpdate(float dt)
{

}

void ShopScene::Unload()
{
    Engine::GetInstance().textures->UnLoad(exitButton);
    Engine::GetInstance().textures->UnLoad(background);
    Engine::GetInstance().textures->UnLoad(emptyButtons);
    Engine::GetInstance().textures->UnLoad(potionButton);
    Engine::GetInstance().textures->UnLoad(keyButton);
    Engine::GetInstance().textures->UnLoad(ownerSprite);
    Engine::GetInstance().textures->UnLoad(chestButton);
    Engine::GetInstance().textures->UnLoad(fullBackground);
    Engine::GetInstance().textures->UnLoad(chestBackground);

    Engine::GetInstance().textures->UnLoad(openButton);
    Engine::GetInstance().textures->UnLoad(moneyCounter);
    Engine::GetInstance().textures->UnLoad(otherCounter);
    Engine::GetInstance().textures->UnLoad(keyCounter);
    Engine::GetInstance().textures->UnLoad(fullBackground);
    Engine::GetInstance().textures->UnLoad(characterSprite);

    for (SDL_Texture* tex : loadedItemTextures)
    {
        Engine::GetInstance().textures->UnLoad(tex);
    }
    loadedItemTextures.clear();

    Engine::GetInstance().uiManager->CleanUp();
}

void ShopScene::LoadTextures()
{
    exitButton = Engine::GetInstance().textures->Load("Assets/Textures/HumanIsland/BackButton.png");
    //background = Engine::GetInstance().textures->Load("Assets/Textures/HumanIsland/ShopBackground.png");
    emptyButtons = Engine::GetInstance().textures->Load("Assets/Textures/ShopScene/EmptyTextButton.png");

    keyButton = Engine::GetInstance().textures->Load("Assets/Textures/ShopScene/BuyKeyButton.png");
    potionButton = Engine::GetInstance().textures->Load("Assets/Textures/ShopScene/BuyPotionButton.png");

    IslandFaction faction = shop->GetIsland()->GetIslandFaction();
    std::string path = "Assets/Textures/ShopScene/" + SceneUtils::GetFactionString(faction);
    background = Engine::GetInstance().textures->Load((path + "/background.png").c_str());
    ownerSprite = Engine::GetInstance().textures->Load((path + "/ownerSprite.png").c_str());
    chestButton = Engine::GetInstance().textures->Load(("Assets/Textures/ShopScene/chest" + SceneUtils::GetFactionString(shop->GetIsland()->GetIslandFaction()) + ".png").c_str());
    chestBackground = Engine::GetInstance().textures->Load("Assets/Textures/ShopScene/chestBackground.png");
    openButton = Engine::GetInstance().textures->Load("Assets/Textures/ShopScene/OpenButton.png");
    moneyCounter = Engine::GetInstance().textures->Load("Assets/Textures/ShopScene/moneyCounter.png");
    otherCounter = Engine::GetInstance().textures->Load("Assets/Textures/ShopScene/BackpackCounter.png");
    keyCounter = Engine::GetInstance().textures->Load("Assets/Textures/ShopScene/keyCounter.png");

    //chestSpritesheet = Engine::GetInstance().textures->Load("Assets/Textures/Animations/chestAnimated.png");
    chestSpritesheet = Engine::GetInstance().textures->Load("Assets/Textures/Animations/chestAnimated.png");

    claimButton = Engine::GetInstance().textures->Load("Assets/Textures/ShopScene/claimButton.png");
    emptyCard = Engine::GetInstance().textures->Load("Assets/Textures/ShopScene/emptyCard.png");
    moneyCard = Engine::GetInstance().textures->Load("Assets/Textures/ShopScene/moneyCard.png");
    
    
    std::unordered_map<int, std::string> aliases = { {0,"idle"} };
    anims.LoadFromTSX("Assets/Textures/Animations/chestAnimated.tsx", aliases);
    anims.SetLoop("idle", false);

    fullBackground = Engine::GetInstance().textures->Load((path + "/fullBack.png").c_str());
    characterSprite = Engine::GetInstance().textures->Load((path + "/NPC.png").c_str());

}

bool ShopScene::OnUIMouseClickEvent(UIElement* uiElement)
{
    switch (uiElement->id)
    {
    case BACK_BUTTON_ID:
        Engine::GetInstance().audio->PlayFx(buttonPress);
        if (chestPopped) { break; }
        goldCounter.MoveCounter(1144, 62);
        Engine::GetInstance().scene->PopScene();
        break;
    case OPEN_CHEST_ID:
        if (chestItemTexture != nullptr)
        {
            Engine::GetInstance().textures->UnLoad(chestItemTexture);
            chestItemTexture = nullptr;
        }
        Engine::GetInstance().audio->PlayFx(buttonPress);
        Engine::GetInstance().uiManager->RemoveElementsByRange(0, 10);
        chestOpen = true;
        OpenUIChest();
        goldCounter.MoveCounter(1016, 101);
        break;
    case OPEN_SHOP_BUTTON:
    {
        Engine::GetInstance().audio->PlayFx(buttonPress);
        Engine::GetInstance().uiManager->RemoveElementsByRange(0, 10);
        PushDialogue();
        goldCounter.SetPosition(965, 328);
        break;
    }
    case CLOSE_CHEST_ID:
        if (chestOpen && selectedItem != nullptr && !selectedItem->IsPurchased())
        {
            delete selectedItem;
            selectedItem = nullptr;
        }
        if (chestItemTexture != nullptr)
        {
            Engine::GetInstance().textures->UnLoad(chestItemTexture);
            chestItemTexture = nullptr;
        }
        Engine::GetInstance().audio->PlayFx(buttonPress);
        if (chestPopped) { break; }
        if (shopOpen)
        {
            IslandFaction faction = shop->GetIsland()->GetIslandFaction();
            if (faction == IslandFaction::SIRENS || faction == IslandFaction::REPTILES)
            {
                Engine::GetInstance().scene->PopScene();
                break;
            }
        }

        Engine::GetInstance().uiManager->RemoveElementsByRange(0, 300);
        chestOpen = 0;
        shopOpen = 0;

        goldCounter.SetPosition(1160, 62);
        CreateUI();
        break;
    case OPEN_BUTTON_ID:
        //if money and animationPlaying 0, take money. else break
        Engine::GetInstance().audio->PlayFx(buttonPress);
        if (chestPopped) { break; }
        if (alliedParty->GetInventory().GetItemCount("key") < 1) { break; }
        alliedParty->GetInventory().ConsumeItem("key");
        
        animationPlaying = true;
        anims.SetCurrent("idle");
        anims.SetLoop("idle", false);
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

    case 100:
    case 101:
    case 102:
    case 103: //potion
    case 104: //key
    {
        int index = uiElement->id - ITEMS_AVAILABLE_BASE;

        Item* item = shop->GetCurrentItems()[index];

        if (alliedParty->GetGold() <= item->GetPrice())
        {
            LOG("No tienes suficiente oro");
            return true;
        }

        if (EquippableItem* equippable = dynamic_cast<EquippableItem*>(item)) { //if is qquippable
            if (item->IsPurchased())
            {
                LOG("Item ya comprado");
                return true;
            }
            else {
                //play spend money sound
                Engine::GetInstance().audio->PlayFx(spendMoneyfx);
            }
            selectedItem = item;
            selectedItemButton = uiElement;
            state = ShopState::SELECT_CHARACTER;
            CreateCharacterSelectionUI();
        }

        if (ConsumableItem* consumable = dynamic_cast<ConsumableItem*>(item)) { //if is consumable
            alliedParty->SpendGold(item->GetPrice());
            alliedParty->GetInventory().AddItem("consumable");
            //play spend money sound
            Engine::GetInstance().audio->PlayFx(spendMoneyfx);

#if _DEBUG
            LOG("|Consumable purchased|");
            LOG("Consumables in the party: %d", alliedParty->GetInventory().GetItemCount("consumable"));
#endif // _DEBUG
        }

        if (KeyItem* keyItem = dynamic_cast<KeyItem*>(item)) { //if is key
            alliedParty->SpendGold(item->GetPrice());
            alliedParty->GetInventory().AddItem("key");
            Engine::GetInstance().audio->PlayFx(spendMoneyfx);

#if _DEBUG
            LOG("|Key purchased|");
            LOG("Keys in the party: %d", alliedParty->GetInventory().GetItemCount("key"));
#endif // _DEBUG
        }
        break;
    }
    case 200:
    case 201:
    case 202:
    {
        int index = uiElement->id - CHARACTERS_AVAILABLE_BASE;
        Character* character = alliedParty->GetMembers()[index];

        EquippableItem* equippable = dynamic_cast<EquippableItem*>(selectedItem);
        if (!equippable) { return false; }

        if (chestOpen) //chest
        {
            if (alliedParty->GetInventory().EquipItem(character->GetName(), equippable))
            {
                selectedItem->SetPurchased(true);
                chestPopped = false;
                rewardAmount = 0;
                LOG("Item equipado desde chest");
            }
            else
            {
                alliedParty->AddGold(10);
                chestPopped = false;
                rewardAmount = 0;
                LOG("No hay espacio, ganar 10 gold");
            }

            Engine::GetInstance().uiManager->RemoveElementsByRange(10, 300);
        }
        else //pnale shop
        {
            if (alliedParty->GetInventory().EquipItem(character->GetName(), equippable))
            {
                equippable->Use(character);
                alliedParty->SpendGold(selectedItem->GetPrice());
                Engine::GetInstance().audio->PlayFx(spendMoneyfx);
                selectedItem->SetPurchased(true);

                UIButton* btn = dynamic_cast<UIButton*>(selectedItemButton);
                if (btn != nullptr)
                {
                    btn->SetDisabledRow(2);
                    btn->SetDisabled(true);
                }
                selectedItemButton = nullptr;

                //check quest buy an item
                QuestManager::GetInstance().OnItemPurchased(equippable->GetName(), SceneUtils::GetFactionString(shop->GetIsland()->GetIslandFaction()));

                LOG("Item equipado desde tienda");
            }
            else
            {
                 LOG("No hay espacio");
            }

            state = ShopState::SHOW_ITEMS;
            Engine::GetInstance().uiManager->RemoveElementsByRange(CHARACTERS_AVAILABLE_BASE, CHARACTERS_AVAILABLE_BASE + 99);
        }
        break;
    }
    default:
        break;
    }
    return true;
}

void ShopScene::OnResume()
{
    if (pendingPop) { return; }

    CreateUI();

    if (state == ShopState::SHOW_ITEMS)
    {
        Engine::GetInstance().uiManager->RemoveElementsByRange(OPEN_SHOP_BUTTON, OPEN_SHOP_BUTTON);
        CreateItemButtons();
    }
}

void ShopScene::OnPause()
{
    Engine::GetInstance().uiManager->CleanUp();
}

SDL_Rect ShopScene::GetOwnerBounds() const
{
    switch (shop->GetIsland()->GetIslandFaction())
    {
    case IslandFaction::HUMANS:   return HUMAN_CHARA_SELECT_BOUNDS;
    case IslandFaction::BIRD:     return BIRD_CHARA_SELECT_BOUNDS;
    case IslandFaction::SIRENS:   return SIREN_CHARA_SELECT_BOUNDS;
    case IslandFaction::REPTILES: return REPTILE_CHARA_SELECT_BOUNDS;
    default:                      return HUMAN_CHARA_SELECT_BOUNDS;
    }
}

SDL_Rect ShopScene::GetSpriteBounds() const
{
    switch (shop->GetIsland()->GetIslandFaction())
    {
    case IslandFaction::HUMANS:   return HUMAN_SPRITE_BOUNDS;
    case IslandFaction::BIRD:     return BIRD_SPRITE_BOUNDS;
    case IslandFaction::SIRENS:   return SIREN_SPRITE_BOUNDS;
    case IslandFaction::REPTILES: return REPTILE_SPRITE_BOUNDS;
    default:                      return HUMAN_SPRITE_BOUNDS;
    }
}

void ShopScene::CreateUI()
{

    SDL_Rect chestBounds = GetChestBoundsShop();

    IslandFaction faction = shop->GetIsland()->GetIslandFaction();
    if (faction == IslandFaction::HUMANS || faction == IslandFaction::BIRD) {
        Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, OPEN_CHEST_ID, "", chestBounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, chestButton, 0, chestBounds.w, chestBounds.h
            );
    }

    SDL_Rect backBounds = { 20, 20, 72, 72 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, BACK_BUTTON_ID, "", backBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, exitButton, 0, backBounds.w, backBounds.h
    );

    // BOTÓN ABRIR TIENDA
    SDL_Rect openBtn = GetOwnerBounds();
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, OPEN_SHOP_BUTTON, "", openBtn,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, ownerSprite, 0, openBtn.w, openBtn.h
    );


}

void ShopScene::PushDialogue()
{
    NPC* npc = shop->GetOwner();
    if (npc == nullptr)
    {
        LOG("Hostel: NPC es nullptr");
        return;
    }
    else {
        LOG("Hostel: NPC correcto");
    }
    IslandFaction faction = shop->GetIsland()->GetIslandFaction();
    bool popOnLeave = (faction == IslandFaction::SIRENS || faction == IslandFaction::REPTILES);

    LOG("Dialogue id npc: %s", npc->GetDialogueId().c_str());
    Engine::GetInstance().scene->PushScene(
        new DialogueScene(npc->GetDialogueId(),
            [this, popOnLeave]()
            {
                std::string action = DialogueManager::GetLastChoiceTag();

                if (action == "buy")
                {
                    LOG("SHOP: buy");
                    goldCounter.MoveCounter(965, 328);
                    Faction factShop = Faction::UNDEFINED;
                    if (shop->GetIsland()->GetIslandFaction() == IslandFaction::HUMANS) { factShop = Faction::HUMAN; }
                    else if (shop->GetIsland()->GetIslandFaction() == IslandFaction::BIRD) { factShop = Faction::BIRD; }
                    else if (shop->GetIsland()->GetIslandFaction() == IslandFaction::SIRENS) { factShop = Faction::SIREN; }
                    else if (shop->GetIsland()->GetIslandFaction() == IslandFaction::REPTILES) { factShop = Faction::REPTILE; }
                    shop->GenerateItems(factShop);

                    state = ShopState::SHOW_ITEMS;
                    CreateItemButtons();
                    shopOpen = 1;
                    Engine::GetInstance().uiManager->RemoveElementsByRange(0, 10);

                    SDL_Rect crossBounds2 = CROSS_BOUNDS2;
                    Engine::GetInstance().uiManager->CreateUIElement(
                        UIElementType::BUTTON, CLOSE_CHEST_ID, "", crossBounds2,
                        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, exitButton, 0, crossBounds2.w, crossBounds2.h
                    );

                    
                }
                else if (popOnLeave)
                {
                    LOG("DOCK SCENE: Pop scene");
                    state = ShopState::CLOSED;
                    shopOpen = false;
                    pendingPop = true;
                }
                else
                {
                    state = ShopState::CLOSED;
                    shopOpen = false;
                }
            }
        )
    );
}

void ShopScene::CreateItemButtons()
{
    // borrar items y characters anteriores
    Engine::GetInstance().uiManager->RemoveElementsByRange(ITEMS_AVAILABLE_BASE, CHARACTERS_AVAILABLE_BASE + 99);
    Engine::GetInstance().uiManager->RemoveElementsByRange(0,100);

    int startX = 193;

    loadedItemTextures.clear();

    SDL_Rect backBounds = { 118, 63, 72, 72 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, CLOSE_CHEST_ID, "", backBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, exitButton, 0, backBounds.w, backBounds.h
    );

    for (int i = 0; i < shop->GetCurrentItems().size(); i++)
    {
        SDL_Rect rect = { startX + i * 256, 138, 229, 304};
        SDL_Rect keyBtn = {961, 135, 64, 64};
        SDL_Rect potionBtn = {961, 207, 64, 64};

        std::string label = shop->GetCurrentItems()[i]->GetName();

        if (shop->GetCurrentItems()[i]->IsPurchased())
        {
            label += " (SOLD)";
        }

        Item* item = shop->GetCurrentItems()[i];

        bool purchased = item->IsPurchased();

        if (item == dynamic_cast<EquippableItem*>(item)) {
#if _DEBUG
            LOG("CREATING ITEMS BUTTONS: Item is equippable (name-> %s)", shop->GetCurrentItems()[i]->GetName().c_str());
#endif // _DEBUG

            IslandFaction faction = shop->GetIsland()->GetIslandFaction();
            std::string factionFolder = SceneUtils::GetFactionString(faction);
            std::string texturePath = "Assets/Textures/ShopScene/Items/" + factionFolder + "/" + shop->GetCurrentItems()[i]->GetName() + ".png";
            SDL_Texture* itemTexture = Engine::GetInstance().textures->Load(texturePath.c_str());

            auto element = Engine::GetInstance().uiManager->CreateUIElement(
                UIElementType::BUTTON,
                ITEMS_AVAILABLE_BASE + i,
                "",
                rect,
                [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, itemTexture, 0, rect.w, rect.h
            );

            loadedItemTextures.push_back(itemTexture);

            // Si ya fue comprado, deshabilitar directamente
            if (purchased && element != nullptr)
            {
                UIButton* btn = dynamic_cast<UIButton*>(element.get());
                if (btn != nullptr)
                {
                    btn->SetDisabledRow(2);
                    btn->SetDisabled(true);
                }
            }
        }
        else if (item == dynamic_cast<KeyItem*>(item)) {
#if _DEBUG
            LOG("CREATING ITEMS BUTTONS: Item is key");
#endif // _DEBUG
            Engine::GetInstance().uiManager->CreateUIElement(
                UIElementType::BUTTON,
                ITEMS_AVAILABLE_BASE + i,
                "",
                keyBtn,
                [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, keyButton, 0, keyBtn.w, keyBtn.h
            );
        }
        else if (item == dynamic_cast<ConsumableItem*>(item)) {
#if _DEBUG
            LOG("CREATING ITEMS BUTTONS: Item is consumable");
#endif // _DEBUG
            Engine::GetInstance().uiManager->CreateUIElement(
                UIElementType::BUTTON,
                ITEMS_AVAILABLE_BASE + i,
                "",
                potionBtn,
                [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, potionButton, 0, potionBtn.w, potionBtn.h
            );
        }
        else {
#if _DEBUG
            LOG("Item not recognized");
#endif // _DEBUG
        }
    }
}



void ShopScene::OpenUIChest()
{
  
    chestOpen = 1;

    SDL_Rect crossBounds = CROSS_BOUNDS;
    SDL_Rect openBounds = OPEN_BUTTON_BOUNDS;

    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, CLOSE_CHEST_ID, "", crossBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, exitButton, 0, crossBounds.w, crossBounds.h
    );
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, OPEN_BUTTON_ID, "", openBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, openButton, 0, openBounds.w, openBounds.h
    );
}

void ShopScene::PlayAnimation(float dt)
{
    if (chestSpritesheet == nullptr) { LOG("chest spritesheet nullptr"); }
    anims.Update(dt);

    const SDL_Rect& animFrame = anims.GetCurrentFrame();

    Engine::GetInstance().render->DrawTexture(
        chestSpritesheet,
        0,
        0,
        &animFrame
    );

    if (anims.IsCurrentFinished()) {
        animationPlaying = false; chestPopped = true; GiveReward();
    };
}

void ShopScene::GiveReward()
{
    srand(time(NULL));
    rewardAmount = rand()%50;

    LOG("reward Amount: %d", rewardAmount);
    SDL_Rect claimBounds = CHEST_ITEM_POSITIONS;

    if (rewardAmount > 40) {
        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON, REWARD_EMPTY, "", claimBounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, claimButton, 0, claimBounds.w, claimBounds.h
        );
    }
    else if (rewardAmount < 21){
        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON, REWARD_GOLD, "", claimBounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, claimButton, 0, claimBounds.w, claimBounds.h
        );
    }
    else { 

        Faction faction = Faction::UNDEFINED;
        IslandFaction islandFaction = shop->GetIsland()->GetIslandFaction();
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

            chestOpen = true; 
            LOG("Chest reward: item equippable -> %s", chosen->GetName().c_str());
            CreateCharacterSelectionUI();
        }
    }
}

void ShopScene::CreateCharacterSelectionUI()
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

SDL_Rect ShopScene::GetChestBoundsShop() const
{
    switch (shop->GetIsland()->GetIslandFaction())
    {
    case IslandFaction::HUMANS:   return CHEST_HUMAN_BOUNDS;
    case IslandFaction::BIRD:     return CHEST_BIRD_BOUNDS;
    case IslandFaction::SIRENS:   return CHEST_SIREN_BOUNDS;
    case IslandFaction::REPTILES: return CHEST_REPTILE_BOUNDS;
    default:                      return CHEST_HUMAN_BOUNDS;
    }
}

void ShopScene::LoadSound() {
    spendMoneyfx = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Island_menu/coin2.wav");
    buttonPress = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/UIfx/button_press.wav");
}
