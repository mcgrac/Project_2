#include "HostelScene.h"
#include "Engine.h"
#include "Audio.h"
#include "Scene.h"
#include "UIManager.h"
#include "Render.h"
#include "Log.h"
#include "Textures.h"
#include "NPC.h"
#include "DialogueScene.h"
#include "DialogueManager.h"
#include "Character.h"
#include "SceneUtils.h"

#pragma region POSITIONS
#pragma region REST_BUTTON
const SDL_Rect HostelScene::REST_HUMAN_BOUNDS = { 100, 50, 414, 414 };
const SDL_Rect HostelScene::REST_BIRD_BOUNDS = { 100, 50, 414, 414 };
const SDL_Rect HostelScene::REST_SIREN_BOUNDS = { 100, 50, 414, 414 };
const SDL_Rect HostelScene::REST_REPTILE_BOUNDS = { 100, 50, 414, 414 };
#pragma endregion
#pragma region MEAL_BUTTON
const SDL_Rect HostelScene::MEAL_HUMAN_BOUNDS = { 500, 50, 414, 414 };
const SDL_Rect HostelScene::MEAL_BIRD_BOUNDS = { 500, 50, 414, 414 };
const SDL_Rect HostelScene::MEAL_SIREN_BOUNDS = { 500, 50, 414, 414 };
const SDL_Rect HostelScene::MEAL_REPTILE_BOUNDS = { 500, 50, 414, 414 };
#pragma endregion
const SDL_Rect HostelScene::CHARA_SELECT_BOUNDS = { 200, 500, 202, 63 };
#pragma endregion

HostelScene::HostelScene(Hostel* hostel, Party* allied)
    : hostel(hostel),
      alliedParty(allied), 
      background(nullptr), 
      exitButton(nullptr), 
      ownerSprite(nullptr),
      restButton(nullptr),
      mealButton(nullptr),
      showRestPanel(false),
      showSelectCharaPanel(false),
      pendingRefresh(false)
{
    sceneName = "HostelScene";
}

HostelScene::~HostelScene() {}

void HostelScene::Load()
{
    LOG("HostelScene: cargando hostel.");
    LoadTextures();
    LoadSound();
    CreateUI();
}

void HostelScene::Update(float dt)
{
    if (pendingRefresh) //avoids double clicking buttons
    {
        pendingRefresh = false;

        //delete and update base UI
        Engine::GetInstance().uiManager->CleanUp();
        CreateUI();
    }

    Engine::GetInstance().render->DrawTexture(background, 0, 0);
}

void HostelScene::PostUpdate(float dt)
{
    Engine::GetInstance().render->DrawText(
        "HOSTEL", 540, 50, 200, 40, { 255, 255, 255, 255 }
    );

    if (showRestPanel)
    {
        SDL_Rect panel = { 350, 250, 400, 200 };
        Engine::GetInstance().render->DrawRectangle(panel, 0, 0, 0, 200, true, false);
    }
}

void HostelScene::Unload()
{
    Engine::GetInstance().textures->UnLoad(exitButton);
    Engine::GetInstance().textures->UnLoad(background);
    Engine::GetInstance().textures->UnLoad(ownerSprite);
    Engine::GetInstance().textures->UnLoad(restButton);
    Engine::GetInstance().textures->UnLoad(mealButton);
    Engine::GetInstance().textures->UnLoad(emptyButtons);

    Engine::GetInstance().uiManager->CleanUp();
}

void HostelScene::LoadTextures()
{
    //always the same
    exitButton = Engine::GetInstance().textures->Load("Assets/Textures/HumanIsland/BackButton.png");
    restButton = Engine::GetInstance().textures->Load("Assets/Textures/HostelScene/RestButton.png");
    emptyButtons = Engine::GetInstance().textures->Load("Assets/Textures/ShopScene/EmptyTextButton.png");

    IslandFaction faction = hostel->GetIsland()->GetIslandFaction();
    std::string path = "Assets/Textures/HostelScene/" + SceneUtils::GetFactionString(faction) + "/";
    background = Engine::GetInstance().textures->Load((path + "background.png").c_str());
    ownerSprite = Engine::GetInstance().textures->Load((path + "ownerSprite.png").c_str());
    mealButton = Engine::GetInstance().textures->Load((path + "mealButton.png").c_str());
}

void HostelScene::LoadSound() {
    restfx = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/UIfx/rest.wav");
    buttonPress = buttonPress = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/UIfx/button_press.wav");
}

