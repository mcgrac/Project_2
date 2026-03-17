#include "InGameScene.h"
#include "CharacterFactory.h"
#include "Character.h"
#include "Scene.h"
#include "Engine.h"
#include "Audio.h"
#include "Map.h"
#include "EntityManager.h"
#include "UIManager.h"
#include "Log.h"


InGameScene::InGameScene(std::vector<std::string> _characterNames)
    : characterNames(_characterNames)
    , alliedParty(nullptr)
{
}

InGameScene::~InGameScene()
{
    DestroyParty();
}

void InGameScene::Load()
{
    Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/level-iv-339695.wav");
    Engine::GetInstance().map->Load("Assets/Maps/", "MapTemplate.tmx");

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
}

void InGameScene::Update(float dt)
{

}

void InGameScene::PostUpdate(float dt)
{

}

void InGameScene::Unload()
{
    DestroyParty();
    Engine::GetInstance().uiManager->CleanUp();
    Engine::GetInstance().map->CleanUp();
    Engine::GetInstance().entityManager->CleanUp();
}

bool InGameScene::OnUIMouseClickEvent(UIElement* uiElement)
{
    switch (uiElement->id)
    {
    // Add buttons in game
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
