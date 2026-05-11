#pragma once
#include <string>
#include <vector>
#include <unordered_map>

// An asnwer option inside a dialogue node
struct DialogueOption
{
    std::string action;

    std::string text; //button text
    int nextNode;   // id next node
};

// A single dialogue node
struct DialogueNode
{
    int id;
    std::string speaker;    // name
    std::string portrait;   // path portrait
    std::string text;       // text dialogue
    bool isEnd;      // true if dialogue ends
    std::vector<DialogueOption> options;    // answer options
};

// Complete dialogue (many nodes together)
struct Dialogue
{
    std::string id;
    std::unordered_map<int, DialogueNode> nodes;  // id -> node
};

//static
class DialogueManager
{
public:

    static bool LoadDialogues(const std::string& xmlPath);
    static bool StartDialogue(const std::string& dialogueId);
    static void ChooseOption(int optionIndex);
    static void EndDialogue();

    // helpers
    static bool IsActive();
    static const DialogueNode* GetCurrentNode();
    static bool IsCurrentNodeEnd();

    static const inline std::string GetLastChoiceTag() { return lastChoiceTag;}

private:
    DialogueManager() = delete;

    static std::unordered_map<std::string, Dialogue> dialogues;
    static std::string activeDialogueId;
    static int currentNodeId;
    static bool active;
    static std::string lastChoiceTag;
};