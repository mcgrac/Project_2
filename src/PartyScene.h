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

#pragma region POSITIONS

#pragma region Items

    static const SDL_Rect Item1_Bounds;
    int spaceItems = 70;

#pragma endregion
#pragma region STATS

    static const SDL_Rect Stat_Bounds;
    int spaceStats = 47;

#pragma endregion

#pragma region EXIT

    static const SDL_Rect Exit_Bounds;

#pragma endregion

#pragma region Icons

    static const SDL_Rect Icon_Bounds;
    int spaceIcons = 100;

#pragma endregion

#pragma region Gems

    static const SDL_Rect Gem_Bounds;
    int spaceGems = 252;

#pragma endregion

#pragma region Ascensions

    static const SDL_Rect Ascension_Bounds;
    int spaceAscensionsX = 251;
    int spaceAscensionsY = 110;

#pragma endregion

#pragma region Lines

    static const SDL_Rect Line_Bounds;
    static const SDL_Rect Line_Bounds2;
    int spaceLines = 52;

#pragma endregion

#pragma region LEVEL
    static const SDL_Rect level_Bounds;
#pragma endregion

#pragma endregion

private:
    Party* alliedParty;
    int selectedMemberIndex = 0;   // 0-2, personaje actualmente mostrado

#pragma region TEXTURES
    // -------------Texturas ----------
    SDL_Texture* background = nullptr;

    SDL_Texture* portraitTexture = nullptr;  // portrait grande del personaje actual
    std::string loadedPortraitName = "";        // para saber si hay que recargar
    SDL_Texture* abilityIconsTexture = nullptr;
    SDL_Texture* nameTexture = nullptr;

    SDL_Texture* noBootPanel = nullptr;

    // Texturas árbol de mejoras
    SDL_Texture* upgradeIconsTexture = nullptr;  // spritesheet por personaje

    SDL_Texture* Gem5 = nullptr;
    SDL_Texture* Gem10 = nullptr;
    SDL_Texture* Gem15 = nullptr;
    SDL_Texture* linesTexture = nullptr;  // imagen líneas estática
    SDL_Texture* backButton = nullptr;

    //character icons
    SDL_Texture* markusIcon = nullptr;
    SDL_Texture* gerberaIcon = nullptr;
    SDL_Texture* theresiaIcon = nullptr;
    SDL_Texture* fatuusIcon = nullptr;
    SDL_Texture* jochiIcon = nullptr;
    SDL_Texture* ignisIcon = nullptr;

    //Skill Descriptions
    SDL_Texture* markusSkill0 = nullptr;
    SDL_Texture* markusSkill1 = nullptr;
    SDL_Texture* markusSkill2 = nullptr;
    SDL_Texture* markusSkill3 = nullptr;
    SDL_Texture* markusSkill4 = nullptr;

    SDL_Texture* gerberaSkill0 = nullptr;
    SDL_Texture* gerberaSkill1 = nullptr;
    SDL_Texture* gerberaSkill2 = nullptr;
    SDL_Texture* gerberaSkill3 = nullptr;
    SDL_Texture* gerberaSkill4 = nullptr;

    SDL_Texture* ignisSkill0 = nullptr;
    SDL_Texture* ignisSkill1 = nullptr;
    SDL_Texture* ignisSkill2 = nullptr;
    SDL_Texture* ignisSkill3 = nullptr;
    SDL_Texture* ignisSkill4 = nullptr;

    SDL_Texture* theresiaSkill0 = nullptr;
    SDL_Texture* theresiaSkill1 = nullptr;
    SDL_Texture* theresiaSkill2 = nullptr;
    SDL_Texture* theresiaSkill3 = nullptr;
    SDL_Texture* theresiaSkill4 = nullptr;

    SDL_Texture* fatuusSkill0 = nullptr;
    SDL_Texture* fatuusSkill1 = nullptr;
    SDL_Texture* fatuusSkill2 = nullptr;
    SDL_Texture* fatuusSkill3 = nullptr;
    SDL_Texture* fatuusSkill4 = nullptr;
 

    SDL_Texture* jochiSkill0 = nullptr;
    SDL_Texture* jochiSkill1 = nullptr;
    SDL_Texture* jochiSkill2 = nullptr;
    SDL_Texture* jochiSkill3 = nullptr;
    SDL_Texture* jochiSkill4 = nullptr;

    //AscensionDescriptions
    SDL_Texture* fatuusAscension0 = nullptr;
    SDL_Texture* fatuusAscension1 = nullptr;
    SDL_Texture* fatuusAscension2 = nullptr;
    SDL_Texture* fatuusAscension3 = nullptr;
    SDL_Texture* fatuusAscension4 = nullptr;
    SDL_Texture* fatuusAscension5 = nullptr;

    SDL_Texture* gerberaAscension0 = nullptr;
    SDL_Texture* gerberaAscension1 = nullptr;
    SDL_Texture* gerberaAscension2 = nullptr;
    SDL_Texture* gerberaAscension3 = nullptr;
    SDL_Texture* gerberaAscension4 = nullptr;
    SDL_Texture* gerberaAscension5 = nullptr;

    SDL_Texture* ignisAscension0 = nullptr;
    SDL_Texture* ignisAscension1 = nullptr;
    SDL_Texture* ignisAscension2 = nullptr;
    SDL_Texture* ignisAscension3 = nullptr;
    SDL_Texture* ignisAscension4 = nullptr;
    SDL_Texture* ignisAscension5 = nullptr;

    SDL_Texture* jochiAscension0 = nullptr;
    SDL_Texture* jochiAscension1 = nullptr;
    SDL_Texture* jochiAscension2 = nullptr;
    SDL_Texture* jochiAscension3 = nullptr;
    SDL_Texture* jochiAscension4 = nullptr;
    SDL_Texture* jochiAscension5 = nullptr;

    SDL_Texture* markusAscension0 = nullptr;
    SDL_Texture* markusAscension1 = nullptr;
    SDL_Texture* markusAscension2 = nullptr;
    SDL_Texture* markusAscension3 = nullptr;
    SDL_Texture* markusAscension4 = nullptr;
    SDL_Texture* markusAscension5 = nullptr;

    SDL_Texture* theresiaAscension0 = nullptr;
    SDL_Texture* theresiaAscension1 = nullptr;
    SDL_Texture* theresiaAscension2 = nullptr;
    SDL_Texture* theresiaAscension3 = nullptr;
    SDL_Texture* theresiaAscension4 = nullptr;
    SDL_Texture* theresiaAscension5 = nullptr;

    SDL_Texture* selectedMember = nullptr;
