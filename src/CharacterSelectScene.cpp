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
#include "LoadingScene.h"

CharacterSelectScene::CharacterSelectScene() : 
    background(nullptr), 
    backgroundGerbera(nullptr),
    backgroundIgnis(nullptr),
    backgroundFatuus(nullptr),
    backgroundJochi(nullptr),
    backgroundMarkus(nullptr),
    backgroundTheresia(nullptr),
    spritesheetStartBtn(nullptr), 
    spritesheetCharacters(nullptr),
    backButtonSpritesheet(nullptr),
    panelInformationSpritesheet(nullptr),
    labelSpritesheets(nullptr)
{
    // Registrar personajes disponibles con su nombre de factory y posición en pantalla
    // Añadir una linea nueva por cada personaje
    availableCharacters = {
        { "Gerbera", "Gerbera", 1, 1, Vector2D(800, 100), Vector2D(800, 200)},
        { "Markus", "Markus", 2, 2, Vector2D(800, 100),Vector2D(800, 200)},
        { "Theresia", "Theresia", 3, 3, Vector2D(800, 100), Vector2D(800, 200)},
        { "Jochi", "Jochi", 4, 4, Vector2D(800, 100), Vector2D(800, 200)},
        { "Fatuus", "Fatuus", 5, 5, Vector2D(800, 100), Vector2D(800, 200)},
        { "Ignis", "Ignis", 6, 6, Vector2D(800, 100), Vector2D(800, 200)},
    };

    sceneName = "CharacterSelection";
}

void CharacterSelectScene::Load()
{

    selectedNames.clear();
    for (auto& c : availableCharacters)
    {
        c.selected = false;
    }

    LoadTextures();
    LoadSounds();
    //The scene begins with the tutorial open
    UpdateTutorialUI();
}

void CharacterSelectScene::Update(float dt)
{
 
    if (tutorialOpen) {
        Engine::GetInstance().render->DrawTexture(tutorials[tutorialIndex], 0, 0);
    }
    else {
        if (backName == "Ignis" && switched == false) { Engine::GetInstance().render->DrawTexture(backgroundIgnis, 0, 0); }
        else if (backName == "Gerbera" && switched == false) { Engine::GetInstance().render->DrawTexture(backgroundGerbera, 0, 0); }
        else if (backName == "Jochi" && switched == false) { Engine::GetInstance().render->DrawTexture(backgroundJochi, 0, 0); }
        else if (backName == "Markus" && switched == false) { Engine::GetInstance().render->DrawTexture(backgroundMarkus, 0, 0); }
        else if (backName == "Theresia" && switched == false) { Engine::GetInstance().render->DrawTexture(backgroundTheresia, 0, 0); }
        else if (backName == "Fatuus" && switched == false) { Engine::GetInstance().render->DrawTexture(backgroundFatuus, 0, 0); }
        else if (backName == "Ignis") { Engine::GetInstance().render->DrawTexture(backgroundIgnis1, 0, 0); }
        else if (backName == "Gerbera") { Engine::GetInstance().render->DrawTexture(backgroundGerbera1, 0, 0); }
        else if (backName == "Jochi") { Engine::GetInstance().render->DrawTexture(backgroundJochi1, 0, 0); }
        else if (backName == "Markus") { Engine::GetInstance().render->DrawTexture(backgroundMarkus1, 0, 0); }
        else if (backName == "Theresia") { Engine::GetInstance().render->DrawTexture(backgroundTheresia1, 0, 0); }
        else if (backName == "Fatuus") { Engine::GetInstance().render->DrawTexture(backgroundFatuus1, 0, 0); }
        else { Engine::GetInstance().render->DrawTexture(background, 0, 0); }

        DrawSelectedIndicator();
    }
    
    RenderSelection();
}


void CharacterSelectScene::PostUpdate(float dt) {

}


void CharacterSelectScene::Unload()
{
    // Si el usuario vuelve atrás sin confirmar, destruir los personajes creados
    for (SelectableCharacter& c : availableCharacters)
    {
        if (c.chara != nullptr)
        {
            delete c.chara;
            c.chara = nullptr;
        }
    }

    UnloadTextures();
    Engine::GetInstance().uiManager->CleanUp();
    selectedNames.clear();
}

