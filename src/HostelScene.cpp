#include "HostelScene.h"
#include "Engine.h"
#include "Scene.h"
#include "UIManager.h"
#include "Render.h"
#include "Log.h"
#include "Textures.h"
#include "NPC.h"
#include "DialogueScene.h"
#include "DialogueManager.h"
#include "Character.h"

HostelScene::HostelScene(Hostel* hostel, Party* allied)
    : hostel(hostel),
      alliedParty(allied), 
      background(nullptr), 
      exitButton(nullptr), 
      ownerSprite(nullptr), 
      showRestPanel(false),
      showSelectCharaPanel(false)
{
    sceneName = "HostelScene";
}

HostelScene::~HostelScene() {}

void HostelScene::Load()
{
    LOG("HostelScene: cargando hostel.");
    LoadTextures();
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
    // lógica de descanso y compra de experiencia
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

    Engine::GetInstance().uiManager->CleanUp();
}

void HostelScene::LoadTextures()
{
    exitButton = Engine::GetInstance().textures->Load("Assets/Textures/HumanIsland/BackButton.png");
    background = Engine::GetInstance().textures->Load("Assets/Textures/HumanIsland/HostelBackground.png");
    ownerSprite = Engine::GetInstance().textures->Load("Assets/Textures/HumanIsland/BackButton.png");
}

bool HostelScene::OnUIMouseClickEvent(UIElement* uiElement)
{
    pendingRefresh = true;

    switch (uiElement->id)
    {
    case BACK_BUTTON_ID:
        Engine::GetInstance().scene->PopScene();
        break;
    case 10:
    {
        //start dialogue

        NPC* npc = hostel->GetOwner();
        if (npc == nullptr)   // <-- añade esto
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
        hostel->Rest(alliedParty);
        showRestPanel = false;
        break;
    case 21:
        //buy xp
        //hostel->BuyXP(alliedParty, 50);
        showRestPanel = false;
        showSelectCharaPanel = true;
        break;
    case 22:
        //back panel
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

        SDL_Rect talkBounds = { 500, 300, 72, 72 };
        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON, 10, "", talkBounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, ownerSprite, 0, talkBounds.w, talkBounds.h
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

    SDL_Rect restBtn = { 400, 300, 72, 72 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 20, "", restBtn,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, ownerSprite, 0, restBtn.w, restBtn.h
    );

    SDL_Rect xpBtn = { 400, 370, 72, 72 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 21, "", xpBtn,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, ownerSprite, 0, xpBtn.w, xpBtn.h
    );

    SDL_Rect backPanel = { 400, 440, 72, 72 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 22, "", backPanel,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, ownerSprite, 0, backPanel.w, backPanel.h
    );
}

void HostelScene::OpenSelectCharaPanel()
{
    LOG("Show select chara panel");

    //character buttons
    for (int i = 0; i < alliedParty->GetMemberCount(); i++) {
        SDL_Rect charaBtn = { 400, 300 + (70*i), 72, 72};
        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON, 30 + i, alliedParty->GetMembers()[i]->GetName().c_str(), charaBtn,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, ownerSprite, 0, charaBtn.w, charaBtn.h
        );
    }


    SDL_Rect backPanel = { 400, 500, 72, 72 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 33, "back", backPanel,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, ownerSprite, 0, backPanel.w, backPanel.h
    );
}
