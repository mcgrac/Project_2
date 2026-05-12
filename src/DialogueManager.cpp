#include "DialogueManager.h"
#include "pugixml.hpp"
#include "Log.h"


std::unordered_map<std::string, Dialogue> DialogueManager::dialogues;
std::string DialogueManager::activeDialogueId = "";
int DialogueManager::currentNodeId = -1;
bool DialogueManager::active = false;
std::string DialogueManager::lastChoiceTag = "";

bool DialogueManager::LoadDialogues(const std::string& xmlPath)
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(xmlPath.c_str());

    if (!result)
    {
        LOG("DialogueManager: no se pudo cargar '%s': %s",
            xmlPath.c_str(), result.description());
        return false;
    }

    pugi::xml_node root = doc.child("dialogues");

    for (pugi::xml_node dialogueNode : root.children("dialogue"))
    {
        Dialogue dialogue;
        dialogue.id = dialogueNode.attribute("id").as_string();

        for (pugi::xml_node nodeXml : dialogueNode.children("node"))
        {
            DialogueNode node;
            node.id = nodeXml.attribute("id").as_int();
            node.speaker = nodeXml.attribute("speaker").as_string();
            node.portrait = nodeXml.attribute("portrait").as_string();
            node.text = nodeXml.attribute("text").as_string();
            node.isEnd = nodeXml.attribute("end").as_bool(false);

            for (pugi::xml_node optionXml : nodeXml.children("option"))
            {
                DialogueOption option;
                option.text = optionXml.attribute("text").as_string();
                option.nextNode = optionXml.attribute("nextNode").as_int(-1);
                option.action = optionXml.attribute("action").as_string();
                node.options.push_back(option);
            }

            dialogue.nodes[node.id] = node;
        }

        dialogues[dialogue.id] = dialogue;

        LOG("DialogueManager: dialogo '%s' cargado con %d nodos.",
            dialogue.id.c_str(), (int)dialogue.nodes.size());
    }

    LOG("DialogueManager: %d dialogos cargados en total.", (int)dialogues.size());
    return true;
}


bool DialogueManager::StartDialogue(const std::string& dialogueId)
{
    auto it = dialogues.find(dialogueId);
    if (it == dialogues.end())
    {
        LOG("DialogueManager: dialogo '%s' no encontrado.", dialogueId.c_str());
        return false;
    }

    activeDialogueId = dialogueId;
    currentNodeId = 0;
    active = true;
    lastChoiceTag = "";

    LOG("DialogueManager: iniciando dialogo '%s'.", dialogueId.c_str());
    return true;
}


void DialogueManager::ChooseOption(int optionIndex)
{

    if (!active) return;

    const DialogueNode* node = GetCurrentNode();
    if (node == nullptr) return;

    // Si el nodo es final (con o sin opciones), cerrar
    if (node->isEnd)
    {
        EndDialogue();
        return;
    }

    // Nodo sin opciones también cierra
    if (node->options.empty())
    {
        EndDialogue();
        return;
    }

    if (optionIndex < 0 || optionIndex >= (int)node->options.size()) return;

    // Guardar tag de la opción elegida si es que existe
    if (!node->options[optionIndex].action.empty())
    {
        DialogueManager::lastChoiceTag = node->options[optionIndex].action;
    }

    int nextNode = node->options[optionIndex].nextNode;

    // nextNode negativo = cerrar
    if (nextNode < 0)
    {
        EndDialogue();
        return;
    }

    currentNodeId = nextNode;
    LOG("DialogueManager: avanzando al nodo %d.", currentNodeId);
}

void DialogueManager::EndDialogue()
{
    LOG("DialogueManager: dialogo '%s' terminado.", activeDialogueId.c_str());
    activeDialogueId = "";
    currentNodeId = -1;
    active = false;
}

//helpers
bool DialogueManager::IsActive()
{
    return active;
}

const DialogueNode* DialogueManager::GetCurrentNode()
{
    if (!active) return nullptr;

    auto it = dialogues.find(activeDialogueId);
    if (it == dialogues.end()) return nullptr;

    auto nodeIt = it->second.nodes.find(currentNodeId);
    if (nodeIt == it->second.nodes.end()) return nullptr;

    return &nodeIt->second;
}

bool DialogueManager::IsCurrentNodeEnd()
{
    const DialogueNode* node = GetCurrentNode();
    if (node == nullptr) return true;
    return node->isEnd || node->options.empty();
}