bool HostelScene::OnUIMouseClickEvent(UIElement* uiElement)
{
    pendingRefresh = true;

    switch (uiElement->id)
    {
    case BACK_BUTTON_ID:
        //sound of basic button
        Engine::GetInstance().audio->PlayFx(buttonPress);
        Engine::GetInstance().scene->PopScene();
        break;
    case START_DIALOGUE:
    {
        //start dialogue

        NPC* npc = hostel->GetOwner();
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
                    
                    if (action == "rest")
                    {
                        LOG("Hostel: abrir panel de descanso");
                        showRestPanel = true;
                    }
                }
            )
        );
        break;
    }
    case 20:
        //rest
        if (hostel->CheckGold(hostel->GetRestCost(), alliedParty)) {
            Engine::GetInstance().audio->PlayFx(restfx);
            hostel->Rest(alliedParty);
            showRestPanel = false;
        }
        else {
#if _DEBUG
            LOG("Not enough gold for resting");
#endif // _DEBUG
        }

        break;
    case 21:
        //buy xp
        //hostel->BuyXP(alliedParty, 50);
        if (hostel->CheckGold(hostel->GetMealCost(), alliedParty)) {
            showRestPanel = false;
            showSelectCharaPanel = true;
        }
        else {
#if _DEBUG
            LOG("Not enough gold for a meal");
#endif // _DEBUG
        }
        break;
    case 22:
        //back panel
        Engine::GetInstance().audio->PlayFx(buttonPress);
        showRestPanel = false;
        showSelectCharaPanel = false;
        break;

    //characters selection panel
    case 30:
    case 31:
    case 32:
        hostel->GetADrink(alliedParty, uiElement->text);
        showRestPanel = false;
        showSelectCharaPanel = false;
        break;
    case 33:
        showSelectCharaPanel = false;
        showRestPanel = true;
        break;
    default:
        break;
    }

    return true;
}
void HostelScene::OnResume()
{
    CreateUI();
}

void HostelScene::OnPause()
{
    Engine::GetInstance().uiManager->CleanUp();
}

void HostelScene::CreateUI()
{
    if(!showRestPanel && !showSelectCharaPanel)
    {
        SDL_Rect backBounds = { 20, 20, 72, 72 };
        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON, BACK_BUTTON_ID, "", backBounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, exitButton, 0, backBounds.w, backBounds.h
        );

        SDL_Rect talkBounds = { 500, 300, 168, 211 };
        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON, START_DIALOGUE, "", talkBounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, hostel->GetOwner()->GetTexture(), 0, talkBounds.w, talkBounds.h
        );
    }
    else if (showRestPanel && !showSelectCharaPanel) {
        OpenRestPanel();
    }
    else if (!showRestPanel && showSelectCharaPanel) {
        OpenSelectCharaPanel();
    }
}

void HostelScene::OpenRestPanel()
{
    LOG("Show rest panel");

    showRestPanel = true;

    SDL_Rect restBtn = GetRestBounds();
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 20, "", restBtn,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, restButton, 0, restBtn.w, restBtn.h
    );

    SDL_Rect xpBtn = GetMealBounds();
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 21, "", xpBtn,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, mealButton, 0, xpBtn.w, xpBtn.h
    );

    SDL_Rect backPanel = { 400, 440, 72, 72 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 22, "", backPanel,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, exitButton, 0, backPanel.w, backPanel.h
    );
}

void HostelScene::OpenSelectCharaPanel()
{
    LOG("Show select chara panel");

    //character buttons
    for (int i = 0; i < alliedParty->GetMemberCount(); i++) {
        SDL_Rect charaBtn = CHARA_SELECT_BOUNDS;
        charaBtn.y += (70 * i);
        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON, 30 + i, alliedParty->GetMembers()[i]->GetName().c_str(), charaBtn,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, emptyButtons, 0, charaBtn.w, charaBtn.h
        );
    }


    SDL_Rect backPanel = { 400, 500, 72, 72 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 33, "back", backPanel,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, exitButton, 0, backPanel.w, backPanel.h
    );
}

SDL_Rect HostelScene::GetRestBounds() const
{
    switch (hostel->GetIsland()->GetIslandFaction())
    {
    case IslandFaction::HUMANS:   return REST_HUMAN_BOUNDS;
    case IslandFaction::BIRD:     return REST_BIRD_BOUNDS;
    case IslandFaction::SIRENS:   return REST_SIREN_BOUNDS;
    case IslandFaction::REPTILES: return REST_REPTILE_BOUNDS;
    default:                      return REST_HUMAN_BOUNDS;
    }
}

SDL_Rect HostelScene::GetMealBounds() const
{
    switch (hostel->GetIsland()->GetIslandFaction())
    {
    case IslandFaction::HUMANS:   return MEAL_HUMAN_BOUNDS;
    case IslandFaction::BIRD:     return MEAL_BIRD_BOUNDS;
    case IslandFaction::SIRENS:   return MEAL_SIREN_BOUNDS;
    case IslandFaction::REPTILES: return MEAL_REPTILE_BOUNDS;
    default:                      return MEAL_HUMAN_BOUNDS;
    }
}