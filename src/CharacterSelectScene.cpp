#include "CharacterSelectScene.h"
#include "MainMenuScene.h"
#include "InGameScene.h"
#include "Scene.h"
#include "Engine.h"
#include "Audio.h"
#include "Input.h"
#include "UIManager.h"
#include "Log.h"
#include "Render.h"
#include "Textures.h"


CharacterSelectScene::CharacterSelectScene() : 
    background(nullptr), 
    spritesheetStartBtn(nullptr), 
    spritesheetCharacters(nullptr),
    backButtonSpritesheet(nullptr),
    panelInformationSpritesheet(nullptr),
    labelSpritesheets(nullptr)
{
    // Registrar personajes disponibles con su nombre de factory y posición en pantalla
    // Añade una línea por cada personaje nuevo que crees
    availableCharacters = {
        { "Gerbera", "Gerbera", 1, 1, Vector2D(800, 100), Vector2D(800, 200)},
        { "Markus", "Markus", 2, 2, Vector2D(800, 100),Vector2D(800, 200)},
        { "Theresia", "Theresia", 3, 3, Vector2D(800, 100), Vector2D(800, 200)},
    };
}

void CharacterSelectScene::Load()
{
    selectedNames.clear();
    for (auto& c : availableCharacters)
    {
        c.selected = false;
    }

    LoadTextures();
    CreateCharactersButtons();
    CreateInterfaceButtons();
}

void CharacterSelectScene::Update(float dt)
{
    Engine::GetInstance().render->DrawTexture(background, 0, 0);

    RenderSelection();
}


void CharacterSelectScene::PostUpdate(float dt) {

}


void CharacterSelectScene::Unload()
{
    UnloadTextures();

    Engine::GetInstance().uiManager->CleanUp();
    selectedNames.clear();
}

void CharacterSelectScene::LoadTextures(){

    spritesheetStartBtn = Engine::GetInstance().textures->Load("Assets/Textures/CharacterSelectScene/StartButton.png");
    background = Engine::GetInstance().textures->Load("Assets/Textures/CharacterSelectScene/CharacterSelectionBackground.png");
    spritesheetCharacters = Engine::GetInstance().textures->Load("Assets/Textures/CharacterSelectScene/CharacterButtons.png");
    panelInformationSpritesheet = Engine::GetInstance().textures->Load("Assets/Textures/CharacterSelectScene/Panel.png");
    labelSpritesheets = Engine::GetInstance().textures->Load("Assets/Textures/CharacterSelectScene/CharacterNamePlate.png");
    backButtonSpritesheet = Engine::GetInstance().textures->Load("Assets/Textures/CharacterSelectScene/BackButton.png");
}


void CharacterSelectScene::UnloadTextures()
{
    Engine::GetInstance().textures->UnLoad(background);
    Engine::GetInstance().textures->UnLoad(spritesheetStartBtn);
    Engine::GetInstance().textures->UnLoad(spritesheetCharacters);
    Engine::GetInstance().textures->UnLoad(panelInformationSpritesheet);
    Engine::GetInstance().textures->UnLoad(labelSpritesheets);
    Engine::GetInstance().textures->UnLoad(backButtonSpritesheet);
}

bool CharacterSelectScene::OnUIMouseClickEvent(UIElement* uiElement)
{
    switch (uiElement->id)
    {
    case 1:
        if (HasFullTeam())
        {
            ConfirmSelection();
        }
        else
        {
            LOG("CharacterSelect: select 3 characters to continue.");
        }
        break;
    case 8:
        Engine::GetInstance().scene->ReplaceScene(new MainMenuScene());
        break;
    default:

        if (uiElement->id >= 2 && uiElement->id < 2 + availableCharacters.size()) {
            ToggleSelection(uiElement->id - 2);
        }
        break;
    }
    return true;
}

void CharacterSelectScene::ToggleSelection(int index)
{
    SelectableCharacter& c = availableCharacters[index];

    if (c.selected)
    {
        c.selected = false;
        auto it = std::find(selectedNames.begin(), selectedNames.end(), c.name);
        if (it != selectedNames.end())
        {
            selectedNames.erase(it);
        }

        SetPortraitButtonStateNormal(index);
        LOG("CharacterSelect: %s deseleccionado.", c.name.c_str());
    }
    else
    {
        if (HasFullTeam())
        {
            LOG("CharacterSelect: ya tienes 3 personajes seleccionados.");
            return;
        }
        c.selected = true;
        selectedNames.push_back(c.name);
        SetPortraitButtonStatePressed(index);
        LOG("CharacterSelect: %s seleccionado (%d/3).", c.name.c_str(), (int)selectedNames.size());
    }
}

