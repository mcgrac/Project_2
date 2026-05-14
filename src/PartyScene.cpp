#include "PartyScene.h"
#include "Engine.h"
#include "Scene.h"
#include "UIManager.h"
#include "Textures.h"
#include "Render.h"
#include "Input.h"
#include "Skill.h"
#include "UpgradeTree.h"
#include "UpgradeTier.h"
#include "Upgrade.h"
#include "Inventory.h"
#include "Log.h"
#include <SDL3/SDL.h>
#include <sstream>

const SDL_Rect PartyScene::PORTRAIT_RECT = { 20,   80,  220, 300 };
const SDL_Rect PartyScene::STATS_PANEL_RECT = { 20,  300,  484, 431 };
const SDL_Rect PartyScene::INV_SLOT_RECT = { 250,  80,   64,  64 };
const SDL_Rect PartyScene::SKILL_ICON_RECT = { 640,  80,   60,  60 };
const SDL_Rect PartyScene::UPGRADE_RECT = { 640, 380,  280,  50 };
const SDL_Rect PartyScene::TAB_RECT = { 1050, 20,   60,  60 };
const SDL_Rect PartyScene::NAME_RECT = { 20, 30, 336, 81 };

PartyScene::PartyScene(Party* allied)
    : alliedParty(allied)
    , selectedMemberIndex(0)
{
    sceneName = "PartyScene";
}

PartyScene::~PartyScene() {}

void PartyScene::Load()
{
    LOG("PartyScene: abriendo.");
    LoadTextures();
    RefreshButtons();
}


void PartyScene::Update(float dt)
{
    //debug->add 1000 gold pressing 2
    if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
    {
        Character* c = alliedParty->GetMembers()[selectedMemberIndex];
        c->LevelUp();

    }

    // Cerrar con ESC
    if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
    {
        Engine::GetInstance().scene->PopScene();
    }

    auto& members = alliedParty->GetMembers();
    if (members.empty()) return;

    Character* selected = members[selectedMemberIndex];

    // Fondo semitransparente
    SDL_Rect bg = { 0, 0, 1280, 720 };
    Engine::GetInstance().render->DrawRectangle(bg, 5, 5, 20, 230, true, false);

    RenderBackground(selected);
    RenderCharacterName();
    RenderMemberTabs();
    //RenderPortrait(selected);
    //RenderBars(selected);
    RenderStats(selected);
    RenderInventorySlots(selected);
    RenderSkillIcons(selected);
    RenderUpgradeTree(selected);
}


void PartyScene::PostUpdate(float dt)
{

    DrawSkillTooltip();
    DrawUpgradeTooltip();
    DrawInventoryTooltip();
}

#pragma region UNLOAD
void PartyScene::Unload()
{
    ClearButtons();
    UnloadTextures();
    LOG("PartyScene: cerrada.");
}

void PartyScene::UnloadTextures()
{
    Engine::GetInstance().textures->UnLoad(background);
    Engine::GetInstance().textures->UnLoad(statsPanelTexture);
    Engine::GetInstance().textures->UnLoad(abilityIconsTexture);
    Engine::GetInstance().textures->UnLoad(gemsTexture);
    Engine::GetInstance().textures->UnLoad(linesTexture);
    Engine::GetInstance().textures->UnLoad(upgradeIconsTexture);

    if (portraitTexture != nullptr)
    {
        Engine::GetInstance().textures->UnLoad(portraitTexture);
        portraitTexture = nullptr;
        loadedPortraitName = "";
    }
}
#pragma endregion


