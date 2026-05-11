#pragma once
#include "BaseScene.h"
#include "Combat.h"
#include "Party.h"
#include "Engine.h"
#include "Audio.h"

struct SDL_Texture;

struct AbilitiesSounds {

    std::string idSound = "";
    int fxSound = -1;

    AbilitiesSounds() = default;
    ~AbilitiesSounds(){}

    void SetIdSound(std::string id);
    void SetFxSound(std::string path);
};

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
    void LoadSounds();

    void ChooseSound(std::string id);

    bool OnUIMouseClickEvent(UIElement* uiElement) override;

    inline bool DidPlayerWin() const { return playerWon; }

    //helpers
    void OnResume() override;
    void OnPause() override;
    void CreateUI();

    std::function<void(bool)> onCombatEnd;

private:
    Party* alliedParty;
    Party* enemyParty;
    Combat* combat;

    bool combatFinished;
    bool playerWon;

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
    void DrawSkillCosts();

    void UpdateNextRoundPause(float dt);
    void DrawNextRoundBanner();


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
    SDL_Texture* panelBaseTexture;     
    SDL_Texture* hpBarChunkTexture;     
    SDL_Texture* initiativeBarChunkTexture; 
    SDL_Texture* nextRound;
    SDL_Texture* arrow;
    SDL_Texture* poisonIcon;
    SDL_Texture* burnIcon;

    // icono por personaje: cargados dinámicamente por nombre
    std::unordered_map<std::string, SDL_Texture*> characterIcons;

    int hoveredSkillIdx = -1;

    void DrawArrowCurrentActor();
    void DrawSkillTooltip();
    void UpdateSkillHover();
    void DrawColoredLine(const std::string& line, int x, int y);
    std::vector<std::string> WrapText(const std::string& text, int maxCharsPerLine);


    void LoadSound();
    //audio variables
    int buttonPress;
    //music dir
    const char* combMusic = "Assets/Audio/Music/8bitMusic/over_the_seas.wav";

    float nextRoundTimer = 0.0f;
    bool nextRoundPauseActive = false;

    std::vector<AbilitiesSounds> abilities;

    // ---Character Panels ---------------------------
    void DrawAlliedPanels();
    void DrawEnemyPanels();
    void DrawCharacterPanel(Character* c, int panelX, int panelY, bool isAlly);
    void DrawHealthBar(int x, int y, int currentHP, int maxHP, bool leftToRight = true);
    void DrawInitiativeBar(int x, int y, int currentInitiative, bool leftToRight = true);

    //---------Timer-----------
    static constexpr float NEXT_ROUND_PAUSE_DURATION = 3000.0f; // segundos

    // -------- Panel layout constants ------------------
    static constexpr int PANEL_W = 200;
    static constexpr int PANEL_H = 80;
    // Posición del icono dentro del panel (relativa al origen del panel)
    static constexpr int ICON_OFFSET_X = 2;
    static constexpr int ICON_OFFSET_Y = 3;
    static constexpr int ICON_W = 48;
    static constexpr int ICON_H = 48;
    // Posición de la barra de vida dentro del panel
    static constexpr int HP_BAR_OFFSET_X = 108;
    static constexpr int HP_BAR_OFFSET_Y = 16;
    static constexpr int HP_CHUNK_W = 10;
    static constexpr int HP_CHUNK_H = 12;
    static constexpr int HP_MAX_CHUNKS = 10;
    // Posición de la barra de iniciativa dentro del panel
    static constexpr int INIT_BAR_OFFSET_X = 98;
    static constexpr int INIT_BAR_OFFSET_Y = 40;
    static constexpr int INIT_CHUNK_W = 12;
    static constexpr int INIT_CHUNK_H = 10;
    static constexpr int INIT_MAX_CHUNKS = 10;
    // Posiciones en pantalla de los paneles aliados (izquierda, apilados verticalmente)
    static constexpr int ALLIED_PANEL_X = 10;
    static constexpr int ALLIED_PANEL_START_Y = 400;
    static constexpr int PANEL_VERTICAL_GAP = 90;
    // Posiciones en pantalla de los paneles enemigos (derecha)
    static constexpr int ENEMY_PANEL_X = 1050;
    static constexpr int ENEMY_PANEL_START_Y = 400;
    // Enemies
    static constexpr int BAR_CHUNK_OVERLAP = 2;
    static constexpr int MAX_INITIATIVE = 250;
    static constexpr int HP_BAR_OFFSET_X_ENEMY = 118;
    static constexpr int INIT_BAR_OFFSET_X_ENEMY = 127;

    //-----------Skills UI------------------------
    static constexpr int SKILL_BTN_W = 64;
    static constexpr int SKILL_BTN_H = 64;
    static constexpr int SKILL_BTN_Y = 500;
    static constexpr int SKILL_BTN_SPACING = 70;
};