void CharacterSelectScene::LoadTextures(){

    spritesheetStartBtn = Engine::GetInstance().textures->Load("Assets/Textures/CharacterSelectScene/StartButton.png");
    background = Engine::GetInstance().textures->Load("Assets/Textures/CharacterSelectScene/CharacterSelectionBackground.png");
    backgroundGerbera = Engine::GetInstance().textures->Load("Assets/Textures/CharacterSelectScene/CharacterSelectionBackgroundGerbera.png");
    backgroundTheresia = Engine::GetInstance().textures->Load("Assets/Textures/CharacterSelectScene/CharacterSelectionBackgroundTheresia.png");
    backgroundMarkus = Engine::GetInstance().textures->Load("Assets/Textures/CharacterSelectScene/CharacterSelectionBackgroundMarkus.png");
    backgroundJochi = Engine::GetInstance().textures->Load("Assets/Textures/CharacterSelectScene/CharacterSelectionBackgroundJochi.png");
    backgroundFatuus = Engine::GetInstance().textures->Load("Assets/Textures/CharacterSelectScene/CharacterSelectionBackgroundFatuus.png");
    backgroundIgnis = Engine::GetInstance().textures->Load("Assets/Textures/CharacterSelectScene/CharacterSelectionBackgroundIgnis.png");

    backgroundGerbera1 = Engine::GetInstance().textures->Load("Assets/Textures/CharacterSelectScene/CharacterSelectionBackgroundGerbera1.png");
    backgroundTheresia1 = Engine::GetInstance().textures->Load("Assets/Textures/CharacterSelectScene/CharacterSelectionBackgroundTheresia1.png");
    backgroundMarkus1 = Engine::GetInstance().textures->Load("Assets/Textures/CharacterSelectScene/CharacterSelectionBackgroundMarkus1.png");
    backgroundJochi1 = Engine::GetInstance().textures->Load("Assets/Textures/CharacterSelectScene/CharacterSelectionBackgroundJochi1.png");
    backgroundFatuus1 = Engine::GetInstance().textures->Load("Assets/Textures/CharacterSelectScene/CharacterSelectionBackgroundFatuus1.png");
    backgroundIgnis1 = Engine::GetInstance().textures->Load("Assets/Textures/CharacterSelectScene/CharacterSelectionBackgroundIgnis1.png");

    spritesheetCharacters = Engine::GetInstance().textures->Load("Assets/Textures/CharacterSelectScene/CharacterButtons.png");
    panelInformationSpritesheet = Engine::GetInstance().textures->Load("Assets/Textures/CharacterSelectScene/Panel.png");
    labelSpritesheets = Engine::GetInstance().textures->Load("Assets/Textures/CharacterSelectScene/CharacterNamePlate.png");
    backButtonSpritesheet = Engine::GetInstance().textures->Load("Assets/Textures/CharacterSelectScene/BackButton.png");
    switchButton = Engine::GetInstance().textures->Load("Assets/Textures/CharacterSelectScene/SwitchButton.png");

    tutorialOpenButton = Engine::GetInstance().textures->Load("Assets/Textures/CharacterSelectScene/tutorialOpenButton.png");
    tutorialLeftButton = Engine::GetInstance().textures->Load("Assets/Textures/CharacterSelectScene/left.png");
    tutorialRightButton = Engine::GetInstance().textures->Load("Assets/Textures/CharacterSelectScene/right.png");


   
    for (int i = 0; i < 6; i++) {
        std::string path = "Assets/Textures/CharacterSelectScene/tutorial";
        std::string index = std::to_string(i);
        tutorials.push_back(Engine::GetInstance().textures->Load((path + index + ".png").c_str())) ;
    }
}