bool PartyScene::OnUIMouseClickEvent(UIElement* uiElement)
{
    int id = uiElement->id;

    // Tabs de miembros
    if (id >= MEMBER_TAB_BASE && id < MEMBER_TAB_BASE + (int)alliedParty->GetMemberCount())
    {
        selectedMemberIndex = id - MEMBER_TAB_BASE;
        loadedPortraitName = "";

        ClearButtons();
        RefreshButtons();

        LOG("PartyScene: seleccionado miembro %d.", selectedMemberIndex);
        return true;
    }

    // Cerrar
    if (id == BACK_BTN)
    {
        Engine::GetInstance().scene->PopScene();
        return true;
    }

    // Botones de ascensión: id = UPGRADE_BTN_BASE + tier*10 + opcion(0=A, 1=B)
    if (id >= UPGRADE_BTN_BASE)
    {
        int offsetId = id - UPGRADE_BTN_BASE;
        int tierIdx = offsetId / 10;
        int optionChoice = offsetId % 10;   // 0=A, 1=B

        auto& members = alliedParty->GetMembers();
        if (selectedMemberIndex < (int)members.size())
        {
            Character* c = members[selectedMemberIndex];
            UpgradeTree* tree = c->GetUpgradeTree();
            if (tree != nullptr)
            {
                std::vector<UpgradeTier>& tiers = tree->GetTiers();
                if (tierIdx < (int)tiers.size())
                {
                    // ChooseUpgrade: 1=A, 2=B
                    LOG("Aplicando upgrade: personaje=%s tierIdx=%d optionChoice=%d",
                        c->GetName().c_str(), tierIdx, optionChoice);

                    bool applied = tiers[tierIdx].ChooseUpgrade(optionChoice, *c);
                    if (applied)
                    {
                        LOG("PartyScene: mejora aplicada — tier %d opcion %d.", tierIdx, optionChoice);
                        c->SetTotalPower();
                        c->SetTotalSpeed();
                        c->SetTotalDurability();

                        ClearButtons();
                        RefreshButtons();
                    }
                    else
                    {
                        LOG("PartyScene: mejora no disponible (nivel insuficiente o ya elegida).");
                    }
                }
            }
        }
        return true;
    }

    return true;
}

void PartyScene::RenderCharacterName()
{
    if (nameTexture != nullptr)
    {
        Engine::GetInstance().render->DrawTexture(
            nameTexture,
            NAME_RECT.x,
            NAME_RECT.y
        );
    }
}

#pragma region LOAD
void PartyScene::LoadTextures()
{
    auto& members = alliedParty->GetMembers();
    Character* c = members[selectedMemberIndex];

    LoadBackground(c);
    LoadStatsTable();
    LoadCharacterNames(c);

    std::string path = "Assets/Textures/CombatScene/" + c->GetName() + "/AbilityIcons.png";
    abilityIconsTexture = Engine::GetInstance().textures->Load(path.c_str());

    gemsTexture = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Gems.png");
    linesTexture = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Lines.png");

    std::string upgPath = "Assets/Textures/Teams/" + c->GetName() + "/Upgrades.png";
    upgradeIconsTexture = Engine::GetInstance().textures->Load(upgPath.c_str());
}

void PartyScene::LoadBackground(Character* c)
{
    std::string path;
    path = "Assets/Textures/Teams/" + c->GetName() + "/background.png";
    background = Engine::GetInstance().textures->Load(path.c_str());
}

void PartyScene::LoadStatsTable()
{
    statsPanelTexture = Engine::GetInstance().textures->Load("Assets/Textures/Teams/StatChart.png");
}

void PartyScene::LoadCharacterNames(Character* c)
{
    // Liberar anterior
    if (nameTexture != nullptr)
    {
        Engine::GetInstance().textures->UnLoad(nameTexture);
        nameTexture = nullptr;
    }

    std::string path = "Assets/Textures/Teams/" + c->GetName() + "/nameLabel.png";

    nameTexture = Engine::GetInstance().textures->Load(path.c_str());

    LOG("Loaded name texture: %s", path.c_str());
}
#pragma endregion



#pragma region RENDER
void PartyScene::RenderBackground(Character* c)
{
    Engine::GetInstance().render->DrawTexture(background, 0, 0);
}

