#include "DialogueScene.h"
#include "DialogueManager.h"
#include "Engine.h"
#include "Scene.h"
#include "UIManager.h"
#include "Textures.h"
#include "Render.h"
#include "Log.h"
#include "SceneUtils.h"

TooltipRenderer tooltip;

DialogueScene::DialogueScene(const std::string& dialogueId,
                             std::function<void()> onFinished)
    : dialogueId(dialogueId)
    , onFinished(onFinished),
    portraitTexture(nullptr),
    button(nullptr),
    pendingRefresh(false)
{
    sceneName = "DialogueScene";
}

DialogueScene::~DialogueScene() {}

void DialogueScene::Load()
{
    tooltipRenderer.isDialogue = true;

    LoadTextures();

    if (!DialogueManager::StartDialogue(dialogueId))
    {
        LOG("DialogueScene: no se pudo iniciar el dialogo '%s'.", dialogueId.c_str());
        Engine::GetInstance().scene->PopScene();
        return;
    }

    RefreshOptionButtons();
}

void DialogueScene::Update(float dt)
{
    Engine::GetInstance().render->camera.x = 0;
    if (portraitTexture != nullptr)
    {
        Engine::GetInstance().render->DrawTexture(portraitTexture, 0, 0);
    }

    if (pendingRefresh) //avoids double clicking buttons
    {
        pendingRefresh = false;
        ClearOptionButtons();
        RefreshOptionButtons();
    }

    // Si el diálogo terminó externamente, cerrar
    if (!DialogueManager::IsActive())
    {
        Engine::GetInstance().scene->PopScene();
    }
}

void DialogueScene::PostUpdate(float dt)
{




    const DialogueNode* node = DialogueManager::GetCurrentNode();
    if (node == nullptr) return;

    // Panel de fondo semitransparente
   // SDL_Rect posPanel = { 0, 530, 1280, 190 };
    //Engine::GetInstance().render->DrawTexture(panel, posPanel.x, posPanel.y);

    // Portrait — si cambió, recargarlo
    //UpdatePortrait(node->portrait);



    // Nombre del personaje
   /* Engine::GetInstance().render->DrawText(
        node->speaker.c_str(),
        155, 540, 300, 30,
        { 255, 215, 0, 255 }   // dorado
    );*/

    // Texto del diálogo
    tooltipRenderer.Draw(node->text.c_str(), 447, 520);
    //tocar valores de tooltip para adaptar texto
    
    //Engine::GetInstance().render->DrawText(
    //    node->text.c_str(),
    //    155, 580, 1100, 80,
    //    { 255, 255, 255, 255 }
    //);
}


void DialogueScene::Unload()
{
    ClearOptionButtons();

    if (portraitTexture != nullptr)
    {
        Engine::GetInstance().textures->UnLoad(portraitTexture);
        portraitTexture    = nullptr;
        currentPortraitPath = "";
    }

    if (onFinished)
    {
        onFinished();
    }
}

void DialogueScene::LoadTextures()
{
    button = Engine::GetInstance().textures->Load("Assets/Textures/CombatScene/emptyButton.png");
    panel = Engine::GetInstance().textures->Load("Assets/Textures/Dialogues/TextBox.png");
    
    std::string portraitPath = SceneUtils::GetPortraitPath(dialogueId);
    if (!portraitPath.empty())
    {
        portraitTexture = Engine::GetInstance().textures->Load(portraitPath.c_str());
        currentPortraitPath = portraitPath;
    }
}


bool DialogueScene::OnUIMouseClickEvent(UIElement* uiElement)
{
    pendingRefresh = true;

    int id = uiElement->id;

    if (id >= OPTION_BTN_BASE)
    {
        int optionIndex = id - OPTION_BTN_BASE;

        LOG("CLICK boton id=%d -> optionIndex=%d", id, optionIndex);

        DialogueManager::ChooseOption(optionIndex);

        if (!DialogueManager::IsActive())
        {
            // Diálogo terminado
            Engine::GetInstance().scene->PopScene();
        }
        else
        {
            // Avanzar al siguiente nodo — refrescar botones
            //ClearOptionButtons();
            //RefreshOptionButtons();
        }
    }

    return true;
}


void DialogueScene::RefreshOptionButtons()
{
    const DialogueNode* node = DialogueManager::GetCurrentNode();
    if (node == nullptr) return;

    if (node->options.empty() || node->isEnd)
    {
        // Nodo final — un solo botón para cerrar
        SDL_Rect bounds = { 1000, 63, 202, 63 };
        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON, OPTION_BTN_BASE, "Continue", bounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, button, 0, bounds.w, bounds.h
        );
        return;
    }

    for (int i = 0; i < (int)node->options.size(); ++i)
    {
        SDL_Rect bounds;
        bounds.x = 50;
        bounds.y = 63 + i * 73;
        bounds.w = 202;
        bounds.h = 63;

        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON, OPTION_BTN_BASE + i,
            node->options[i].text.c_str(), bounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, button, 0, bounds.w, bounds.h
        );
    }
}


void DialogueScene::ClearOptionButtons()
{
    for (auto& e : Engine::GetInstance().uiManager->UIElementsList)
    {
        if (e->id >= OPTION_BTN_BASE)
        {
            e->CleanUp();
        }
    }
}


//void DialogueScene::UpdatePortrait(const std::string& portraitPath)
//{
//    if (portraitPath == currentPortraitPath) return;
//
//    if (portraitTexture != nullptr)
//    {
//        Engine::GetInstance().textures->UnLoad(portraitTexture);
//        portraitTexture = nullptr;
//    }
//
//    if (!portraitPath.empty())
//    {
//        portraitTexture = Engine::GetInstance().textures->Load(portraitPath.c_str());
//    }
//
//    currentPortraitPath = portraitPath;
//}

//void DialogueScene::OnResume()
//{
//    CreateUI();
//}
//
//void DialogueScene::OnPause()
//{
//    Engine::GetInstance().uiManager->CleanUp();
//}
//
//void DialogueScene::CreateUI()
//{
//    CreateCharactersButtons();
//    CreateInterfaceButtons();
//}

