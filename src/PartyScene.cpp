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

PartyScene::PartyScene(Party* allied)
    : alliedParty(allied)
    , selectedMemberIndex(0)
{
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
    // Cerrar con ESC
    if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
    {
        Engine::GetInstance().scene->PopScene();
    }
}


void PartyScene::PostUpdate(float dt)
{
    auto& members = alliedParty->GetMembers();
    if (members.empty()) return;

    Character* selected = members[selectedMemberIndex];

    // Fondo semitransparente
    SDL_Rect bg = { 0, 0, 1280, 720 };
    Engine::GetInstance().render->DrawRectangle(bg, 5, 5, 20, 230, true, false);

    RenderBackground(selected);
    RenderMemberTabs();
    RenderPortrait(selected);
    RenderBars(selected);
    RenderStats(selected);
    RenderInventorySlots(selected);
    RenderSkillIcons(selected);
    RenderUpgradeTree(selected);

    // Tooltip al final para que quede encima de todo
    Vector2D mousePos = Engine::GetInstance().input->GetMousePosition();
    if (!tooltipText.empty())
    {
        RenderTooltip((int)mousePos.getX(), (int)mousePos.getY());
    }
    tooltipText = "";
}

void PartyScene::Unload()
{
    ClearButtons();
    UnloadTextures();
    LOG("PartyScene: cerrada.");
}