void PartyScene::RenderMemberTabs()
{
    auto& members = alliedParty->GetMembers();
    for (int i = 0; i < (int)members.size(); ++i)
    {
        SDL_Rect tab = { TAB_RECT.x + i * (TAB_RECT.w + TAB_GAP), TAB_RECT.y, TAB_RECT.w, TAB_RECT.h };

        Uint8 r, g, b;
        if (i == selectedMemberIndex)
        {
            r = 255; g = 215; b = 0;    // dorado si está seleccionado
        }
        else
        {
            r = 80; g = 80; b = 80;
        }

        Engine::GetInstance().render->DrawRectangle(tab, r, g, b, 255, true, false);

        // Nombre abreviado dentro del tab
        std::string abbr = members[i]->GetName().substr(0, 2);
        Engine::GetInstance().render->DrawText(
            abbr.c_str(), tab.x + 10, tab.y + 15, 40, 30, { 255, 255, 255, 255 }
        );
    }
}

void PartyScene::RenderPortrait(Character* c)
{
    // Cargar portrait si cambió de personaje
    std::string portraitPath = "Assets/Textures/Portraits/" + c->GetName() + ".png";
    if (portraitPath != loadedPortraitName)
    {
        if (portraitTexture != nullptr)
        {
            Engine::GetInstance().textures->UnLoad(portraitTexture);
            portraitTexture = nullptr;
        }
        portraitTexture = Engine::GetInstance().textures->Load(portraitPath.c_str());
        loadedPortraitName = portraitPath;
    }

    if (portraitTexture != nullptr)
    {
        Engine::GetInstance().render->DrawTexture(portraitTexture, PORTRAIT_RECT.x, PORTRAIT_RECT.y);
    }
    else
    {
        // Placeholder si no hay textura
        SDL_Rect placeholder = { PORTRAIT_RECT.x, PORTRAIT_RECT.y, PORTRAIT_RECT.w, PORTRAIT_RECT.h };
        Engine::GetInstance().render->DrawRectangle(placeholder, 60, 60, 80, 255, true, false);
    }

    // Nombre del personaje
    Engine::GetInstance().render->DrawText(
        c->GetName().c_str(),
        PORTRAIT_RECT.x, PORTRAIT_RECT.y - 25, PORTRAIT_RECT.w, 22,
        { 255, 255, 255, 255 }
    );
}

void PartyScene::RenderBars(Character* c)
{
    // --------Barra de vida-----------
    float hpRatio = (c->GetMaxHP() > 0)
        ? (float)c->GetCurrentHP() / (float)c->GetMaxHP()
        : 0.0f;

    // Fondo de la barra
    SDL_Rect hpBg = { PORTRAIT_RECT.x, HP_BAR_Y, BAR_W, BAR_H };
    Engine::GetInstance().render->DrawRectangle(hpBg, 60, 0, 0, 255, true, false);

    // Relleno
    SDL_Rect hpFill = { PORTRAIT_RECT.x, HP_BAR_Y, (int)(BAR_W * hpRatio), BAR_H };
    Engine::GetInstance().render->DrawRectangle(hpFill, 0, 200, 50, 255, true, false);

    // Texto HP
    std::string hpText = std::to_string(c->GetCurrentHP()) + "/" + std::to_string(c->GetMaxHP());
    Engine::GetInstance().render->DrawText(
        hpText.c_str(), PORTRAIT_RECT.x + BAR_W + 5, HP_BAR_Y, 100, BAR_H, { 255, 255, 255, 255 }
    );

    // ------------Barra de experiencia-----------
    float expRatio = (float)c->GetExperience() / 100.0f;

    SDL_Rect expBg = { PORTRAIT_RECT.x, EXP_BAR_Y, BAR_W, BAR_H };
    Engine::GetInstance().render->DrawRectangle(expBg, 20, 20, 60, 255, true, false);

    SDL_Rect expFill = { PORTRAIT_RECT.x, EXP_BAR_Y, (int)(BAR_W * expRatio), BAR_H };
    Engine::GetInstance().render->DrawRectangle(expFill, 100, 100, 255, 255, true, false);

    std::string expText = "EXP " + std::to_string(c->GetExperience()) + "/100";
    Engine::GetInstance().render->DrawText(
        expText.c_str(), PORTRAIT_RECT.x + BAR_W + 5, EXP_BAR_Y, 100, BAR_H, { 200, 200, 255, 255 }
    );
}