void CharacterSelectScene::UnloadTextures()
{
    Engine::GetInstance().textures->UnLoad(background);
    Engine::GetInstance().textures->UnLoad(spritesheetStartBtn);
    Engine::GetInstance().textures->UnLoad(spritesheetCharacters);
    Engine::GetInstance().textures->UnLoad(panelInformationSpritesheet);
    Engine::GetInstance().textures->UnLoad(labelSpritesheets);
    Engine::GetInstance().textures->UnLoad(backButtonSpritesheet);
    Engine::GetInstance().textures->UnLoad(switchButton);
    //descriptions
    Engine::GetInstance().textures->UnLoad(backgroundGerbera);
    Engine::GetInstance().textures->UnLoad(backgroundTheresia);
    Engine::GetInstance().textures->UnLoad(backgroundIgnis);
    Engine::GetInstance().textures->UnLoad(backgroundJochi);
    Engine::GetInstance().textures->UnLoad(backgroundFatuus);
    Engine::GetInstance().textures->UnLoad(backgroundMarkus);
    //stats
    Engine::GetInstance().textures->UnLoad(backgroundGerbera1);
    Engine::GetInstance().textures->UnLoad(backgroundTheresia1);
    Engine::GetInstance().textures->UnLoad(backgroundIgnis1);
    Engine::GetInstance().textures->UnLoad(backgroundJochi1);
    Engine::GetInstance().textures->UnLoad(backgroundFatuus1);
    Engine::GetInstance().textures->UnLoad(backgroundMarkus1);
    //tutorial
    for (int i = 0; i < 6; i++) { Engine::GetInstance().textures->UnLoad(tutorials[i]); }
    Engine::GetInstance().textures->UnLoad(tutorialOpenButton);
    Engine::GetInstance().textures->UnLoad(tutorialLeftButton);
    Engine::GetInstance().textures->UnLoad(tutorialRightButton);

}

void CharacterSelectScene::LoadSounds() {
    buttonPress = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/UIfx/button_press.wav");
}

bool CharacterSelectScene::OnUIMouseClickEvent(UIElement* uiElement)
{
    //button pressed sound
    Engine::GetInstance().audio->PlayFx(buttonPress);

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
    case 9:
        switched = !switched;
        break;
    case 10:
        tutorialOpen = !tutorialOpen;
        UpdateTutorialUI();
        break;
    case 11:
        if (tutorialIndex > 0) { tutorialIndex--; }
        break;
    case 12:
        if (tutorialIndex < 5) { tutorialIndex++; }
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

        //delete c.chara;
        //c.chara = nullptr;

        SetPortraitButtonStateNormal(index);
        LOG("CharacterSelect: %s deseleccionado.", c.name.c_str());


        //delete character
        //for (int i = 0; i < createdCharacters.size(); i++) {
        //    if (createdCharacters[i] != nullptr) {
        //        //if selected character name is equal to the name of the character in the vector
        //        if (createdCharacters[i]->GetName() == c.name) {
        //            delete createdCharacters[i];
        //            createdCharacters.erase(createdCharacters.begin() + i);
        //        }
        //    }
        //}
    }
    else
    {
        if (HasFullTeam())
        {
            LOG("CharacterSelect: ya tienes 3 personajes seleccionados.");
            return;
        }

        //Character* created = CharacterFactory::Create(c.name);
        //if (created == nullptr)
        //{
        //    LOG("CharacterSelect: no se pudo crear el personaje '%s'.", c.name.c_str());
        //    return;
        //}

        //c.chara = created;
        c.selected = true;
        selectedNames.push_back(c.name);
        SetPortraitButtonStatePressed(index);
        LOG("CharacterSelect: %s seleccionado (%d/3).", c.name.c_str(), (int)selectedNames.size());

        //create character
        //LOG("Creando personaje: '%s'", c.name.c_str());
        //Character* chara = CharacterFactory::Create(c.name);
        //if (chara != nullptr)
        //{
        //    //alliedParty->AddMember(c);
        //    //add character to the vector
        //    createdCharacters.push_back(chara);
        //}
        //else
        //{
        //    LOG("InGameScene::Load — no se pudo crear el personaje '%s'.", chara->GetName().c_str());
        //}
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
          

            backName = c.name;
        }
    }

  
}

void CharacterSelectScene::ConfirmSelection()
{
    //std::vector<Character*> selectedCharacters;

    //for (SelectableCharacter& c : availableCharacters)
    //{
    //    if (c.selected && c.chara != nullptr)
    //    {
    //        selectedCharacters.push_back(c.chara);
    //        c.chara = nullptr; // InGameScene es ahora owner, no borrar en Unload
    //    }
    //}

#if _DEBUG
    LOG("CharacterSelect: confirmado, lanzando LoadingScene.");
    LOG("Nombres seleccionados: %d", (int)selectedNames.size());
    for (const std::string& n : selectedNames)
    {
        LOG("  -> '%s'", n.c_str());
    }
#endif // _DEBUG

    //Engine::GetInstance().scene->ReplaceScene(new InGameScene(selectedNames, false));
    Engine::GetInstance().scene->ReplaceScene(new LoadingScene(selectedNames, false));
}