bool PartyScene::OnUIMouseClickEvent(UIElement* uiElement)
{
    int id = uiElement->id;

    // Tabs de miembros
    if (id >= MEMBER_TAB_BASE && id < MEMBER_TAB_BASE + (int)alliedParty->GetMemberCount())
    {
        selectedMemberIndex = id - MEMBER_TAB_BASE;
        loadedPortraitName = "";
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
                    bool applied = tiers[tierIdx].ChooseUpgrade(optionChoice + 1, *c);
                    if (applied)
                    {
                        LOG("PartyScene: mejora aplicada — tier %d opcion %d.", tierIdx, optionChoice);
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

#pragma region LOAD
void PartyScene::LoadTextures()
{
    auto& members = alliedParty->GetMembers();
    Character* c = members[selectedMemberIndex];

    LoadBackground(c);
    LoadStatsTable();
    LoadCharacterNames(c);

    background = Engine::GetInstance().textures->Load("Assets/Textures/Backgrounds/PartyBackground.png");
    statsPanelTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/StatsPanel.png");
}

void PartyScene::LoadBackground(Character* c)
{
    std::string path;
    path = "   " + c->GetName(); +"   ";
    Engine::GetInstance().textures->Load(path.c_str());
}

void PartyScene::LoadStatsTable()
{
    Engine::GetInstance().textures->Load("  ");
}

void PartyScene::LoadCharacterNames(Character* c)
{
    std::string path;
    path = "   " + c->GetName(); +"   ";
    Engine::GetInstance().textures->Load(path.c_str());
}
#pragma endregion



#pragma region RENDER
void PartyScene::RenderBackground(Character* c)
{
    std::string path;
    path = "   " + c->GetName(); +"   ";

}

void PartyScene::RenderMemberTabs()
{
    auto& members = alliedParty->GetMembers();
    for (int i = 0; i < (int)members.size(); ++i)
    {
        SDL_Rect tab;
        tab.x = TAB_X + i * (TAB_SIZE + TAB_GAP);
        tab.y = TAB_Y;
        tab.w = TAB_SIZE;
        tab.h = TAB_SIZE;

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
        Engine::GetInstance().render->DrawTexture(portraitTexture, PORTRAIT_X, PORTRAIT_Y);
    }
    else
    {
        // Placeholder si no hay textura
        SDL_Rect placeholder = { PORTRAIT_X, PORTRAIT_Y, PORTRAIT_W, PORTRAIT_H };
        Engine::GetInstance().render->DrawRectangle(placeholder, 60, 60, 80, 255, true, false);
    }

    // Nombre del personaje
    Engine::GetInstance().render->DrawText(
        c->GetName().c_str(),
        PORTRAIT_X, PORTRAIT_Y - 25, PORTRAIT_W, 22,
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
    SDL_Rect hpBg = { BAR_X, HP_BAR_Y, BAR_W, BAR_H };
    Engine::GetInstance().render->DrawRectangle(hpBg, 60, 0, 0, 255, true, false);

    // Relleno
    SDL_Rect hpFill = { BAR_X, HP_BAR_Y, (int)(BAR_W * hpRatio), BAR_H };
    Engine::GetInstance().render->DrawRectangle(hpFill, 0, 200, 50, 255, true, false);

    // Texto HP
    std::string hpText = std::to_string(c->GetCurrentHP()) + "/" + std::to_string(c->GetMaxHP());
    Engine::GetInstance().render->DrawText(
        hpText.c_str(), BAR_X + BAR_W + 5, HP_BAR_Y, 100, BAR_H, { 255, 255, 255, 255 }
    );

    // ------------Barra de experiencia-----------
    float expRatio = (float)c->GetExperience() / 100.0f;

    SDL_Rect expBg = { BAR_X, EXP_BAR_Y, BAR_W, BAR_H };
    Engine::GetInstance().render->DrawRectangle(expBg, 20, 20, 60, 255, true, false);

    SDL_Rect expFill = { BAR_X, EXP_BAR_Y, (int)(BAR_W * expRatio), BAR_H };
    Engine::GetInstance().render->DrawRectangle(expFill, 100, 100, 255, 255, true, false);

    std::string expText = "EXP " + std::to_string(c->GetExperience()) + "/100";
    Engine::GetInstance().render->DrawText(
        expText.c_str(), BAR_X + BAR_W + 5, EXP_BAR_Y, 100, BAR_H, { 200, 200, 255, 255 }
    );
}

void PartyScene::RenderStats(Character* c)
{
    // Fondo del panel de stats
    if (statsPanelTexture != nullptr)
    {
        Engine::GetInstance().render->DrawTexture(
            statsPanelTexture, STATS_PANEL_X, STATS_PANEL_Y
        );
    }
    else
    {
        SDL_Rect panel = { STATS_PANEL_X, STATS_PANEL_Y, STATS_PANEL_W, STATS_PANEL_H };
        Engine::GetInstance().render->DrawRectangle(panel, 20, 20, 35, 220, true, false);
    }

    // Valores numéricos sobre el fondo
    int textX = STATS_PANEL_X + 10;
    int textY = STATS_PANEL_Y + 10;
    int lineH = 28;
    SDL_Color col = { 220, 220, 220, 255 };

    auto DrawStat = [&](const std::string& label, int value)
        {
            std::string line = label + ": " + std::to_string(value);
            Engine::GetInstance().render->DrawText(line.c_str(), textX, textY, STATS_PANEL_W - 20, lineH - 2, col);
            textY += lineH;
        };

    DrawStat("Level", c->GetLevel());
    DrawStat("Power", c->GetTotalPower());
    DrawStat("Speed", c->GetTotalSpeed());
    DrawStat("Durability", c->GetTotalDurability());
    DrawStat("Lifesteal", c->GetLifesteal());
    DrawStat("Heal Pwr", (int)c->GetHealingPower());
    DrawStat("Fire Mod", (int)c->GetFirePower());
    DrawStat("Poison Mod", (int)c->GetPoisonPower());
}

void PartyScene::RenderInventorySlots(Character* c)
{
    Vector2D mousePos = Engine::GetInstance().input->GetMousePosition();
    int mx = (int)mousePos.getX();
    int my = (int)mousePos.getY();

    for (int i = 0; i < 3; ++i)
    {
        SDL_Rect slot;
        slot.x = INV_SLOT_X;
        slot.y = INV_SLOT_Y + i * (INV_SLOT_SIZE + INV_SLOT_GAP);
        slot.w = INV_SLOT_SIZE;
        slot.h = INV_SLOT_SIZE;

        // Fondo del slot
        Engine::GetInstance().render->DrawRectangle(slot, 40, 40, 40, 255, true, false);
        Engine::GetInstance().render->DrawRectangle(slot, 80, 80, 80, 255, false, false);

        // TODO: cuando Inventory esté implementado, dibujar el sprite del item
        // Item* item = c->GetInventory()->GetSlot(i);
        // if (item != nullptr) { DrawTexture(item->GetSprite(), slot.x, slot.y); }

        // Tooltip hover
        bool hovered = mx > slot.x && mx < slot.x + slot.w
            && my > slot.y && my < slot.y + slot.h;
        if (hovered)
        {
            // TODO: tooltipText = item->GetDescription() cuando Inventory esté listo
            tooltipText = "Slot " + std::to_string(i + 1) + " (empty)";
        }
    }
}

void PartyScene::RenderSkillIcons(Character* c)
{
    Vector2D mousePos = Engine::GetInstance().input->GetMousePosition();
    int mx = (int)mousePos.getX();
    int my = (int)mousePos.getY();

    auto& skills = c->GetSkills();

    for (int i = 0; i < (int)skills.size(); ++i)
    {
        SDL_Rect icon;
        icon.x = SKILL_ICON_X + i * (SKILL_ICON_SIZE + SKILL_ICON_GAP);
        icon.y = SKILL_ICON_Y;
        icon.w = SKILL_ICON_SIZE;
        icon.h = SKILL_ICON_SIZE;

        // Placeholder — color según tipo de daño
        Uint8 r, g, b;
        switch (skills[i].GetDamageType())
        {
        case DamageType::Physical:
            r = 180; g = 100; b = 50;
            break;
        case DamageType::Magical:
            r = 80;  g = 80;  b = 200;
            break;
        default:
            r = 80;  g = 150; b = 80;
            break;
        }

        Engine::GetInstance().render->DrawRectangle(icon, r, g, b, 255, true, false);
        Engine::GetInstance().render->DrawRectangle(icon, 200, 200, 200, 255, false, false);

        // Número de la skill encima del icono
        Engine::GetInstance().render->DrawText(
            std::to_string(i + 1).c_str(),
            icon.x + 22, icon.y + 18, 16, 24, { 255, 255, 255, 255 }
        );

        // Tooltip hover
        bool hovered = mx > icon.x && mx < icon.x + icon.w && my > icon.y && my < icon.y + icon.h;
        if (hovered)
        {
            hoveredSkillIdx = i;
        }
    }

    if (hoveredSkillIdx >= 0 && hoveredSkillIdx < (int)skills.size())
    {
        const Skill& skill = skills[hoveredSkillIdx];

        std::string desc = skill.GetName()
            + " | Cost: " + std::to_string(skill.GetInitiativeCost());

        if (!skill.GetEffects().empty())
        {
            desc += " | " + skill.GetEffects()[0].description;
        }

        SDL_Rect descBox = { SKILL_ICON_X, SKILL_ICON_Y + SKILL_ICON_SIZE + 10, 600, 50 };
        Engine::GetInstance().render->DrawRectangle(descBox, 10, 10, 20, 200, true, false);
        Engine::GetInstance().render->DrawText(
            desc.c_str(),
            descBox.x + 8, descBox.y + 8, descBox.w - 16, descBox.h - 16,
            { 255, 255, 255, 255 }
        );
    }

    hoveredSkillIdx = -1;  // resetear cada frame
}

void PartyScene::RenderUpgradeTree(Character* c)
{
    UpgradeTree* tree = c->GetUpgradeTree();
    if (tree == nullptr) return;

    const auto& tiers = tree->GetTiers();

    for (int t = 0; t < (int)tiers.size(); ++t)
    {
        const UpgradeTier& tier = tiers[t];
        bool available = tier.IsAvailable(c->GetLevel());
        bool unlocked = tier.IsUnlocked();

        int baseY = UPGRADE_Y + t * (UPGRADE_BTN_H * 2 + UPGRADE_BTN_GAP * 3);

        // Nivel requerido
        std::string tierLabel = "Tier " + std::to_string(t + 1)
            + " (Lvl " + std::to_string(tier.GetRequiredLevel()) + ")";
        Engine::GetInstance().render->DrawText(
            tierLabel.c_str(), UPGRADE_X, baseY - 20, 300, 18, { 180, 180, 180, 255 }
        );

        // Opción A
        {
            SDL_Rect btnA = { UPGRADE_X, baseY, UPGRADE_BTN_W, UPGRADE_BTN_H };
            Uint8 r, g, b;

            if (unlocked && tier.GetChosen() == &tier.GetOptionA())
            {
                r = 255; g = 215; b = 0;    // dorado — elegida
            }
            else if (available && !unlocked)
            {
                r = 50; g = 150; b = 50;    // verde — disponible
            }
            else
            {
                r = 50; g = 50; b = 50;     // gris — bloqueada
            }

            Engine::GetInstance().render->DrawRectangle(btnA, r, g, b, 255, true, false);
            Engine::GetInstance().render->DrawText(
                tier.GetOptionA().name.c_str(),
                btnA.x + 5, btnA.y + 5, UPGRADE_BTN_W - 10, 20,
                { 255, 255, 255, 255 }
            );
            Engine::GetInstance().render->DrawText(
                tier.GetOptionA().description.c_str(),
                btnA.x + 5, btnA.y + 28, UPGRADE_BTN_W - 10, 16,
                { 200, 200, 200, 255 }
            );
        }

        // Opción B
        {
            SDL_Rect btnB = { UPGRADE_X, baseY + UPGRADE_BTN_H + UPGRADE_BTN_GAP,
                              UPGRADE_BTN_W, UPGRADE_BTN_H };
            Uint8 r, g, b;

            if (unlocked && tier.GetChosen() == &tier.GetOptionB())
            {
                r = 255; g = 215; b = 0;
            }
            else if (available && !unlocked)
            {
                r = 50; g = 150; b = 50;
            }
            else
            {
                r = 50; g = 50; b = 50;
            }

            Engine::GetInstance().render->DrawRectangle(btnB, r, g, b, 255, true, false);
            Engine::GetInstance().render->DrawText(
                tier.GetOptionB().name.c_str(),
                btnB.x + 5, btnB.y + 5, UPGRADE_BTN_W - 10, 20,
                { 255, 255, 255, 255 }
            );
            Engine::GetInstance().render->DrawText(
                tier.GetOptionB().description.c_str(),
                btnB.x + 5, btnB.y + 28, UPGRADE_BTN_W - 10, 16,
                { 200, 200, 200, 255 }
            );
        }
    }
}

void PartyScene::RenderTooltip(int mouseX, int mouseY)
{
    if (tooltipText.empty()) return;

    int tooltipW = 250;
    int tooltipH = 60;
    int tooltipX = mouseX + 15;
    int tooltipY = mouseY - tooltipH - 5;

    // Ajustar para que no se salga de pantalla
    if (tooltipX + tooltipW > 1280) tooltipX = mouseX - tooltipW - 5;
    if (tooltipY < 0)               tooltipY = mouseY + 15;

    SDL_Rect bg = { tooltipX, tooltipY, tooltipW, tooltipH };
    Engine::GetInstance().render->DrawRectangle(bg, 10, 10, 20, 230, true, false);
    Engine::GetInstance().render->DrawRectangle(bg, 150, 150, 150, 255, false, false);

    Engine::GetInstance().render->DrawText(
        tooltipText.c_str(),
        tooltipX + 8, tooltipY + 8, tooltipW - 16, tooltipH - 16,
        { 255, 255, 255, 255 }
    );
}
#pragma endregion



void PartyScene::RefreshButtons()
{
    auto& members = alliedParty->GetMembers();

    // Tabs de miembros
    for (int i = 0; i < (int)members.size(); ++i)
    {
        SDL_Rect tab;
        tab.x = TAB_X + i * (TAB_SIZE + TAB_GAP);
        tab.y = TAB_Y;
        tab.w = TAB_SIZE;
        tab.h = TAB_SIZE;

        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON, MEMBER_TAB_BASE + i, "", tab,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }
        );
    }

    // Botón cerrar
    SDL_Rect closeBounds = { 20, 20, 40, 40 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, BACK_BTN, "X", closeBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }
    );

    // Botones de ascensiones del personaje seleccionado
    if (selectedMemberIndex < (int)members.size())
    {
        Character* c = members[selectedMemberIndex];
        UpgradeTree* tree = c->GetUpgradeTree();
        if (tree != nullptr)
        {
            const auto& tiers = tree->GetTiers();
            for (int t = 0; t < (int)tiers.size(); ++t)
            {
                bool available = tiers[t].IsAvailable(c->GetLevel());
                bool unlocked = tiers[t].IsUnlocked();
                bool canChoose = available && !unlocked;

                int baseY = UPGRADE_Y + t * (UPGRADE_BTN_H * 2 + UPGRADE_BTN_GAP * 3);

                // Solo crear botones clickables si se puede elegir
                if (canChoose)
                {
                    SDL_Rect btnA = { UPGRADE_X, baseY, UPGRADE_BTN_W, UPGRADE_BTN_H };
                    Engine::GetInstance().uiManager->CreateUIElement(
                        UIElementType::BUTTON, UPGRADE_BTN_BASE + t * 10 + 0, "", btnA,
                        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }
                    );

                    SDL_Rect btnB = { UPGRADE_X, baseY + UPGRADE_BTN_H + UPGRADE_BTN_GAP, UPGRADE_BTN_W, UPGRADE_BTN_H };
                    Engine::GetInstance().uiManager->CreateUIElement(
                        UIElementType::BUTTON, UPGRADE_BTN_BASE + t * 10 + 1, "", btnB,
                        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }
                    );
                }
            }
        }
    }
}

void PartyScene::ClearButtons()
{
    Engine::GetInstance().uiManager->CleanUp();
}




void PartyScene::UnloadTextures()
{
    Engine::GetInstance().textures->UnLoad(background);
    Engine::GetInstance().textures->UnLoad(statsPanelTexture);

    if (portraitTexture != nullptr)
    {
        Engine::GetInstance().textures->UnLoad(portraitTexture);
        portraitTexture = nullptr;
        loadedPortraitName = "";
    }
}