void PartyScene::RenderStats(Character* c)
{
    // Fondo del panel de stats
    if (statsPanelTexture != nullptr)
    {
        Engine::GetInstance().render->DrawTexture(
            statsPanelTexture,
            STATS_PANEL_RECT.x,
            STATS_PANEL_RECT.y
        );
    }

    SDL_Color col = { 255,255,255,255 };

    // Posición base RELATIVA al panel
    int textX = STATS_PANEL_RECT.x + STAT_VALUE_X;
    int y = STATS_PANEL_RECT.y + STAT_START_Y;

    auto DrawValue = [&](int value)
    {
            std::string txt = std::to_string(value);

            Engine::GetInstance().render->DrawText(
                txt.c_str(),
                textX,
                y,
                STAT_TEXT_W,
                STAT_TEXT_H,
                col
            );

            y += STAT_LINE_GAP;
    };

    DrawValue(c->GetTotalPower());
    DrawValue(c->GetTotalDurability());
    DrawValue(c->GetTotalSpeed());
    DrawValue((float)c->GetPoisonPower());
    DrawValue((float)c->GetFirePower());
    DrawValue((float)c->GetHealingPower());
    DrawValue(c->GetLifesteal());
}

void PartyScene::RenderInventorySlots(Character* c)
{
    hoveredInventorySlot = -1;
    inventoryTooltipText = "";

    SDL_Point mousePos = SceneUtils::GetMousePosition();
    int mx = mousePos.x;
    int my = mousePos.y;

    for (int i = 0; i < 3; ++i)
    {
        SDL_Rect slot = { INV_SLOT_RECT.x, INV_SLOT_RECT.y + i * (INV_SLOT_RECT.h + INV_SLOT_GAP), INV_SLOT_RECT.w, INV_SLOT_RECT.h };

        // Fondo del slot
        Engine::GetInstance().render->DrawRectangle(slot, 40, 40, 40, 255, true, false);
        Engine::GetInstance().render->DrawRectangle(slot, 80, 80, 80, 255, false, false);

        // TODO: cuando Inventory esté implementado, dibujar el sprite del item
        // Item* item = c->GetInventory()->GetSlot(i);
        // if (item != nullptr) { DrawTexture(item->GetSprite(), slot.x, slot.y); }

        // Tooltip hover
        bool hovered = SceneUtils::PointInRect(mx, my, slot);

        if (hovered)
        {
            hoveredInventorySlot = i;

            std::vector<EquippableItem*>& equipped = alliedParty->GetInventory().GetEquipped(c->GetName());

            if (i < (int)equipped.size() && equipped[i] != nullptr)
            {
                inventoryTooltipText = equipped[i]->GetName();
            }
            else
            {
                inventoryTooltipText = "Slot empty";
            }
        }
    }
}

void PartyScene::RenderSkillIcons(Character* c)
{
    hoveredSkillIdx = -1;

    SDL_Point mousePos = SceneUtils::GetMousePosition();
    int mx = mousePos.x;
    int my = mousePos.y;

    auto& skills = c->GetSkills();

    for (int i = 0; i < (int)skills.size(); ++i)
    {
        SDL_Rect icon = { SKILL_ICON_RECT.x + i * (SKILL_ICON_RECT.w + SKILL_ICON_GAP), SKILL_ICON_RECT.y, SKILL_ICON_RECT.w, SKILL_ICON_RECT.h };

        bool hovered = SceneUtils::PointInRect(mx, my, icon);


        if (hovered)
        {
            hoveredSkillIdx = i;
        }
    }
}

