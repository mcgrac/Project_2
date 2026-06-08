#include "PartyScene.h"
#include "Engine.h"
#include "Audio.h"
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

const SDL_Rect PartyScene::PORTRAIT_RECT = { 514,   10,  220, 300 };
const SDL_Rect PartyScene::STATS_PANEL_RECT = { 20,  300,  484, 431 };
const SDL_Rect PartyScene::INV_SLOT_RECT = { 420,  11,   64,  64 };
const SDL_Rect PartyScene::SKILL_ICON_RECT = { 709,  246,   64,  64 };

const SDL_Rect PartyScene::TAB_RECT = { 514, 10,   64,  64 };
const SDL_Rect PartyScene::NAME_RECT = { 20, 30, 336, 81 };

#pragma region POSITIONS
#pragma region Items

const SDL_Rect PartyScene::Item1_Bounds = { 191, 71, 669, 428 };

#pragma endregion
#pragma region Stats

const SDL_Rect PartyScene::Stat_Bounds = { 624, 92, 16, 28 };

#pragma endregion

#pragma region Gems

const SDL_Rect PartyScene::Gem_Bounds = { 503, 395, 72, 72 };

#pragma endregion
#pragma region LEVEL
const SDL_Rect PartyScene::level_Bounds = { 50, 250, 50, 50 };
#pragma endregion

#pragma endregion

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

void PartyScene::LoadSound() {
    buttonPress = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/UIfx/button_press.wav");
    upgradefx = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Island_menu/levelUp.wav");
}

void PartyScene::Update(float dt)
{
#if _DEBUG
    //debug->level up
    if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
    {
        Character* c = alliedParty->GetMembers()[selectedMemberIndex];
        c->LevelUp();

    }
#endif // _DEBUG

    auto& members = alliedParty->GetMembers();
    if (members.empty()) return;

    Character* selected = members[selectedMemberIndex];

    // Fondo semitransparente
    SDL_Rect bg = { 0, 0, 1280, 720 };
    Engine::GetInstance().render->DrawRectangle(bg, 5, 5, 20, 230, true, false);

    RenderBackground(selected);
    //RenderCharacterName();
    RenderMemberTabs();
    RenderStats(selected);
    RenderBars(selected);
    RenderInventorySlots(selected);
    RenderSkillIcons(selected);
    RenderUpgradeTree(selected);
    RenderLevel(selected);
}