//render depending on the state (selected or not)
void CharacterSelectScene::RenderSelection()
{
    for (int i = 0; i < (int)availableCharacters.size(); ++i)
    {
        const SelectableCharacter& c = availableCharacters[i];

        if (IsPortraitHoveredOrSelected(i))
        {
            if (labelSpritesheets != nullptr)
            {
                SDL_Rect labelRect;
                labelRect.x = 0;
                labelRect.y = (c.labelRow - 1) * LABEL_FRAME_H;
                labelRect.w = LABEL_FRAME_W;
                labelRect.h = LABEL_FRAME_H;

                Engine::GetInstance().render->DrawTexture(
                    labelSpritesheets,
                    (int)c.labelPos.getX(),
                    (int)c.labelPos.getY(),
                    &labelRect
                );
            }

            if (panelInformationSpritesheet != nullptr)
            {
                SDL_Rect panelRect;
                panelRect.x = 0;
                panelRect.y = (c.panelRow - 1) * PANEL_FRAME_H;
                panelRect.w = PANEL_FRAME_W;
                panelRect.h = PANEL_FRAME_H;

                Engine::GetInstance().render->DrawTexture(
                    panelInformationSpritesheet,
                    (int)c.panelPos.getX(),
                    (int)c.panelPos.getY(),
                    &panelRect
                );
            }
        }
    }

    std::string counter = "Seleccionados: " + std::to_string(selectedNames.size()) + " / 3";
    Engine::GetInstance().render->DrawText(counter.c_str(), 400, 240, 280, 30, { 255, 255, 0, 255 });
}

void CharacterSelectScene::ConfirmSelection()
{
    LOG("CharacterSelect: confirmado, lanzando InGameScene.");
    LOG("Nombres seleccionados: %d", (int)selectedNames.size());
    for (const std::string& n : selectedNames)
    {
        LOG("  -> '%s'", n.c_str());
    }

    // Pasa el vector de nombres a InGameScene
    Engine::GetInstance().scene->ReplaceScene(new InGameScene(selectedNames, false));
}

void CharacterSelectScene::CreateCharactersButtons()
{
    //for (int i = 0; i < availableCharacters.size(); i++) {
    //    const SelectableCharacter& c = availableCharacters[i];
    //    Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 0 + i, "", c.bounds,
    //        [this](UIElement* e) {return this->OnUIMouseClickEvent(e); }, {}, spritesheetCharacters, c.portraitCol, 195, 306);
    //}

    //gerbera selection
    SDL_Rect btPos1 = { 100, 50, 195, 306 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 2, "", btPos1,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, spritesheetCharacters, 0, btPos1.w, btPos1.h
    );

    //Markus selection
    SDL_Rect btPos2 = { 300, 50, 195, 306 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 3, "", btPos2,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, spritesheetCharacters, 1, btPos2.w, btPos2.h
    );

    //theresia selection
    SDL_Rect btPos3 = { 500, 50, 195, 306 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 4, "", btPos3,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, spritesheetCharacters, 2, btPos3.w, btPos3.h
    );

    //joochi selection
    SDL_Rect btPos4 = { 100, 400, 195, 306 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 5, "", btPos4,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, spritesheetCharacters, 3, btPos4.w, btPos4.h
    );

    //fatuus selection
    SDL_Rect btPos5 = { 300, 400, 195, 306 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 6, "", btPos5,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, spritesheetCharacters, 4, btPos5.w, btPos5.h
    );

    //ignis selection
    SDL_Rect btPos6 = { 500, 400, 195, 306 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 7, "", btPos6,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, spritesheetCharacters, 5, btPos6.w, btPos6.h
    );
}

void CharacterSelectScene::CreateInterfaceButtons()
{
    SDL_Rect confirmBounds = { 900, 600, 125, 72 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 1, "", confirmBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, spritesheetStartBtn, 0, confirmBounds.w, confirmBounds.h
    );

    SDL_Rect backButton = { 20, 20, 72, 72 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 8, "", backButton,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, backButtonSpritesheet, 0, backButton.w, backButton.h
    );
}

void CharacterSelectScene::SetPortraitButtonStatePressed(int index)
{
    int targetId = 2 + index;
    for (auto& element : Engine::GetInstance().uiManager->UIElementsList) {
        if (element->id == targetId) {
            element->state = UIElementState::SELECTED;
            LOG("Changed id button %d to focused", element->id);
            return;
        }
    }
}

void CharacterSelectScene::SetPortraitButtonStateNormal(int index)
{
    int targetId = 2 + index;
    for (auto& element : Engine::GetInstance().uiManager->UIElementsList) {
        if (element->id == targetId) {
            element->state = UIElementState::NORMAL;
            LOG("Changed id button %d to normal", element->id);
            return;
        }
    }
}

bool CharacterSelectScene::IsPortraitHoveredOrSelected(int index) const
{
    int targetId = 2 + index;  // tus botones de portrait empiezan en id 2
    for (const auto& element : Engine::GetInstance().uiManager->UIElementsList)
    {
        if (element->id == targetId)
        {
            return element->state == UIElementState::FOCUSED
                || element->state == UIElementState::PRESSED;
        }
    }
    return false;
}