void PartyScene::RenderUpgradeTree(Character* c)
{
    hoveredUpgradeOption = -1;
    hoveredUpgradeT = -1;

    UpgradeTree* tree = c->GetUpgradeTree();
    if (tree == nullptr) return;

    const auto& tiers = tree->GetTiers();

    for (int t = 0; t < (int)tiers.size(); ++t)
    {
        int gemOffset = t * (GEM_W + UPGRADE_ICON_W + UPGRADE_ICON_GAP);
        int gemX = TREE_START_X + gemOffset;
        int gemY = TREE_START_Y;

        // Dibujar gema (columna t de la spritesheet, fila 0 = normal)
        SDL_Rect gemSrc = { t * GEM_W, 0, GEM_W, GEM_H };
        Engine::GetInstance().render->DrawTexture(gemsTexture, gemX, gemY, &gemSrc);

        // Dibujar línea entre gema y siguiente bloque (excepto en el último tier)
        if (t < (int)tiers.size() - 1)
        {
            int lineX = gemX + GEM_W + UPGRADE_ICON_W;
            int lineY = TREE_START_Y + (GEM_H / 2) - (LINE_H / 2);
            Engine::GetInstance().render->DrawTexture(linesTexture, lineX, lineY);
        }

        // Dibujar iconos de mejora
        bool available = tiers[t].IsAvailable(c->GetLevel());
        bool unlocked = tiers[t].IsUnlocked();
        bool aChosen = unlocked && (tiers[t].GetChosen() == &tiers[t].GetOptionA());
        bool bChosen = unlocked && (tiers[t].GetChosen() == &tiers[t].GetOptionB());

        int iconX = gemX + GEM_W;

        // ---- Opción A ----
        SDL_Rect iconASrc = { (t * 2) * UPGRADE_ICON_W, 0, UPGRADE_ICON_W, UPGRADE_ICON_H };
        if (aChosen)
        {
            iconASrc.y = UPGRADE_ICON_H; // fila pressed
            SetTextureTint(upgradeIconsTexture, 255, 255, 150); // brillo dorado
        }
        else if (!available || bChosen)
        {
            SetTextureTint(upgradeIconsTexture, 80, 80, 80); // gris
        }
        else
        {
            ResetTextureTint(upgradeIconsTexture); // normal disponible
        }
        Engine::GetInstance().render->DrawTexture(
            upgradeIconsTexture, iconX, TREE_START_Y, &iconASrc
        );
        ResetTextureTint(upgradeIconsTexture);

        // ---- Opción B ----
        SDL_Rect iconBSrc = { (t * 2 + 1) * UPGRADE_ICON_W, 0, UPGRADE_ICON_W, UPGRADE_ICON_H };
        if (bChosen)
        {
            iconBSrc.y = UPGRADE_ICON_H; // fila pressed
            SetTextureTint(upgradeIconsTexture, 255, 255, 150); // brillo dorado
        }
        else if (!available || aChosen)
        {
            SetTextureTint(upgradeIconsTexture, 80, 80, 80); // gris
        }
        else
        {
            ResetTextureTint(upgradeIconsTexture); // normal disponible
        }
        Engine::GetInstance().render->DrawTexture(
            upgradeIconsTexture, iconX, TREE_START_Y + UPGRADE_ICON_H + UPGRADE_ICON_GAP, &iconBSrc
        );
        ResetTextureTint(upgradeIconsTexture);

        //hover detection
        SDL_Point mousePos = SceneUtils::GetMousePosition();
        int mx = mousePos.x;
        int my = mousePos.y;

        SDL_Rect iconARect = { iconX, TREE_START_Y, UPGRADE_ICON_W, UPGRADE_ICON_H };
        SDL_Rect iconBRect = { iconX, TREE_START_Y + UPGRADE_ICON_H + UPGRADE_ICON_GAP, UPGRADE_ICON_W, UPGRADE_ICON_H };

        if (SceneUtils::PointInRect(mx, my, iconARect))
        {
            hoveredUpgradeT = t;
            hoveredUpgradeOption = 0;
        }
        else if (SceneUtils::PointInRect(mx, my, iconBRect))
        {
            hoveredUpgradeT = t;
            hoveredUpgradeOption = 1;
        }
    }
}
#pragma endregion