#pragma endregion

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

    //render character names
    void RenderCharacterName();

    //render level
    void RenderLevel(Character* c);
    
    // Recrea los botones de la UI al cambiar de personaje
    void RefreshButtons();
    void ClearButtons();



    void LoadTextures();
    void UnloadTextures();

    void LoadSkillDescriptions();
    void LoadAscensionDescriptions();

    //tooltip
    void DrawSkillTooltip();
    TooltipRenderer tooltipRenderer;

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

    //bars
    DynamicBar hpBar;
    DynamicBar xpBar;

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

    static const SDL_Rect TAB_RECT;            // posición base, X se desplaza por índice
    static const SDL_Rect NAME_RECT;           //letrero

    // ----------- Stats panel layout ------------
    // Posición del primer stat dentro del panel
    static constexpr int STAT_VALUE_X = 250;
    static constexpr int STAT_START_Y = 100;
    static constexpr int STAT_LINE_GAP = 47;

    // Tamaño del texto
    static constexpr int STAT_TEXT_W = 45;
    static constexpr int STAT_TEXT_H = 20;
    //-----------------------------

    //--------- Layout árbol de mejoras


    static constexpr int GEM_W = 72;
    static constexpr int GEM_H = 72;

    static constexpr int UPGRADE_ICON_W = 72;
    static constexpr int UPGRADE_ICON_H = 72;


    static constexpr int LINE_W = 114; //line ascension
    static constexpr int LINE_H = 116; //line ascension
    //---------------------------

    static constexpr int HP_BAR_Y = 315;
    static constexpr int EXP_BAR_Y = 296;
    static constexpr int BAR_W = 220;
    static constexpr int BAR_H = 12;
    //-------------------------------

    static constexpr int INV_SLOT_GAP = 7;
    static constexpr int SKILL_ICON_GAP = 6;
    static constexpr int UPGRADE_BTN_GAP = 10;
    static constexpr int TAB_GAP = 36;

    //audio variables
    int buttonPress;
    int upgradefx;

    const char* musicTrack;

};
