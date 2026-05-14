#pragma once
#include "BaseScene.h"
#include "Party.h"
#include "Character.h"
#include <vector>
#include <string>
#include "SceneUtils.h"
#include "SDL3/SDL.h"

struct SDL_Texture;

class PartyScene : public BaseScene
{
public:
    PartyScene(Party* allied);
    ~PartyScene();

    void Load() override;
    void LoadSound();
    void Update(float dt) override;
    void PostUpdate(float dt) override;
    void Unload() override;

    bool OnUIMouseClickEvent(UIElement* uiElement) override;

private:
    Party* alliedParty;
    int selectedMemberIndex = 0;   // 0-2, personaje actualmente mostrado

    // -------------Texturas ----------
    SDL_Texture* background = nullptr;
    SDL_Texture* statsPanelTexture = nullptr;  // fondo del panel de stats
    SDL_Texture* portraitTexture = nullptr;  // portrait grande del personaje actual
    std::string loadedPortraitName = "";        // para saber si hay que recargar
    SDL_Texture* abilityIconsTexture = nullptr;
    SDL_Texture* nameTexture = nullptr;

    // Texturas árbol de mejoras
    SDL_Texture* upgradeIconsTexture = nullptr;  // spritesheet por personaje
    SDL_Texture* gemsTexture = nullptr;  // spritesheet gemas estática
    SDL_Texture* linesTexture = nullptr;  // imagen líneas estática

    void LoadBackground(Character* c);
    void LoadStatsTable();
    void LoadCharacterNames(Character* c);
    // ------------ Tooltips ----------
    std::string tooltipText = "";   // texto a mostrar en el tooltip activo
    int hoveredSkillIdx = -1;   // índice de la skill hoverada (-1 = ninguna)

    // ------------ Secciones de la UI --------------
    // Background
    void RenderBackground(Character* c);

    // Dibuja los 3 botones mini de selección de personaje (esquina superior derecha)
    void RenderMemberTabs();

    // Dibuja el portrait grande + nombre
    void RenderPortrait(Character* c);

    // Dibuja la barra de vida y la de experiencia
    void RenderBars(Character* c);

    // Dibuja el panel de stats con valores numéricos
    void RenderStats(Character* c);

    // Dibuja los 3 slots de inventario y gestiona tooltips de items
    void RenderInventorySlots(Character* c);

    // Dibuja los iconos de habilidades y gestiona tooltips
    void RenderSkillIcons(Character* c);

    // Dibuja los botones de ascensiones
    void RenderUpgradeTree(Character* c);

    // Recrea los botones de la UI al cambiar de personaje
    void RefreshButtons();
    void ClearButtons();

    //render character names
    void RenderCharacterName();

    void LoadTextures();
    void UnloadTextures();

    //tooltip
    void DrawSkillTooltip();
    TooltipRenderer tooltipRenderer;
    //void DrawColoredLine(const std::string& line, int x, int y);
    //std::vector<std::string> WrapText(const std::string& text, int maxCharsPerLine);
    void DrawUpgradeTooltip();
    void DrawInventoryTooltip();

    //Color tint
    void SetTextureTint(SDL_Texture* tex, Uint8 r, Uint8 g, Uint8 b);
    void ResetTextureTint(SDL_Texture* tex);

    //tooltip ascensions
    int hoveredUpgradeT = -1;  // tier hovered
    int hoveredUpgradeOption = -1;  // 0=A, 1=B

    //tooltip inventory
    int hoveredInventorySlot = -1;
    std::string inventoryTooltipText = "";

    // ----- IDs de botones ----------------------
    // Tabs de miembros: 1, 2, 3
    // Botón cerrar: 9
    // Ascensiones tier 0: 100 (optionA), 101 (optionB)
    // Ascensiones tier 1: 110 (optionA), 111 (optionB)
    // Ascensiones tier 2: 120 (optionA), 121 (optionB)
    static constexpr int MEMBER_TAB_BASE = 1;
    static constexpr int BACK_BTN = 9;
    static constexpr int SKILL_BTN_BASE = 50;
    static constexpr int UPGRADE_BTN_BASE = 100;   // 100 + tier*10 + 0/1 para A/B

    //Layout constants
    static const SDL_Rect PORTRAIT_RECT;
    static const SDL_Rect STATS_PANEL_RECT;
    static const SDL_Rect INV_SLOT_RECT;       // posición base, Y se desplaza por índice
    static const SDL_Rect SKILL_ICON_RECT;     // posición base, X se desplaza por índice
    static const SDL_Rect UPGRADE_RECT;        // posición base, Y se desplaza por tier
    static const SDL_Rect TAB_RECT;            // posición base, X se desplaza por índice
    static const SDL_Rect NAME_RECT;           //letrero

    // ----------- Stats panel layout ------------
    // Posición del primer stat dentro del panel
    static constexpr int STAT_VALUE_X = 250;
    static constexpr int STAT_START_Y = 100;
    static constexpr int STAT_LINE_GAP = 45;

    // Tamaño del texto
    static constexpr int STAT_TEXT_W = 45;
    static constexpr int STAT_TEXT_H = 30;
    //-----------------------------

    //--------- Layout árbol de mejoras
    static constexpr int TREE_START_X = 400;
    static constexpr int TREE_START_Y = 500;

    static constexpr int GEM_W = 72;
    static constexpr int GEM_H = 72;

    static constexpr int UPGRADE_ICON_W = 72;
    static constexpr int UPGRADE_ICON_H = 72;
    static constexpr int UPGRADE_ICON_GAP = 10;   // gap vertical entre A y B

    static constexpr int LINE_W = 114; //line ascension
    static constexpr int LINE_H = 116; //line ascension
    //---------------------------

    static constexpr int HP_BAR_Y = 390;
    static constexpr int EXP_BAR_Y = 405;
    static constexpr int BAR_W = 220;
    static constexpr int BAR_H = 12;
    //-------------------------------

    static constexpr int INV_SLOT_GAP = 10;
    static constexpr int SKILL_ICON_GAP = 10;
    static constexpr int UPGRADE_BTN_GAP = 10;
    static constexpr int TAB_GAP = 10;

    //audio variables
    int buttonPress;
    int upgradefx;

    const char* musicTrack;

};
