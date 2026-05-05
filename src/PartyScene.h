#pragma once
#include "BaseScene.h"
#include "Party.h"
#include "Character.h"
#include <vector>
#include <string>

struct SDL_Texture;

class PartyScene : public BaseScene
{
public:
    PartyScene(Party* allied);
    ~PartyScene();

    void Load() override;
    void Update(float dt) override;
    void PostUpdate(float dt) override;
    void Unload() override;

    bool OnUIMouseClickEvent(UIElement* uiElement) override;

private:
    Party* alliedParty;
    int      selectedMemberIndex = 0;   // 0-2, personaje actualmente mostrado

    // -------------Texturas ----------
    SDL_Texture* background = nullptr;
    SDL_Texture* statsPanelTexture = nullptr;  // fondo del panel de stats
    SDL_Texture* portraitTexture = nullptr;  // portrait grande del personaje actual
    std::string loadedPortraitName = "";        // para saber si hay que recargar

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

    // Dibuja el tooltip en la posición del ratón
    void RenderTooltip(int mouseX, int mouseY);

    // Recrea los botones de la UI al cambiar de personaje
    void RefreshButtons();
    void ClearButtons();

    void LoadTextures();
    void UnloadTextures();

    // ----- IDs de botones ----------------------
    // Tabs de miembros: 1, 2, 3
    // Botón cerrar: 9
    // Ascensiones tier 0: 100 (optionA), 101 (optionB)
    // Ascensiones tier 1: 110 (optionA), 111 (optionB)
    // Ascensiones tier 2: 120 (optionA), 121 (optionB)
    static constexpr int MEMBER_TAB_BASE = 1;
    static constexpr int BACK_BTN = 9;
    static constexpr int UPGRADE_BTN_BASE = 100;   // 100 + tier*10 + 0/1 para A/B

    //Layout constants
    static constexpr int PORTRAIT_X = 20;
    static constexpr int PORTRAIT_Y = 80;
    static constexpr int PORTRAIT_W = 220;
    static constexpr int PORTRAIT_H = 300;

    static constexpr int STATS_PANEL_X = 20;
    static constexpr int STATS_PANEL_Y = 420;
    static constexpr int STATS_PANEL_W = 220;
    static constexpr int STATS_PANEL_H = 250;

    static constexpr int BAR_X = 20;
    static constexpr int HP_BAR_Y = 390;
    static constexpr int EXP_BAR_Y = 405;
    static constexpr int BAR_W = 220;
    static constexpr int BAR_H = 12;

    static constexpr int INV_SLOT_X = 250;
    static constexpr int INV_SLOT_Y = 80;
    static constexpr int INV_SLOT_SIZE = 64;
    static constexpr int INV_SLOT_GAP = 10;

    static constexpr int SKILL_ICON_X = 640;
    static constexpr int SKILL_ICON_Y = 80;
    static constexpr int SKILL_ICON_SIZE = 60;
    static constexpr int SKILL_ICON_GAP = 10;

    static constexpr int UPGRADE_X = 640;
    static constexpr int UPGRADE_Y = 380;
    static constexpr int UPGRADE_BTN_W = 280;
    static constexpr int UPGRADE_BTN_H = 50;
    static constexpr int UPGRADE_BTN_GAP = 10;

    static constexpr int TAB_X = 1050;
    static constexpr int TAB_Y = 20;
    static constexpr int TAB_SIZE = 60;
    static constexpr int TAB_GAP = 10;
};
