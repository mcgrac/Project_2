#include "NPC.h"
#include "DialogueManager.h"

NPC::NPC(const std::string& name, const std::string& dialogueId)
    : name(name)
    , dialogueId(dialogueId)
{
}

void NPC::Interact()
{
    DialogueManager::StartDialogue(dialogueId);
}