void PartyScene::PostUpdate(float dt)
{
    Engine::GetInstance().render->DrawTexture(noBootPanel, 501, 90);
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

    Engine::GetInstance().textures->UnLoad(noBootPanel);

    Engine::GetInstance().textures->UnLoad(markusSkill0);
    Engine::GetInstance().textures->UnLoad(markusSkill1);
    Engine::GetInstance().textures->UnLoad(markusSkill2);
    Engine::GetInstance().textures->UnLoad(markusSkill3);
    Engine::GetInstance().textures->UnLoad(markusSkill4);

    Engine::GetInstance().textures->UnLoad(gerberaSkill0);
    Engine::GetInstance().textures->UnLoad(gerberaSkill1);
    Engine::GetInstance().textures->UnLoad(gerberaSkill2);
    Engine::GetInstance().textures->UnLoad(gerberaSkill3);
    Engine::GetInstance().textures->UnLoad(gerberaSkill4);


    Engine::GetInstance().textures->UnLoad(theresiaSkill0);
    Engine::GetInstance().textures->UnLoad(theresiaSkill1);
    Engine::GetInstance().textures->UnLoad(theresiaSkill2);
    Engine::GetInstance().textures->UnLoad(theresiaSkill3);
    Engine::GetInstance().textures->UnLoad(theresiaSkill4);

    Engine::GetInstance().textures->UnLoad(ignisSkill0);
    Engine::GetInstance().textures->UnLoad(ignisSkill1);
    Engine::GetInstance().textures->UnLoad(ignisSkill2);
    Engine::GetInstance().textures->UnLoad(ignisSkill3);
    Engine::GetInstance().textures->UnLoad(ignisSkill4);

    Engine::GetInstance().textures->UnLoad(fatuusSkill0);
    Engine::GetInstance().textures->UnLoad(fatuusSkill1);
    Engine::GetInstance().textures->UnLoad(fatuusSkill2);
    Engine::GetInstance().textures->UnLoad(fatuusSkill3);
    Engine::GetInstance().textures->UnLoad(fatuusSkill4);


    Engine::GetInstance().textures->UnLoad(markusAscension0);
    Engine::GetInstance().textures->UnLoad(markusAscension1);
    Engine::GetInstance().textures->UnLoad(markusAscension2);
    Engine::GetInstance().textures->UnLoad(markusAscension3);
    Engine::GetInstance().textures->UnLoad(markusAscension4);
    Engine::GetInstance().textures->UnLoad(markusAscension5);

    Engine::GetInstance().textures->UnLoad(gerberaAscension0);
    Engine::GetInstance().textures->UnLoad(gerberaAscension1);
    Engine::GetInstance().textures->UnLoad(gerberaAscension2);
    Engine::GetInstance().textures->UnLoad(gerberaAscension3);
    Engine::GetInstance().textures->UnLoad(gerberaAscension4);
    Engine::GetInstance().textures->UnLoad(gerberaAscension5);

    Engine::GetInstance().textures->UnLoad(theresiaAscension0);
    Engine::GetInstance().textures->UnLoad(theresiaAscension1);
    Engine::GetInstance().textures->UnLoad(theresiaAscension2);
    Engine::GetInstance().textures->UnLoad(theresiaAscension3);
    Engine::GetInstance().textures->UnLoad(theresiaAscension4);
    Engine::GetInstance().textures->UnLoad(theresiaAscension5);

    Engine::GetInstance().textures->UnLoad(fatuusAscension0);
    Engine::GetInstance().textures->UnLoad(fatuusAscension1);
    Engine::GetInstance().textures->UnLoad(fatuusAscension2);
    Engine::GetInstance().textures->UnLoad(fatuusAscension3);
    Engine::GetInstance().textures->UnLoad(fatuusAscension4);
    Engine::GetInstance().textures->UnLoad(fatuusAscension5);

    Engine::GetInstance().textures->UnLoad(jochiAscension0);
    Engine::GetInstance().textures->UnLoad(jochiAscension1);
    Engine::GetInstance().textures->UnLoad(jochiAscension2);
    Engine::GetInstance().textures->UnLoad(jochiAscension3);
    Engine::GetInstance().textures->UnLoad(jochiAscension4);
    Engine::GetInstance().textures->UnLoad(jochiAscension5);

    Engine::GetInstance().textures->UnLoad(ignisAscension0);
    Engine::GetInstance().textures->UnLoad(ignisAscension1);
    Engine::GetInstance().textures->UnLoad(ignisAscension2);
    Engine::GetInstance().textures->UnLoad(ignisAscension3);
    Engine::GetInstance().textures->UnLoad(ignisAscension4);
    Engine::GetInstance().textures->UnLoad(ignisAscension5);

    Engine::GetInstance().textures->UnLoad(markusIcon);
    Engine::GetInstance().textures->UnLoad(gerberaIcon);
    Engine::GetInstance().textures->UnLoad(theresiaIcon);
    Engine::GetInstance().textures->UnLoad(jochiIcon);
    Engine::GetInstance().textures->UnLoad(fatuusIcon);
    Engine::GetInstance().textures->UnLoad(ignisIcon);


    Engine::GetInstance().textures->UnLoad(jochiSkill0);

    Engine::GetInstance().textures->UnLoad(background);

    Engine::GetInstance().textures->UnLoad(abilityIconsTexture);
    Engine::GetInstance().textures->UnLoad(Gem5);
    Engine::GetInstance().textures->UnLoad(Gem10);
    Engine::GetInstance().textures->UnLoad(Gem15);
    Engine::GetInstance().textures->UnLoad(linesTexture);
    Engine::GetInstance().textures->UnLoad(upgradeIconsTexture);
    Engine::GetInstance().textures->UnLoad(selectedMember);

    hpBar.UnloadTexture();
    xpBar.UnloadTexture();

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
                        Engine::GetInstance().audio->PlayFx(upgradefx);
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

void PartyScene::RenderLevel(Character* c)
{
    int level = c->GetLevel();
    std::string levelStr = std::to_string(level);
    SDL_Color col = { 255,255,255,255 };

    if(level<10){ Engine::GetInstance().render->DrawText(levelStr.c_str(), 40, 313, 15, 28, col); }
    else{ Engine::GetInstance().render->DrawText(levelStr.c_str(), 34, 313, 27, 28, col); }
 
}

#pragma region LOAD
void PartyScene::LoadTextures()
{
    auto& members = alliedParty->GetMembers();
    Character* c = members[selectedMemberIndex];

    markusIcon = Engine::GetInstance().textures->Load("Assets/Textures/Teams/markusIcon.png");
    gerberaIcon = Engine::GetInstance().textures->Load("Assets/Textures/Teams/gerberaIcon.png");
    theresiaIcon = Engine::GetInstance().textures->Load("Assets/Textures/Teams/theresiaIcon.png");
    ignisIcon = Engine::GetInstance().textures->Load("Assets/Textures/Teams/ignisIcon.png");
    fatuusIcon = Engine::GetInstance().textures->Load("Assets/Textures/Teams/fatuusIcon.png");
    jochiIcon = Engine::GetInstance().textures->Load("Assets/Textures/Teams/jochiIcon.png");

    noBootPanel = Engine::GetInstance().textures->Load("Assets/Textures/Teams/NoBootPanel.png");
    selectedMember = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Selector.png");

    LoadBackground(c);
    LoadStatsTable();
    LoadCharacterNames(c);
    LoadSkillDescriptions();
    LoadAscensionDescriptions();

    std::string path = "Assets/Textures/CombatScene/" + c->GetName() + "/AbilityIcons.png";
    abilityIconsTexture = Engine::GetInstance().textures->Load(path.c_str());


    Gem5 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Gem5.png");
    Gem10 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Gem10.png");
    Gem15 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Gem15.png");
    linesTexture = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Lines.png");

    std::string upgPath = "Assets/Textures/Teams/" + c->GetName() + "/Upgrades.png";
    upgradeIconsTexture = Engine::GetInstance().textures->Load(upgPath.c_str());

    backButton = Engine::GetInstance().textures->Load("Assets/Textures/Teams/BackButton.png");

    hpBar.chunkW = 24;
    hpBar.chunkH = 20;
    hpBar.position = Vector2D(89.0f, 315.0f);
    hpBar.leftToRight = true;
    hpBar.maxChunks = 18;
    hpBar.chunkOverlap = 4;
    hpBar.LoadTexture("Assets/Textures/Teams/HealthPoint.png");

    xpBar.chunkW = 24;
    xpBar.chunkH = 8;
    xpBar.position = Vector2D(89.0f, 296.0f);
    xpBar.leftToRight = true;
    xpBar.maxChunks = 18;
    xpBar.chunkOverlap = 4;
    xpBar.LoadTexture("Assets/Textures/Teams/EXPPoint.png");
}

void PartyScene::LoadSkillDescriptions()
{
    markusSkill0 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Markus/skill0.png");
    markusSkill1 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Markus/skill1.png");
    markusSkill2 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Markus/skill2.png");
    markusSkill3 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Markus/skill3.png");
    markusSkill4 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Markus/skill4.png");

    gerberaSkill0 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Gerbera/skill0.png");
    gerberaSkill1 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Gerbera/skill1.png");
    gerberaSkill2 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Gerbera/skill2.png");
    gerberaSkill3 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Gerbera/skill3.png");
    gerberaSkill4 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Gerbera/skill4.png");

    theresiaSkill0 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Theresia/skill0.png");
    theresiaSkill1 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Theresia/skill1.png");
    theresiaSkill2 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Theresia/skill2.png");
    theresiaSkill3 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Theresia/skill3.png");
    theresiaSkill4 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Theresia/skill4.png");

    jochiSkill0 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Jochi/skill0.png");
    jochiSkill1 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Jochi/skill1.png");
    jochiSkill2 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Jochi/skill2.png");
    jochiSkill3 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Jochi/skill3.png");
    jochiSkill4 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Jochi/skill4.png");

    fatuusSkill0 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Fatuus/skill0.png");
    fatuusSkill1 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Fatuus/skill1.png");
    fatuusSkill2 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Fatuus/skill2.png");
    fatuusSkill3 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Fatuus/skill3.png");
    fatuusSkill4 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Fatuus/skill4.png");

    ignisSkill0 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Ignis/skill0.png");
    ignisSkill1 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Ignis/skill1.png");
    ignisSkill2 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Ignis/skill2.png");
    ignisSkill3 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Ignis/skill3.png");
    ignisSkill4 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Ignis/skill4.png");
}

void PartyScene::LoadAscensionDescriptions()
{
    markusAscension0 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Markus/ascension0.png");
    markusAscension1 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Markus/ascension1.png");
    markusAscension2 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Markus/ascension2.png");
    markusAscension3 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Markus/ascension3.png");
    markusAscension4 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Markus/ascension4.png");
    markusAscension5 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Markus/ascension5.png");

    gerberaAscension0 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Gerbera/ascension0.png");
    gerberaAscension1 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Gerbera/ascension1.png");
    gerberaAscension2 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Gerbera/ascension2.png");
    gerberaAscension3 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Gerbera/ascension3.png");
    gerberaAscension4 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Gerbera/ascension4.png");
    gerberaAscension5 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Gerbera/ascension5.png");

    ignisAscension0 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Ignis/ascension0.png");
    ignisAscension1 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Ignis/ascension1.png");
    ignisAscension2 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Ignis/ascension2.png");
    ignisAscension3 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Ignis/ascension3.png");
    ignisAscension4 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Ignis/ascension4.png");
    ignisAscension5 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Ignis/ascension5.png");

    theresiaAscension0 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Theresia/ascension0.png");
    theresiaAscension1 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Theresia/ascension1.png");
    theresiaAscension2 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Theresia/ascension2.png");
    theresiaAscension3 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Theresia/ascension3.png");
    theresiaAscension4 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Theresia/ascension4.png");
    theresiaAscension5 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Theresia/ascension5.png");

    jochiAscension0 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Jochi/ascension0.png");
    jochiAscension1 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Jochi/ascension1.png");
    jochiAscension2 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Jochi/ascension2.png");
    jochiAscension3 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Jochi/ascension3.png");
    jochiAscension4 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Jochi/ascension4.png");
    jochiAscension5 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Jochi/ascension5.png");

    fatuusAscension0 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Fatuus/ascension0.png");
    fatuusAscension1 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Fatuus/ascension1.png");
    fatuusAscension2 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Fatuus/ascension2.png");
    fatuusAscension3 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Fatuus/ascension3.png");
    fatuusAscension4 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Fatuus/ascension4.png");
    fatuusAscension5 = Engine::GetInstance().textures->Load("Assets/Textures/Teams/Fatuus/ascension5.png");
}

void PartyScene::LoadBackground(Character* c)
{
    std::string path;
    path = "Assets/Textures/Teams/" + c->GetName() + "/background.png";
    background = Engine::GetInstance().textures->Load(path.c_str());
}

void PartyScene::LoadStatsTable()
{

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

        if (members[i]->GetName() == "Markus") {
            Engine::GetInstance().render->DrawTexture(markusIcon, tab.x, tab.y);
        }
        else if (members[i]->GetName() == "Gerbera") {
            Engine::GetInstance().render->DrawTexture(gerberaIcon, tab.x, tab.y);
        }
        else if (members[i]->GetName() == "Theresia") {
            Engine::GetInstance().render->DrawTexture(theresiaIcon, tab.x, tab.y);
        }
        else if (members[i]->GetName() == "Ignis") {
            Engine::GetInstance().render->DrawTexture(ignisIcon, tab.x, tab.y);
        }
        else if (members[i]->GetName() == "Fatuus") {
            Engine::GetInstance().render->DrawTexture(fatuusIcon, tab.x, tab.y);
        }
        else if (members[i]->GetName() == "Jochi") {
            Engine::GetInstance().render->DrawTexture(jochiIcon, tab.x, tab.y);
        }

        if (members[i]->GetLevel() >= 20) {
            SDL_Color Yellow = { 255, 255, 0, 255 };
            std::string lvl = std::to_string(members[i]->GetLevel());
            Engine::GetInstance().render->DrawText((lvl).c_str(), 518 + (100 * i), 16, 17, 12, Yellow);
        }
        else if (members[i]->GetLevel() < 10) {
            SDL_Color White = { 255, 255, 255, 255 };
            std::string lvl = std::to_string(members[i]->GetLevel());
            Engine::GetInstance().render->DrawText((lvl).c_str(), 522 + (100 * i), 16, 10, 12, White);
        }
        else {
            SDL_Color White = { 255, 255, 255, 255 };
            std::string lvl = std::to_string(members[i]->GetLevel());
            Engine::GetInstance().render->DrawText((lvl).c_str(), 518 + (100 * i), 16, 17, 12, White);
        }

        if (i == selectedMemberIndex)
        {
            if (selectedMember != nullptr)
            {
                Engine::GetInstance().render->DrawTexture(selectedMember, tab.x, tab.y);
            }
        }
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
    hpBar.Draw(c->GetCurrentHP(), c->GetMaxHP());
    xpBar.Draw(c->GetExperience(), 100);
}

void PartyScene::RenderStats(Character* c)
{
    // Fondo del panel de stats

    SDL_Color col = { 255,255,255,255 };

    // Posición base RELATIVA al panel
    int textX = 406;
    int textW = 24;
    int y = 378;

    auto DrawValue = [&](int value)
    {
            if (value < 10) {
                textX = 418;
                textW = 12;
            }
            else if (value < 100) {
                textX = 406;
                textW = 24;
            }
            else {
                textX = 398;
                textW = 32;
            }
            std::string txt = std::to_string(value);

            Engine::GetInstance().render->DrawText(
                txt.c_str(),
                textX,
                y,
                textW,
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

    std::vector<EquippableItem*>& equipped = alliedParty->GetInventory().GetEquipped(c->GetName());


    for (int i = 0; i < 3; ++i)
    {
        SDL_Rect slot = { INV_SLOT_RECT.x, INV_SLOT_RECT.y + i * (INV_SLOT_RECT.h + INV_SLOT_GAP), INV_SLOT_RECT.w, INV_SLOT_RECT.h };

        // Dibujar textura del item si el slot está ocupado
        if (i < (int)equipped.size() && equipped[i] != nullptr)
        {
            std::string texPath = "Assets/Textures/Teams/ItemsIcons/" + equipped[i]->GetName() + ".png";
            SDL_Texture* itemTex = Engine::GetInstance().textures->Load(texPath.c_str());

            if (itemTex != nullptr)
            {
                Engine::GetInstance().render->DrawTexture(itemTex, slot.x, slot.y, nullptr, false);
                Engine::GetInstance().textures->UnLoad(itemTex);
            }
        }

        // Tooltip hover
        bool hovered = SceneUtils::PointInRect(mx, my, slot);

        if (hovered)
        {
            hoveredInventorySlot = i;

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


    if (c->GetLevel() >= 5) {
        Engine::GetInstance().render->DrawTexture(Gem5, 503, 395);
    }

    if (c->GetLevel() >= 10) {
        Engine::GetInstance().render->DrawTexture(Gem10, 755, 395);
    }

    if (c->GetLevel() >= 15) {
        Engine::GetInstance().render->DrawTexture(Gem15, 1006, 395);
    }

    for (int t = 0; t < (int)tiers.size(); ++t)
    {

        // Dibujar iconos de mejora
        bool available = tiers[t].IsAvailable(c->GetLevel());
        bool unlocked = tiers[t].IsUnlocked();
        bool aChosen = unlocked && (tiers[t].GetChosen() == &tiers[t].GetOptionA());
        bool bChosen = unlocked && (tiers[t].GetChosen() == &tiers[t].GetOptionB());

        int iconX = 627 + 254 * t;
       
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
            upgradeIconsTexture, iconX, 340, &iconASrc
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
            upgradeIconsTexture, iconX, 451, &iconBSrc
        );
        ResetTextureTint(upgradeIconsTexture);

        //hover detection
        SDL_Point mousePos = SceneUtils::GetMousePosition();
        int mx = mousePos.x;
        int my = mousePos.y;

        SDL_Rect iconARect = { iconX, 340, UPGRADE_ICON_W, UPGRADE_ICON_H };
        SDL_Rect iconBRect = { iconX, 451, UPGRADE_ICON_W, UPGRADE_ICON_H };

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

    //---------skills----------
    auto& skills = c->GetSkills();
    for (int i = 0; i < (int)skills.size(); ++i)
    {
        SDL_Rect icon = { SKILL_ICON_RECT.x + i * (SKILL_ICON_RECT.w + SKILL_ICON_GAP),
                          SKILL_ICON_RECT.y, SKILL_ICON_RECT.w, SKILL_ICON_RECT.h };
        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON,
            SKILL_BTN_BASE + i,
            "",
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
    SDL_Rect closeBounds = { 1188, 10, 72, 72 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, BACK_BTN, "", closeBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {},
        backButton, 0, closeBounds.w, closeBounds.h
    );


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
                
                int iconX = 627 + 252*t;

                // columnas en spritesheet: t*2 = opcionA, t*2+1 = opcionB
                SDL_Rect btnA = { iconX, 340, UPGRADE_ICON_W, UPGRADE_ICON_H };
                Engine::GetInstance().uiManager->CreateUIElement(
                    UIElementType::BUTTON, UPGRADE_BTN_BASE + t * 10 + 0, "", btnA,
                    [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {},
                    upgradeIconsTexture, t * 2, btnA.w, btnA.h
                );

                SDL_Rect btnB = { iconX, 451, UPGRADE_ICON_W, UPGRADE_ICON_H };

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

    if (c->GetName() == "Markus") {
        switch (hoveredSkillIdx){
            case 0: Engine::GetInstance().render->DrawTexture(markusSkill0, 501, 90);
                break;
            case 1: Engine::GetInstance().render->DrawTexture(markusSkill1, 501, 90);
                break;
            case 2: Engine::GetInstance().render->DrawTexture(markusSkill2, 501, 90);
                break;
            case 3: Engine::GetInstance().render->DrawTexture(markusSkill3, 501, 90);
                break;
            case 4: Engine::GetInstance().render->DrawTexture(markusSkill4, 501, 90);
                break;
        }
    }
    else if (c->GetName() == "Gerbera") {
        switch (hoveredSkillIdx) {
        case 0: Engine::GetInstance().render->DrawTexture(gerberaSkill0, 501, 90);
            break;
        case 1: Engine::GetInstance().render->DrawTexture(gerberaSkill1, 501, 90);
            break;
        case 2: Engine::GetInstance().render->DrawTexture(gerberaSkill2, 501, 90);
            break;
        case 3: Engine::GetInstance().render->DrawTexture(gerberaSkill3, 501, 90);
            break;
        case 4: Engine::GetInstance().render->DrawTexture(gerberaSkill4, 501, 90);
            break;
        }
    }
    else if (c->GetName() == "Ignis") {
        switch (hoveredSkillIdx) {
        case 0: Engine::GetInstance().render->DrawTexture(ignisSkill0, 501, 90);
            break;
        case 1: Engine::GetInstance().render->DrawTexture(ignisSkill1, 501, 90);
            break;
        case 2: Engine::GetInstance().render->DrawTexture(ignisSkill2, 501, 90);
            break;
        case 3: Engine::GetInstance().render->DrawTexture(ignisSkill3, 501, 90);
            break;
        case 4: Engine::GetInstance().render->DrawTexture(ignisSkill4, 501, 90);
            break;
        }
    }
    else if (c->GetName() == "Theresia") {
        switch (hoveredSkillIdx) {
        case 0: Engine::GetInstance().render->DrawTexture(theresiaSkill0, 501, 90);
            break;
        case 1: Engine::GetInstance().render->DrawTexture(theresiaSkill1, 501, 90);
            break;
        case 2: Engine::GetInstance().render->DrawTexture(theresiaSkill2, 501, 90);
            break;
        case 3: Engine::GetInstance().render->DrawTexture(theresiaSkill3, 501, 90);
            break;
        case 4: Engine::GetInstance().render->DrawTexture(theresiaSkill4, 501, 90);
            break;
        }
    }
    else if (c->GetName() == "Fatuus") {
        switch (hoveredSkillIdx) {
        case 0: Engine::GetInstance().render->DrawTexture(fatuusSkill0, 501, 90);
            break;
        case 1: Engine::GetInstance().render->DrawTexture(fatuusSkill1, 501, 90);
            break;
        case 2: Engine::GetInstance().render->DrawTexture(fatuusSkill2, 501, 90);
            break;
        case 3: Engine::GetInstance().render->DrawTexture(fatuusSkill3, 501, 90);
            break;
        case 4: Engine::GetInstance().render->DrawTexture(fatuusSkill4, 501, 90);
            break;
        }
    }
    else if (c->GetName() == "Jochi") {
        switch (hoveredSkillIdx) {
        case 0: Engine::GetInstance().render->DrawTexture(jochiSkill0, 501, 90);
            break;
        case 1: Engine::GetInstance().render->DrawTexture(jochiSkill1, 501, 90);
            break;
        case 2: Engine::GetInstance().render->DrawTexture(jochiSkill2, 501, 90);
            break;
        case 3: Engine::GetInstance().render->DrawTexture(jochiSkill3, 501, 90);
            break;
        case 4: Engine::GetInstance().render->DrawTexture(jochiSkill4, 501, 90);
            break;
        }
    }

    



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

//draw description ascension

    if (c->GetName() == "Markus") {
        if (upgrade->name == "Deadlier Rays") {
            Engine::GetInstance().render->DrawTexture(markusAscension0, 501, 545);
        }
        else if (upgrade->name == "Morning Light") {
            Engine::GetInstance().render->DrawTexture(markusAscension1, 501, 545);
        }
        else if (upgrade->name == "Day Break") {
            Engine::GetInstance().render->DrawTexture(markusAscension2, 501, 545);
        }
        else if (upgrade->name == "Purifier") {
            Engine::GetInstance().render->DrawTexture(markusAscension3, 501, 545);
        }
        else if (upgrade->name == "Fight") {
            Engine::GetInstance().render->DrawTexture(markusAscension4, 501, 545);
        }
        else if (upgrade->name == "Flight") {
            Engine::GetInstance().render->DrawTexture(markusAscension5, 501, 545);
        }
    }
    else if (c->GetName() == "Gerbera") {
        if (upgrade->name == "Poisoned Points") {
            Engine::GetInstance().render->DrawTexture(gerberaAscension0, 501, 545);
        }
        else if (upgrade->name == "Serrated Points") {
            Engine::GetInstance().render->DrawTexture(gerberaAscension1, 501, 545);
        }
        else if (upgrade->name == "Swift Shooting") {
            Engine::GetInstance().render->DrawTexture(gerberaAscension2, 501, 545);
        }
        else if (upgrade->name == "Steady Shooting") {
            Engine::GetInstance().render->DrawTexture(gerberaAscension3, 501, 545);
        }
        else if (upgrade->name == "Fire Lotus") {
            Engine::GetInstance().render->DrawTexture(gerberaAscension4, 501, 545);
        }
        else if (upgrade->name == "Thorned Rose") {
            Engine::GetInstance().render->DrawTexture(gerberaAscension5, 501, 545);
        }
    }
    else if (c->GetName() == "Ignis") {
        if (upgrade->name == "Life Surge") {
            Engine::GetInstance().render->DrawTexture(ignisAscension0, 501, 545);
        }
        else if (upgrade->name == "Power Surge") {
            Engine::GetInstance().render->DrawTexture(ignisAscension1, 501, 545);
        }
        else if (upgrade->name == "Quick Metabolism") {
            Engine::GetInstance().render->DrawTexture(ignisAscension2, 501, 545);
        }
        else if (upgrade->name == "Uncontrolled Overgrowth") {
            Engine::GetInstance().render->DrawTexture(ignisAscension3, 501, 545);
        }
        else if (upgrade->name == "Jungle's Heart") {
            Engine::GetInstance().render->DrawTexture(ignisAscension4, 501, 545);
        }
        else if (upgrade->name == "Fire Spirit") {
            Engine::GetInstance().render->DrawTexture(ignisAscension5, 501, 545);
        }
    }
    else if (c->GetName() == "Theresia") {
        if (upgrade->name == "Heavy Armor") {
            Engine::GetInstance().render->DrawTexture(theresiaAscension0, 501, 545);
        }
        else if (upgrade->name == "Light Armor") {
            Engine::GetInstance().render->DrawTexture(theresiaAscension1, 501, 545);
        }
        else if (upgrade->name == "Solar Cape") {
            Engine::GetInstance().render->DrawTexture(theresiaAscension2, 501, 545);
        }
        else if (upgrade->name == "Moonsilver Pendant") {
            Engine::GetInstance().render->DrawTexture(theresiaAscension3, 501, 545);
        }
        else if (upgrade->name == "Immovable Object") {
            Engine::GetInstance().render->DrawTexture(theresiaAscension4, 501, 545);
        }
        else if (upgrade->name == "Crown's Will") {
            Engine::GetInstance().render->DrawTexture(theresiaAscension5, 501, 546);
        }
    }
    else if (c->GetName() == "Fatuus") {
        if (upgrade->name == "Sharp-Ended Scales") {
            Engine::GetInstance().render->DrawTexture(fatuusAscension0, 501, 545);
        }
        else if (upgrade->name == "Reinforced Scales") {
            Engine::GetInstance().render->DrawTexture(fatuusAscension1, 501, 545);
        }
        else if (upgrade->name == "Apex Predator") {
            Engine::GetInstance().render->DrawTexture(fatuusAscension2, 501, 545);
        }
        else if (upgrade->name == "Elementalist") {
            Engine::GetInstance().render->DrawTexture(fatuusAscension3, 501, 545);
        }
        else if (upgrade->name == "Specialization") {
            Engine::GetInstance().render->DrawTexture(fatuusAscension4, 501, 545);
        }
        else if (upgrade->name == "Adaptability") {
            Engine::GetInstance().render->DrawTexture(fatuusAscension5, 501, 545);
        }
    }
    else if (c->GetName() == "Jochi") {
        if (upgrade->name == "Artifact Studies") {
            Engine::GetInstance().render->DrawTexture(jochiAscension0, 501, 545);
        }
        else if (upgrade->name == "Alchemy Studies") {
            Engine::GetInstance().render->DrawTexture(jochiAscension1, 501, 545);
        }
        else if (upgrade->name == "Quick Spelling") {
            Engine::GetInstance().render->DrawTexture(jochiAscension2, 501, 545);
        }
        else if (upgrade->name == "Elemental Scepter") {
            Engine::GetInstance().render->DrawTexture(jochiAscension3, 501, 545);
        }
        else if (upgrade->name == "Immesurable Power") {
            Engine::GetInstance().render->DrawTexture(jochiAscension4, 501, 545);
        }
        else if (upgrade->name == "Poisoned Blood") {
            Engine::GetInstance().render->DrawTexture(jochiAscension5, 501, 545);
        }
        }
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