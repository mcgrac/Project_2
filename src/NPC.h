#pragma once
#include <string>

class NPC
{
public:
    NPC(const std::string& name, const std::string& dialogueId);
    ~NPC() = default;

    void Interact();

    inline const std::string& GetName() const { return name; }
    inline const std::string& GetDialogueId() const { return dialogueId; }

private:
    std::string name;
    std::string dialogueId;
};