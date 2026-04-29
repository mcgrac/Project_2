#include "NPC.h"
#include "DialogueManager.h"
#include "Engine.h"
#include "Textures.h"
#include "Island.h"
#include "Log.h"

NPC::NPC(const std::string& _name, const std::string& _dialogueId, IslandFaction _faction, std::string _building)
    : name(_name)
    , dialogueId(_dialogueId)
    , faction(_faction)
    , building(_building)
    , spritesheet(nullptr)
{
    LOG("Loading texture NPC...");
    LoadTexture();
    if (spritesheet)
    {
        LOG("Texture loading succesfull");
    }
}

NPC::~NPC()
{
    Cleanup();
}

void NPC::Cleanup()
{
    //unload textures
    if (spritesheet)
    {
        Engine::GetInstance().textures->UnLoad(spritesheet);
        spritesheet = nullptr;
    }
}

void NPC::Interact()
{
    DialogueManager::StartDialogue(dialogueId);
}

void NPC::LoadTexture()
{
    //load texture depending on the NPC
    switch (faction)
    {
    case IslandFaction::HUMANS:

        if (building == "Shop") {
            spritesheet = Engine::GetInstance().textures->Load("Assets/Textures/HumanIsland/Shop/ShopOwner.png");
        }
        else if (building == "Hostel") {
            spritesheet = Engine::GetInstance().textures->Load("Assets/Textures/HumanIsland/Hostel/HostelOwner.png");
        }
        else if (building == "Dockyard") {
            spritesheet = Engine::GetInstance().textures->Load("Assets/Textures/HumanIsland/Dock/DockOwner.png");
        }
        else {
            LOG("ERROR: no building found");
        }

        break;
    case IslandFaction::REPTILES:

        if (building == "Shop") {
            spritesheet = Engine::GetInstance().textures->Load("Assets/Textures/HumanIsland/Shop/ShopOwner.png");
        }
        else if (building == "Hostel") {
            spritesheet = Engine::GetInstance().textures->Load("Assets/Textures/HumanIsland/Hostel/HostelOwner.png");
        }
        else if (building == "Dockyard") {
            spritesheet = Engine::GetInstance().textures->Load("Assets/Textures/HumanIsland/Dock/DockOwner.png");
        }
        else {
            LOG("ERROR: no building found");
        }

        break;

    default:
        break;
    }
}