void PartyScene::RefreshButtons()
{
    //get the current character
    Character* c = alliedParty->GetMembers()[selectedMemberIndex];

    //---------------abilities--------
    // Recargar spritesheet del personaje seleccionado
    if (abilityIconsTexture != nullptr)
    {
        Engine::GetInstance().textures->UnLoad(abilityIconsTexture);
        abilityIconsTexture = nullptr;
    }

    std::string path = "Assets/Textures/CombatScene/" + c->GetName() + "/AbilityIcons.png";
    abilityIconsTexture = Engine::GetInstance().textures->Load(path.c_str());

    //recarga upgrades spritesheet
    if (upgradeIconsTexture != nullptr)
    {
        Engine::GetInstance().textures->UnLoad(upgradeIconsTexture);
        upgradeIconsTexture = nullptr;
    }
    upgradeIconsTexture = Engine::GetInstance().textures->Load(("Assets/Textures/Teams/" + c->GetName() + "/Upgrades.png").c_str());

    //---------skills
    auto& skills = c->GetSkills();
    for (int i = 0; i < (int)skills.size(); ++i)
    {
        SDL_Rect icon = { SKILL_ICON_RECT.x + i * (SKILL_ICON_RECT.w + SKILL_ICON_GAP),
                          SKILL_ICON_RECT.y, SKILL_ICON_RECT.w, SKILL_ICON_RECT.h };
        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON,
            SKILL_BTN_BASE + i,
            skills[i].GetName().c_str(),
            icon,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {},
            abilityIconsTexture, i, icon.w, icon.h
        );
    }

    // ------Tabs de miembros---------------
    auto& members = alliedParty->GetMembers();

    for (int i = 0; i < (int)members.size(); ++i)
    {
        SDL_Rect tab = { TAB_RECT.x + i * (TAB_RECT.w + TAB_GAP), TAB_RECT.y, TAB_RECT.w, TAB_RECT.h };

        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON, MEMBER_TAB_BASE + i, "", tab,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }
        );
    }

    // --------------Botón cerrar------------
    SDL_Rect closeBounds = { 20, 20, 40, 40 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, BACK_BTN, "X", closeBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }
    );

    // ----------Botones de ascensiones del personaje seleccionado--------------
    //if (selectedMemberIndex < (int)members.size())
    //{
    //    Character* c = members[selectedMemberIndex];
    //    UpgradeTree* tree = c->GetUpgradeTree();
    //    if (tree != nullptr)
    //    {
    //        const auto& tiers = tree->GetTiers();
    //        for (int t = 0; t < (int)tiers.size(); ++t)
    //        {
    //            bool available = tiers[t].IsAvailable(c->GetLevel());
    //            bool unlocked = tiers[t].IsUnlocked();
    //            bool canChoose = available && !unlocked;

    //            int baseY = UPGRADE_RECT.y + t * (UPGRADE_RECT.h * 2 + UPGRADE_BTN_GAP * 3);

    //            // Solo crear botones clickables si se puede elegir
    //            if (canChoose)
    //            {
    //                SDL_Rect btnA = { UPGRADE_RECT.x, baseY, UPGRADE_RECT.w, UPGRADE_RECT.h };
    //                Engine::GetInstance().uiManager->CreateUIElement(
    //                    UIElementType::BUTTON, UPGRADE_BTN_BASE + t * 10 + 0, "", btnA,
    //                    [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }
    //                );

    //                SDL_Rect btnB = { UPGRADE_RECT.x, baseY + UPGRADE_RECT.h + UPGRADE_BTN_GAP, UPGRADE_RECT.w, UPGRADE_RECT.h };
    //                Engine::GetInstance().uiManager->CreateUIElement(
    //                    UIElementType::BUTTON, UPGRADE_BTN_BASE + t * 10 + 1, "", btnB,
    //                    [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }
    //                );
    //            }
    //        }
    //    }
    //}

    //------------background-----------------
    if (background != nullptr) {
        Engine::GetInstance().textures->UnLoad(background);
        background = nullptr;
    }

    //-------------ascensions--------
    UpgradeTree* tree = c->GetUpgradeTree();
    if (tree != nullptr)
    {
        const auto& tiers = tree->GetTiers();
        for (int t = 0; t < (int)tiers.size(); ++t)
        {
            bool available = tiers[t].IsAvailable(c->GetLevel());
            bool unlocked = tiers[t].IsUnlocked();
            bool canChoose = available && !unlocked;

            if (canChoose)
            {
                int gemOffset = t * (GEM_W + UPGRADE_ICON_W + UPGRADE_ICON_GAP);
                int iconX = TREE_START_X + GEM_W + gemOffset;

                // columnas en spritesheet: t*2 = opcionA, t*2+1 = opcionB
                SDL_Rect btnA = { iconX, TREE_START_Y, UPGRADE_ICON_W, UPGRADE_ICON_H };
                Engine::GetInstance().uiManager->CreateUIElement(
                    UIElementType::BUTTON, UPGRADE_BTN_BASE + t * 10 + 0, "", btnA,
                    [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {},
                    upgradeIconsTexture, t * 2, btnA.w, btnA.h
                );

                SDL_Rect btnB = { iconX, TREE_START_Y + UPGRADE_ICON_H + UPGRADE_ICON_GAP, UPGRADE_ICON_W, UPGRADE_ICON_H };

                Engine::GetInstance().uiManager->CreateUIElement(
                    UIElementType::BUTTON, UPGRADE_BTN_BASE + t * 10 + 1, "", btnB,
                    [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {},
                    upgradeIconsTexture, t * 2 + 1, btnB.w, btnB.h
                );
            }
        }
    }

    LoadBackground(c);
    LoadCharacterNames(c);
}

void PartyScene::ClearButtons()
{
    Engine::GetInstance().uiManager->CleanUp();
}

#pragma region SKILL_HOVER
void PartyScene::DrawSkillTooltip()
{
    if (hoveredSkillIdx == -1) { return; }

    Character* c = alliedParty->GetMembers()[selectedMemberIndex];
    auto& skills = c->GetSkills();
    if (hoveredSkillIdx >= (int)skills.size()) { return; }

    tooltipRenderer.Draw(
        skills[hoveredSkillIdx].GetFullDescription(),
        SKILL_ICON_RECT.x,
        SKILL_ICON_RECT.y + SKILL_ICON_RECT.h + 10
    );
}

void PartyScene::DrawUpgradeTooltip()
{
    if (hoveredUpgradeT == -1) { return; }

    Character* c = alliedParty->GetMembers()[selectedMemberIndex];
    UpgradeTree* tree = c->GetUpgradeTree();
    if (tree == nullptr) { return; }

    const auto& tiers = tree->GetTiers();
    if (hoveredUpgradeT >= (int)tiers.size()) { return; }

    const Upgrade* upgrade = nullptr;
    if (hoveredUpgradeOption == 0)
    {
        upgrade = &tiers[hoveredUpgradeT].GetOptionA();
    }
    else
    {
        upgrade = &tiers[hoveredUpgradeT].GetOptionB();
    }

    if (upgrade == nullptr) { return; }

    tooltipRenderer.Draw(
        upgrade->GetFullDescription(),
        TREE_START_X,
        TREE_START_Y + GEM_H + UPGRADE_ICON_H + 20
    );

    hoveredUpgradeT = -1;
    hoveredUpgradeOption = -1;
}

void PartyScene::DrawInventoryTooltip()
{
    if (hoveredInventorySlot == -1)
    {
        return;
    }

    SDL_Point mousePos = SceneUtils::GetMousePosition();

    tooltipRenderer.Draw(
        inventoryTooltipText,
        mousePos.x + 20,
        mousePos.y + 20
    );

    hoveredInventorySlot = -1;
}
#pragma endregion

#pragma region COLOR TINT
void PartyScene::SetTextureTint(SDL_Texture* tex, Uint8 r, Uint8 g, Uint8 b)
{
    if (tex == nullptr) { return; }
    SDL_SetTextureColorMod(tex, r, g, b);
}

void PartyScene::ResetTextureTint(SDL_Texture* tex)
{
    if (tex == nullptr) { return; }
    SDL_SetTextureColorMod(tex, 255, 255, 255);
}
#pragma endregion