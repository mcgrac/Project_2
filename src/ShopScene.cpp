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
#include "SceneUtils.h"

ShopScene::ShopScene(Shop* shop, Party* allied)
    : shop(shop), alliedParty(allied), ownerSprite(nullptr)
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
}

void ShopScene::Update(float dt)
{
    Engine::GetInstance().render->DrawTexture(background, 0, 0);
    //lógica de compra de items

    //debug->add 1000 gold pressing F12
    if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_0) == KEY_DOWN)
    {
        LOG("Adding Gold to party");
        alliedParty->AddGold(1000);
        LOG("Party Gold is: %d", alliedParty->GetGold());
    }
}

void ShopScene::PostUpdate(float dt)
{
    Engine::GetInstance().render->DrawText(
        "TIENDA", 540, 50, 200, 40, { 255, 255, 255, 255 }
    );
}

void ShopScene::Unload()
{
    Engine::GetInstance().textures->UnLoad(exitButton);
    Engine::GetInstance().textures->UnLoad(background);
    Engine::GetInstance().textures->UnLoad(emptyButtons);
    Engine::GetInstance().textures->UnLoad(potionButton);
    Engine::GetInstance().textures->UnLoad(keyButton);
    Engine::GetInstance().textures->UnLoad(ownerSprite);

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
}

bool ShopScene::OnUIMouseClickEvent(UIElement* uiElement)
{
    switch (uiElement->id)
    {
    case BACK_BUTTON_ID:
        Engine::GetInstance().audio->PlayFx(buttonPress);
        Engine::GetInstance().scene->PopScene();
        break;
    case OPEN_SHOP_BUTTON:
    {
        Engine::GetInstance().audio->PlayFx(buttonPress);
        NPC* npc = shop->GetOwner();
        if (npc == nullptr)
        {
            LOG("Hostel: NPC es nullptr");
            break;
        }
        else {
            LOG("Hostel: NPC correcto");
        }

        LOG("Dialogue id npc: %s", npc->GetDialogueId().c_str());
        Engine::GetInstance().scene->PushScene(
            new DialogueScene(npc->GetDialogueId(),
                [this]()
                {
                    std::string action = DialogueManager::GetLastChoiceTag();

                    if (action == "buy")
                    {
                        LOG("SHOP: buy");
                        shop->GenerateItems(Faction::BIRD);

                        state = ShopState::SHOW_ITEMS;

                        CreateItemButtons();
                    }
                }
            )
        );
        break;
    }
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

        if (alliedParty->GetInventory().EquipItem(character->GetName(), equippable))
        {
            alliedParty->SpendGold(selectedItem->GetPrice());
            Engine::GetInstance().audio->PlayFx(spendMoneyfx);

            selectedItem->SetPurchased(true);

            LOG("Item equipado");
        }
        else
        {
            LOG("No hay espacio");
        }

        state = ShopState::SHOW_ITEMS;

        // borrar personajes pero mantener items
        Engine::GetInstance().uiManager->RemoveElementsByRange(CHARACTERS_AVAILABLE_BASE, CHARACTERS_AVAILABLE_BASE + 99);
        break;
    }
    default:
        break;
    }
    return true;
}
void ShopScene::OnResume()
{
    CreateUI();
}

void ShopScene::OnPause()
{
    Engine::GetInstance().uiManager->CleanUp();
}

void ShopScene::CreateUI()
{
    SDL_Rect backBounds = { 20, 20, 72, 72 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, BACK_BUTTON_ID, "", backBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, exitButton, 0, backBounds.w, backBounds.h
    );

    // BOTÓN ABRIR TIENDA
    SDL_Rect openBtn = { 500, 600, 309, 186 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, OPEN_SHOP_BUTTON, "OPEN SHOP", openBtn,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, ownerSprite, 0, openBtn.w, openBtn.h
    );
}

void ShopScene::CreateItemButtons()
{
    // borrar items y characters anteriores
    Engine::GetInstance().uiManager->RemoveElementsByRange(ITEMS_AVAILABLE_BASE, CHARACTERS_AVAILABLE_BASE + 99);

    int startX = 400;

    loadedItemTextures.clear();

    for (int i = 0; i < shop->GetCurrentItems().size(); i++)
    {
        SDL_Rect rect = { startX + i * 150, 300, 229, 304};
        SDL_Rect keyBtn = {40, 40, 64, 64};
        SDL_Rect potionBtn = {80, 80, 64, 64};

        std::string label = shop->GetCurrentItems()[i]->GetName();

        if (shop->GetCurrentItems()[i]->IsPurchased())
        {
            label += " (SOLD)";
        }

        Item* item = shop->GetCurrentItems()[i];

        if (item == dynamic_cast<EquippableItem*>(item)) {
#if _DEBUG
            LOG("CREATING ITEMS BUTTONS: Item is equippable (name-> %s)", shop->GetCurrentItems()[i]->GetName().c_str());
#endif // _DEBUG

            IslandFaction faction = shop->GetIsland()->GetIslandFaction();
            std::string factionFolder = SceneUtils::GetFactionString(faction);
            std::string texturePath = "Assets/Textures/ShopScene/Items/" + factionFolder + "/" + shop->GetCurrentItems()[i]->GetName() + ".png";
            SDL_Texture* itemTexture = Engine::GetInstance().textures->Load(texturePath.c_str());

            Engine::GetInstance().uiManager->CreateUIElement(
                UIElementType::BUTTON,
                ITEMS_AVAILABLE_BASE + i,
                label.c_str(),
                rect,
                [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, itemTexture, 0, rect.w, rect.h
            );

            loadedItemTextures.push_back(itemTexture);
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

void ShopScene::CreateCharacterSelectionUI()
{
    Engine::GetInstance().uiManager->RemoveElementsByRange(CHARACTERS_AVAILABLE_BASE, CHARACTERS_AVAILABLE_BASE + 99);

    for (int i = 0; i < alliedParty->GetMemberCount(); i++)
    {
        SDL_Rect rect = { 400, 400 + i * 80, 202, 63 };

        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON,
            CHARACTERS_AVAILABLE_BASE + i,
            alliedParty->GetMembers()[i]->GetName().c_str(),
            rect,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, emptyButtons, 0, rect.w, rect.h
        );
    }
}

void ShopScene::LoadSound() {
    spendMoneyfx = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Island_menu/coin2.wav");
    buttonPress = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/UIfx/button_press.wav");
}
