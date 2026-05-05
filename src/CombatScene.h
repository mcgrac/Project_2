#pragma once
#include "BaseScene.h"
#include "Combat.h"
#include "Party.h"

struct SDL_Texture;

class CombatScene : public BaseScene
{
public:

    CombatScene(Party* allied, int _shipLevel);
    ~CombatScene();

    void Load() override;
    void Update(float dt) override;
    void PostUpdate(float dt) override;
    void Unload() override;
    void LoadTextures() override;

    bool OnUIMouseClickEvent(UIElement* uiElement) override;

    //helpers
    void OnResume() override;
    void OnPause() override;
    void CreateUI();

private:
    Party* alliedParty;
    Party* enemyParty;
    Combat* combat;

    bool combatFinished;

    void CreateEnemyParty();
    void DestroyEnemyParty();

    SDL_Texture* background;

    //combat UI
    enum class CombatUIState
    {
        HIDDEN,
        SELECTING_LANE,     // pre-combat: player assigns each character to a lane
        SELECTING_SKILL,
        SELECTING_TARGET
    };

    CombatUIState uiState = CombatUIState::HIDDEN;
    int selectedSkillIdx = -1;

    void UpdateCombatUI();
    void ShowSkillButtons();
    void ShowTargetPanel();
    void HideCombatUI();
    void ShowCurrentHP();

    // ── Lane selection ───────────────────────────────────────────────────────
    // Tracks which lane each allied character has been assigned to.
    // Key = character pointer, Value = assigned LaneType.
    // Built up one character at a time during SELECTING_LANE phase.
    std::unordered_map<Character*, LaneType> laneAssignments;

    // Index of the allied character currently being assigned a lane (0, 1, 2)
    int laneAssignmentCursor = 0;

    // Prevents the same click from propagating to the next lane panel in the same frame
    bool laneInputConsumed;
    bool combatInputConsumed = false;

    int shipLevel;

    void ShowLaneSelectionFor(int characterIndex);
    void FinalizeLaneAssignments();
    bool IsLaneTaken(LaneType laneType) const;

    void CreateSkillButtons(Character* c);

    SDL_Texture* abilityIcons;

    SDL_Texture* abilityIcons2;

    int hoveredSkillIdx = -1;

    void DrawSkillTooltip();
    void UpdateSkillHover();
    void DrawColoredLine(const std::string& line, int x, int y);
    std::vector<std::string> WrapText(const std::string& text, int maxCharsPerLine);

};
