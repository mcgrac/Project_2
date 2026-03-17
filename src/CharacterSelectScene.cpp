#include "CharacterSelectScene.h"
#include "InGameScene.h"
#include "Scene.h"
#include "Engine.h"
#include "Audio.h"
#include "Input.h"
#include "UIManager.h"
#include "Log.h"


CharacterSelectScene::CharacterSelectScene()
{
    // Registrar personajes disponibles con su nombre de factory y posición en pantalla
    // Añade una línea por cada personaje nuevo que crees
    availableCharacters = {
        { "Markus", "Markus",  { 200, 300, 120, 160 } },
        { "Theresia", "Theresia",   { 440, 300, 120, 160 } },
        { "Gerbera", "Gerbera", { 680, 300, 120, 160 } },
    };
}

void CharacterSelectScene::Load()
{
    selectedNames.clear();
    for (auto& c : availableCharacters)
    {
        c.selected = false;
    }

    SDL_Rect confirmBounds = { 460, 540, 160, 40 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, CONFIRM_BUTTON_ID, "Confirmar", confirmBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }
    );

}

void CharacterSelectScene::Update(float dt)
{
    // Detectar click del ratón sobre los personajes
    if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
    {
        Vector2D mousePos = Engine::GetInstance().input->GetMousePosition();
        HandleCharacterClick((int)mousePos.getX(), (int)mousePos.getY());
    }

    RenderSelection();
}


void CharacterSelectScene::PostUpdate(float dt) {

}


void CharacterSelectScene::Unload()
{
    Engine::GetInstance().uiManager->CleanUp();
    selectedNames.clear();
}

void CharacterSelectScene::LoadTextures(){

}


bool CharacterSelectScene::OnUIMouseClickEvent(UIElement* uiElement)
{
    switch (uiElement->id)
    {
    case CONFIRM_BUTTON_ID:
        if (HasFullTeam())
            ConfirmSelection();
        else
            LOG("CharacterSelect: select 3 characters to continue.");
        break;
    default:
        break;
    }
    return true;
}

void CharacterSelectScene::HandleCharacterClick(int mouseX, int mouseY)
{
    for (int i = 0; i < (int)availableCharacters.size(); ++i)
    {
        const SDL_Rect& b = availableCharacters[i].bounds;
        bool insideX = mouseX > b.x && mouseX < b.x + b.w;
        bool insideY = mouseY > b.y && mouseY < b.y + b.h;

        if (insideX && insideY)
        {
            ToggleSelection(i);
            return;
        }
    }
}


void CharacterSelectScene::ToggleSelection(int index)
{
    SelectableCharacter& c = availableCharacters[index];

    if (c.selected)
    {
        c.selected = false;
        auto it = std::find(selectedNames.begin(), selectedNames.end(), c.name);
        if (it != selectedNames.end())
            selectedNames.erase(it);
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
        LOG("CharacterSelect: %s seleccionado (%d/3).", c.name.c_str(), (int)selectedNames.size());
    }
}

//render depending on the state (selected or not)
void CharacterSelectScene::RenderSelection()
{
    for (const auto& c : availableCharacters)
    {
        SDL_Color color;
        if (c.selected)
        {
            color = { 0, 200, 0, 255 };
        }
        else
        {
            color = { 0, 0, 200, 255 };
        }

        Engine::GetInstance().render->DrawRectangle(c.bounds, color.r, color.g, color.b, color.a, true, false);
        Engine::GetInstance().render->DrawText(
            c.label.c_str(),
            c.bounds.x, c.bounds.y + c.bounds.h + 5,
            c.bounds.w, 20,
            { 255, 255, 255, 255 }
        );
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
    Engine::GetInstance().scene->ReplaceScene(new InGameScene(selectedNames));
}
