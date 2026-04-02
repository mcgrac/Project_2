#include "InGameScene.h"
#include "CombatScene.h"
#include "islandScene.h"
#include "CharacterFactory.h"
#include "Character.h"
#include "Scene.h"
#include "Engine.h"
#include "Audio.h"
#include "Map.h"
#include "EntityManager.h"
#include "UIManager.h"
#include "Log.h"
#include "Textures.h"
#include "Render.h"
#include "SaveLoad.h"

InGameScene::InGameScene(std::vector<std::string> _characterNames, bool _isContinue)
    : characterNames(_characterNames)
    , alliedParty(nullptr), background(nullptr), isContinue(_isContinue)
{
}

InGameScene::~InGameScene()
{
    DestroyParty();
}

void InGameScene::Load()
{
    Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/level-iv-339695.wav");
    //Engine::GetInstance().map->Load("Assets/Maps/", "MapTemplate.tmx");

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

    // test debug characters info
    for (Character* c : alliedParty->GetMembers())
    {
        c->PrintDebugInfo();
    }

    //load textures
    LoadTextures();

    //buttons creation
    //Botón de iniciar combate
    SDL_Rect combatBtnBounds = { 20, 20, 154, 60 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, 1, "Start Combat", combatBtnBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, spritesheet, 0
    );

    //load world
    worldMap.LoadWorld("Assets/Maps/world.xml");

    //callback when the player arrives to an island->world map notify ingameScene
    worldMap.arrivalIsland = [this](const Island& island) {
        Engine::GetInstance().scene->PushScene(new IslandScene(island, &worldMap, alliedParty));
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
}

void InGameScene::Update(float dt)
{
    //render textures
    Engine::GetInstance().render->DrawTexture(background, 0, 0);

    worldMap.Update(dt);
    worldMap.PostUpdate(dt);
}

void InGameScene::PostUpdate(float dt)
{

}

void InGameScene::Unload()
{
    LOG("InGameScene::Unload LLAMADO — alliedParty: %p", alliedParty);

    //unload worldMap
    worldMap.UnloadWorld();

    DestroyParty();
    Engine::GetInstance().uiManager->CleanUp();
    //Engine::GetInstance().map->CleanUp();
    Engine::GetInstance().entityManager->CleanUp();
}

void InGameScene::LoadTextures(){
    //load background
    background = Engine::GetInstance().textures->Load("Assets/Textures/Backgrounds/IslandsScreen.png");
    spritesheet = Engine::GetInstance().textures->Load("Assets/Textures/UI/buttons2.png");
}

bool InGameScene::OnUIMouseClickEvent(UIElement* uiElement)
{
    switch (uiElement->id)
    {
    // Add buttons in game
    case 1:
        LOG("InGameScene: iniciando combate...");
        // PushScene — InGameScene queda suspendida con todo su estado
        Engine::GetInstance().scene->PushScene(new CombatScene(alliedParty));
        break;
    default:
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
