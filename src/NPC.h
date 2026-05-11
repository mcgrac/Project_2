#pragma once
#include <string>
#include "Island.h"

struct SDL_Texture;

class NPC
{
public:
    NPC(const std::string& _name, const std::string& _dialogueId, IslandFaction _faction, std::string _building);
    ~NPC();

    void Cleanup();

    void Interact();

    inline const std::string& GetName() const { return name; }
    inline const std::string& GetDialogueId() const { return dialogueId; }
    inline SDL_Texture* GetTexture() { return spritesheet; }

    void LoadTexture();

private:
    std::string name;
    std::string dialogueId;

    IslandFaction faction;
    std::string building;

    SDL_Texture* spritesheet;
};