void CharacterSelectScene::CreateCharactersButtons()
{
    //for (int i = 0; i < availableCharacters.size(); i++) {
    //    const SelectableCharacter& c = availableCharacters[i];
    //    Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 0 + i, "", c.bounds,
    //        [this](UIElement* e) {return this->OnUIMouseClickEvent(e); }, {}, spritesheetCharacters, c.portraitCol, 195, 306);
    //}

    //gerbera selection
    SDL_Rect btPos1 = { 81, 60, 195, 306 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 2, "", btPos1,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, spritesheetCharacters, 0, btPos1.w, btPos1.h
    );

    //Markus selection
    SDL_Rect btPos2 = { 330, 60, 195, 306 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 3, "", btPos2,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, spritesheetCharacters, 1, btPos2.w, btPos2.h
    );

    //theresia selection
    SDL_Rect btPos3 = { 581, 60, 195, 306 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 4, "", btPos3,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, spritesheetCharacters, 2, btPos3.w, btPos3.h
    );

    //joochi selection
    SDL_Rect btPos4 = { 81, 391, 195, 306 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 5, "", btPos4,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, spritesheetCharacters, 3, btPos4.w, btPos4.h
    );

    //fatuus selection
    SDL_Rect btPos5 = { 330, 391, 195, 306 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 6, "", btPos5,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, spritesheetCharacters, 4, btPos5.w, btPos5.h
    );

    //ignis selection
    SDL_Rect btPos6 = { 581, 391, 195, 306 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 7, "", btPos6,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, spritesheetCharacters, 5, btPos6.w, btPos6.h
    );
}

void CharacterSelectScene::CreateInterfaceButtons()
{
    SDL_Rect confirmBounds = { 1132, 643, 125, 72 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 1, "", confirmBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, spritesheetStartBtn, 0, confirmBounds.w, confirmBounds.h
    );

    SDL_Rect backButton = { 4, 5, 72, 72 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 8, "", backButton,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, backButtonSpritesheet, 0, backButton.w, backButton.h
    );

    SDL_Rect switchButtonBounds = { 842, 13, 72, 72 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 9, "", switchButtonBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, switchButton, 0, switchButtonBounds.w, switchButtonBounds.h
    );

    //OpenTutorial
    SDL_Rect openBounds = { 4, 85, 72, 72 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 10, "tutorial", openBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, tutorialOpenButton, 0, openBounds.w, openBounds.h
    );
}

void CharacterSelectScene::DrawSelectedIndicator()
{
    SDL_Color White = { 255, 255, 255 };
    SDL_Color Yellow = { 255, 255, 0 };
    int selected = selectedNames.size();

    std::string selectedStr = std::to_string(selected);

    if (selected==3) { Engine::GetInstance().render->DrawText(("Selected: " + selectedStr + "/3").c_str(), 915, 663, 206, 33, Yellow); }
    else{ Engine::GetInstance().render->DrawText(("Selected: " + selectedStr + "/3").c_str(), 915, 663, 206, 33, White); }

    
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

void CharacterSelectScene::OnResume()
{
    CreateUI();
}

void CharacterSelectScene::OnPause()
{
    Engine::GetInstance().uiManager->CleanUp();
}

void CharacterSelectScene::CreateUI()
{
    CreateCharactersButtons();
    CreateInterfaceButtons();
}

void CharacterSelectScene::UpdateTutorialUI()
{
    Engine::GetInstance().uiManager->RemoveElementsByRange(0, 20);

    if (tutorialOpen == true) {
        

        //OpenTutorial
        SDL_Rect openBounds = { 4, 85, 72, 72 };
        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON, 10, "tutorial", openBounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, tutorialOpenButton, 0, openBounds.w, openBounds.h
        );

        //Left
        SDL_Rect leftBounds = { 29, 339, 42, 42 };
        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON, 11, "left", leftBounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, tutorialLeftButton, 0, leftBounds.w, leftBounds.h
        );

        //Right
        SDL_Rect rightBounds = { 1209, 339, 42, 42 };
        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON, 12, "right", rightBounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, tutorialRightButton, 0, rightBounds.w, rightBounds.h
        );
    }
    else {

        Engine::GetInstance().uiManager->RemoveElementsByRange(0, 20);
        CreateUI();
    }

}
