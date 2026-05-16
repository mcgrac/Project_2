#include "CombatScene.h"
#include "CharacterFactory.h"
#include "Character.h"
#include "Scene.h"
#include "UIManager.h"
#include "Engine.h"
#include "Audio.h"
#include "Log.h"
#include "Textures.h"
#include "Render.h"
#include <sstream>
#include "Window.h"

#pragma region Continue
const SDL_Rect CombatScene::CONTINUE_BOUNDS = { 473, 540, 335, 105 };
#pragma endregion

#pragma region Extra Buttons
const SDL_Rect CombatScene::POTION_BOUNDS = { 852, 501, 64, 64 };
const SDL_Rect CombatScene::SKIP_BOUNDS = { 922, 501, 64, 64 };
#pragma endregion

#pragma region Potions
const SDL_Rect CombatScene::TEXT1_BOUNDS = { 879, 570, 11, 19 };
const SDL_Rect CombatScene::TEXT2_BOUNDS = { 874, 570, 22, 19 };
#pragma endregion

#pragma region ABILITIES_SOUND
void AbilitiesSounds::SetIdSound(std::string id)
{
    idSound = id;
}
void AbilitiesSounds::SetFxSound(std::string path)
{
    fxSound = Engine::GetInstance().audio->LoadFx(path.c_str());
}
#pragma endregion

CombatScene::CombatScene(Party* _allied, int _shipLevel, IslandFaction _faction)
    : alliedParty(_allied)
    , enemyParty(nullptr)
    , combat(nullptr)
    , combatFinished(false)
    , background(nullptr)
    , nextRound(nullptr)
    , arrow(nullptr)
    , poisonIcon(nullptr)
    , burnIcon(nullptr)
    , uiState(CombatUIState::HIDDEN)
    , selectedSkillIdx(-1)
    , laneInputConsumed (false)
    , shipLevel (_shipLevel)
    , playerWon(false)
    , currentIslandFaction(_faction)
{
    sceneName = "CombatScene";
}

CombatScene::~CombatScene()
{
    DestroyEnemyParty();
}

void CombatScene::LoadSounds() {

    LoadSoundsParty(alliedParty);
    LoadSoundsParty(enemyParty);
}

void CombatScene::LoadSoundsParty(Party* party)
{
    for (Character* c : party->GetMembers()) {

        //check if enemy
        std::string folder;
        if (c->GetIsAllied()) {
            folder = "allies";
        }
        else {
            folder = "enemies";
        }

        for (int i = 0; i < c->GetSkills().size(); i++) {
            Skill skill = c->GetSkills()[i];

            AbilitiesSounds ability;

            std::string path = "Assets/Audio/Fx/" + folder + "/" + c->GetName() + "/" + skill.GetAnimationId() + ".wav";
            ability.SetFxSound(path);
            ability.SetIdSound(skill.GetAnimationId());

            abilities.push_back(ability);
        }
    }
}

void CombatScene::Load()
{
    LOG("CombatScene: cargando...");

    CreateEnemyParty();
    LoadTextures();
    LoadSounds();
    LoadSound();
    Engine::GetInstance().audio->PlayMusic(("Assets/Audio/Music/Combat" + SceneUtils::GetFactionString(currentIslandFaction) + ".wav").c_str());

    // ---------Testing------------
    for (Character* c : alliedParty->GetMembers())
    {
        c->PrintDebugInfo();
    }
    // -----------------------------

    if (enemyParty == nullptr)
    {
        LOG("CombatScene: ERROR — enemyParty es nullptr tras CreateEnemyParty");
        return;
    }

    LOG("CombatScene: enemyParty tiene %d miembros:", enemyParty->GetMemberCount());
    for (Character* c : enemyParty->GetMembers())
    {
        LOG("  enemy -> %s", c->GetName().c_str());
    }

    // Combat is created after lane assignments are confirmed — see FinalizeLaneAssignments()
    // Start the lane selection phase immediately
    laneAssignmentCursor = 0;
    laneAssignments.clear();
    ShowLaneSelectionFor(laneAssignmentCursor);
}

void CombatScene::Update(float dt)
{
    potionCount = alliedParty->GetInventory().GetItemCount("consumable");

    // Dibujar background cada frame
    Engine::GetInstance().render->DrawTexture(background, 0, 0);

    //play music
    if (!Engine::GetInstance().audio->IsMusicPlaying()) {
        LOG("Play music again!");
        Engine::GetInstance().audio->PlayMusic(combMusic);
    }

    // Reset the click guard every frame so the next real click is accepted
    laneInputConsumed = false;
    combatInputConsumed = false;

    // Lane selection is still ongoing — don't run combat yet
    if (uiState == CombatUIState::SELECTING_LANE)
    {
        DrawUILaneSelection(currentSelecting);
        return;
    }

    // Testing: F1 = victory, F2 = defeat
    if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
    {
        LOG("CombatScene [TEST]: forzando VICTORIA.");
        combat->ForceVictory();
    }
    if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
    {
        LOG("CombatScene [TEST]: forzando DERROTA.");
        combat->ForceDefeat();
    }

    for (Character* c : combat->GetAllCombatants()) {
        //call update of every character (animations)
        c->Update(dt);
    }

    //draw status icons
    DrawStatusIcons();

    //------Draw panels--------------------
    DrawAlliedPanels();
    DrawEnemyPanels();

    DrawArrowCurrentActor();
    UpdateNextRoundPause(dt);
    DrawSkillCosts();

    //------Gestionar UI si es turno del jugador--------
    UpdateCombatUI();

    //-------Hovered Skill----------------
    UpdateSkillHover();
    DrawSkillTooltip();

    //ShowCurrentHP();

    if (!combat->GetWaitingForInput())
    {
        // Iterar hasta que el combate necesite esperar input externo
        // (input del jugador o animación) o haya terminado
        bool keepRunning = true;

        while (keepRunning)
        {
            combat->Run();

            if (combat->CombatIsFinished()) { keepRunning = false; }
            else if (combat->GetWaitingForInput()) { keepRunning = false; }
            else if (combat->IsWaitingAnimation()) { keepRunning = false; }
            else if (combat->IsNextRoundPause()) { keepRunning = false; }
        }
        //combat->Run();
    }

    //Draw Potions UI
    if(potionCount==0){ Engine::GetInstance().render->DrawTexture(potionEmpty, POTION_BOUNDS.x, POTION_BOUNDS.y); }

    SDL_Color White = { 255, 255, 255 };
    SDL_Color Red = { 255, 0, 0 };

    std::string potions = std::to_string(potionCount);
    if(potionCount>=10){ Engine::GetInstance().render->DrawText((potions).c_str(), TEXT1_BOUNDS.x, TEXT1_BOUNDS.y, TEXT1_BOUNDS.w, TEXT1_BOUNDS.h, White); }
    else if(potionCount>0){ Engine::GetInstance().render->DrawText((potions).c_str(), TEXT1_BOUNDS.x, TEXT1_BOUNDS.y, TEXT1_BOUNDS.w, TEXT1_BOUNDS.h, White); }
    else{ Engine::GetInstance().render->DrawText((potions).c_str(), TEXT1_BOUNDS.x, TEXT1_BOUNDS.y, TEXT1_BOUNDS.w, TEXT1_BOUNDS.h, Red); }
    

    // Al acabar el combate volvemos a InGameScene (que quedó suspendida)
    if(combat->CombatIsFinished())
    {
        if (!resultPanelActive)
        {
            HideCombatUI();
            bool won = (combat->GetResult() == CombatResult::VICTORY);
            ShowResultPanel(won);
        }
        DrawResultPanel();
    }
}

void CombatScene::PostUpdate(float dt) 
{

}

void CombatScene::Unload()
{
    LOG("CombatScene: descargando...");
    HideCombatUI();

    //textures
    Engine::GetInstance().textures->UnLoad(background);
    Engine::GetInstance().textures->UnLoad(abilityIcons);
    Engine::GetInstance().textures->UnLoad(panelBaseTexture);
    Engine::GetInstance().textures->UnLoad(hpBarChunkTexture);
    Engine::GetInstance().textures->UnLoad(initiativeBarChunkTexture);
    Engine::GetInstance().textures->UnLoad(nextRound);
    Engine::GetInstance().textures->UnLoad(arrow);
    Engine::GetInstance().textures->UnLoad(poisonIcon);
    Engine::GetInstance().textures->UnLoad(burnIcon);
    Engine::GetInstance().textures->UnLoad(potionIcon);

    Engine::GetInstance().textures->UnLoad(backLose);
    Engine::GetInstance().textures->UnLoad(backWin);
    Engine::GetInstance().textures->UnLoad(continueLose);
    Engine::GetInstance().textures->UnLoad(continueWin);
    Engine::GetInstance().textures->UnLoad(potionEmpty);

    for (auto& pair : characterIcons)
    {
        Engine::GetInstance().textures->UnLoad(pair.second);
    }
    characterIcons.clear();
    delete combat;
    combat = nullptr;

    DestroyEnemyParty();
}

void CombatScene::LoadTextures()
{
    abilityIcons = Engine::GetInstance().textures->Load("Assets/Textures/CombatScene/AbilityIcons.png");
    background = Engine::GetInstance().textures->Load("Assets/Textures/Backgrounds/BattleBackground.png");
    panelBaseTexture = Engine::GetInstance().textures->Load("Assets/Textures/CombatScene/Panel.png");
    hpBarChunkTexture = Engine::GetInstance().textures->Load("Assets/Textures/CombatScene/HealthPoint.png");
    initiativeBarChunkTexture = Engine::GetInstance().textures->Load("Assets/Textures/CombatScene/InitiativePoint.png");
    nextRound = Engine::GetInstance().textures->Load("Assets/Textures/CombatScene/nextRound.png");
    arrow = Engine::GetInstance().textures->Load("Assets/Textures/CombatScene/ArrowMarker.png");
    poisonIcon = Engine::GetInstance().textures->Load("Assets/Textures/CombatScene/PoisonIndicator.png");
    burnIcon = Engine::GetInstance().textures->Load("Assets/Textures/CombatScene/FireIndicator.png");
    potionIcon = Engine::GetInstance().textures->Load("Assets/Textures/CombatScene/potionIcon.png");

    continueLose = Engine::GetInstance().textures->Load("Assets/Textures/CombatScene/continueLose.png");
    continueWin = Engine::GetInstance().textures->Load("Assets/Textures/CombatScene/continueWin.png");
    backLose = Engine::GetInstance().textures->Load("Assets/Textures/CombatScene/lose.png");
    backWin = Engine::GetInstance().textures->Load("Assets/Textures/CombatScene/win.png");
    potionEmpty = Engine::GetInstance().textures->Load("Assets/Textures/CombatScene/potionEmpty.png");
    emptyButton = Engine::GetInstance().textures->Load("Assets/Textures/CombatScene/emptyButton.png");
    laneMarkus = Engine::GetInstance().textures->Load("Assets/Textures/CombatScene/laneMarkus.png");
    laneTheresia = Engine::GetInstance().textures->Load("Assets/Textures/CombatScene/laneTheresia.png");
    laneGerbera = Engine::GetInstance().textures->Load("Assets/Textures/CombatScene/laneGerbera.png");
    laneFatuus = Engine::GetInstance().textures->Load("Assets/Textures/CombatScene/laneFatuus.png");
    laneJochi = Engine::GetInstance().textures->Load("Assets/Textures/CombatScene/laneMarkus.png");
    laneIgnis = Engine::GetInstance().textures->Load("Assets/Textures/CombatScene/laneIgnis.png");

    // Cargar icono de cada personaje dinámicamente desde las parties
    auto loadIconForParty = [&](Party* party)
        {
            for (Character* c : party->GetMembers())
            {
                const std::string& name = c->GetName();
                if (characterIcons.find(name) != characterIcons.end()) { continue; } // ya cargado

                std::string folder;
                if (party->GetMembers()[0]->GetIsAllied()) {
                    folder = "allied";
                }
                else {
                    folder = "enemy";
                }
                std::string path = "Assets/Textures/CombatScene/Icons/" + folder + "/" + name + "Icon.png";
                characterIcons[name] = Engine::GetInstance().textures->Load(path.c_str());
            }
        };

    loadIconForParty(alliedParty);
    loadIconForParty(enemyParty);
}

bool CombatScene::OnUIMouseClickEvent(UIElement* uiElement)
{
    switch (uiElement->id)
    {
        // ---------- SKILLS ----------
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    {
        if (combatInputConsumed) { break; }
        combatInputConsumed = true;

        selectedSkillIdx = uiElement->id - 1;
        ShowTargetPanel();
        break;
    }

    // ---------- TARGETS ----------
    case 10:
    case 11:
    case 12:
    {
        if (combatInputConsumed) { break; }
        combatInputConsumed = true;

        int targetIndex = uiElement->id - 10;

        //Play Sound
        ChooseSound(combat->GetCurrentActor()->GetSkills()[selectedSkillIdx].GetAnimationId());

        combat->SubmitPlayerChoice(selectedSkillIdx, targetIndex);

        HideCombatUI();

        selectedSkillIdx = -1;

        break;
    }

    // ---------- BACK ----------
    case 20:
    {
        if (combatInputConsumed) { break; }
        combatInputConsumed = true;

        ShowSkillButtons();
        break;
    }

    // ---- LANE SELECTION ----------
    case 30: // Back lane
    case 31: // Side lane
    case 32: // Front lane
    {
        if (laneInputConsumed)
        {
            break;
        }
        laneInputConsumed = true;

        LaneType chosen;
        if (uiElement->id == 30)
        {
            chosen = LaneType::BACK;
        }
        else if (uiElement->id == 31)
        {
            chosen = LaneType::SIDE;
        }
        else
        {
            chosen = LaneType::FRONT;
        }

        Character* c = alliedParty->GetMembers()[laneAssignmentCursor];
        laneAssignments[c] = chosen;

        LOG("CombatScene: %s -> lane %d", c->GetName().c_str(), (int)chosen);

        laneAssignmentCursor++;

        if (laneAssignmentCursor < (int)alliedParty->GetMemberCount())
        {
            ShowLaneSelectionFor(laneAssignmentCursor);
        }
        else
        {
            FinalizeLaneAssignments();
        }
        break;
    }
    case 40: // PASS
    {
        if (combatInputConsumed) { break; }
        combatInputConsumed = true;

        combat->SubmitPlayerChoice(-1, 0);
        HideCombatUI();
        selectedSkillIdx = -1;
        break;
    }
    case 50: // POCIÓN
    {
        if (combatInputConsumed) { break; }
        combatInputConsumed = true;

        int potions = alliedParty->GetInventory().GetItemCount("consumable");
        if (potions <= 0)
        {
            LOG("CombatScene: no hay pociones.");
            break;
        }

        Character* actor = combat->GetCurrentActor();
        if (actor == nullptr) { break; }

        alliedParty->GetInventory().ConsumeItem("consumable");
        
        actor->Heal(POTION_HEAL_AMOUNT);

        LOG("CombatScene: %s usa poción — cura %d HP. HP ahora: %d/%d",
            actor->GetName().c_str(), POTION_HEAL_AMOUNT,
            actor->GetCurrentHP(), actor->GetMaxHP());

        HideCombatUI();
        selectedSkillIdx = -1;

        // Consumir turno igual que el pass
        combat->SubmitPlayerChoice(-1, 0);

        if (alliedParty->GetInventory().GetItemCount("consumable") == 0) { Engine::GetInstance().uiManager->RemoveElementsByRange(49, 51); }
        break;
    }
    case 60: // Continue — cerrar panel de resultado
    {
        resultPanelActive = false;
        bool won = (combat->GetResult() == CombatResult::VICTORY);
        if (onCombatEnd)
        {
            onCombatEnd(won);
        }
        Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/Map.wav");
        Engine::GetInstance().scene->PopScene();
        break;
    }
    default:
        break;
    }

    return true;
}

#pragma region LANE SELECTION
// ------------ Lane selection ---------------------------
void CombatScene::ShowLaneSelectionFor(int characterIndex)
{
    HideCombatUI();
    uiState = CombatUIState::SELECTING_LANE;

    Character* c = alliedParty->GetMembers()[characterIndex];

    LOG("CombatScene: selecting lane for %s", c->GetName().c_str());

    currentSelecting = c->GetName();

    // Three lane buttons stacked vertically in the center of the screen
    // Label shows the lane name and its bonus so the player can make an informed choice


    struct LaneOption
    {
        int id;
        LaneType type;
    };

    LaneOption options[3] = {

        { 30, LaneType::BACK },
        { 31, LaneType::SIDE },
        { 32, LaneType::FRONT }
    };

    std::string labels[3] = {
    "+" + std::to_string(15 * shipLevel) + " Power",
    "+" + std::to_string(10 * shipLevel) + " Power  +" + std::to_string(10 * shipLevel) + " Speed",
    "+" + std::to_string(10 * shipLevel) + " Speed  +" + std::to_string(10 * shipLevel) + " HP"
    };


    for (int i = 0; i < 3; i++)
    {
        // Skip lanes already assigned to a previous character
        if (IsLaneTaken(options[i].type))
        {
            continue;
        }

        SDL_Rect bounds;
        bounds.x = 539;
        bounds.y = 360 + i * 71;
        bounds.w = 202;
        bounds.h = 63;

        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON,
            options[i].id,
            labels[i].c_str(),
            bounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); },
            {}, emptyButton, 0, bounds.w, bounds.h
        );
    }
}

void CombatScene::FinalizeLaneAssignments()
{
    HideCombatUI();

    // Now that every character has a lane, create the Combat object
    combat = new Combat(alliedParty, enemyParty, shipLevel);

    for (auto& pair : laneAssignments)
    {
        combat->AssignLane(pair.first, pair.second);
    }

    LOG("CombatScene: all lanes assigned, starting combat.");
}

bool CombatScene::IsLaneTaken(LaneType laneType) const
{
    for (const auto& pair : laneAssignments)
    {
        if (pair.second == laneType)
        {
            return true;
        }
    }
    return false;
}
#pragma endregion

void CombatScene::DrawArrowCurrentActor()
{
    Character* c = combat->GetCurrentActor();
    if (c == nullptr) { return; }
    Vector2D position { 0,0 };
    position.setX((int)c->GetPosition().getX() - (arrow->w / 2));
    position.setY((int)c->GetPosition().getY() - 100);
    Engine::GetInstance().render->DrawTexture(arrow, position.getX(), position.getY());
}

#pragma region SKILL_HOVER
void CombatScene::UpdateSkillHover()
{
    if (uiState != CombatUIState::SELECTING_SKILL) {
        hoveredSkillIdx = -1;
        return;
    }

    int mouseX, mouseY;

    Vector2D pos = Engine::GetInstance().input->GetMousePosition();

    mouseX = pos.getX();
    mouseY = pos.getY();

    hoveredSkillIdx = -1;

    for (auto e : Engine::GetInstance().uiManager->UIElementsList)
    {
        if (e->id >= 1 && e->id <= 5) // skills
        {
            SDL_Rect r = e->bounds;

            if (mouseX >= r.x && mouseX <= r.x + r.w &&
                mouseY >= r.y && mouseY <= r.y + r.h)
            {
                hoveredSkillIdx = e->id - 1;
                break;
            }
        }
    }
}

void CombatScene::DrawSkillTooltip()
{
    if (hoveredSkillIdx == -1) { return; }

    Character* actor = combat->GetCurrentActor();
    if (actor == nullptr) { return; }

    auto& skills = actor->GetSkills();
    if (hoveredSkillIdx >= (int)skills.size()) { return; }

    tooltipRenderer.Draw(
        skills[hoveredSkillIdx].GetFullDescription(),
        (int)Engine::GetInstance().window->width / 3,
        600
    );
}
#pragma endregion

void CombatScene::ChooseSound(std::string id)
{
    for (AbilitiesSounds sound : abilities) {
        if (sound.idSound == id) {
            Engine::GetInstance().audio->PlayFx(sound.fxSound);
        }
    }
}

#pragma region CHARACTER PANELS

void CombatScene::ShowResultPanel(bool victory)
{
    resultPanelActive = true;
    resultPanelIsVictory = victory;

    if(victory){ Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/EndCombatWin.wav"); }
    else{ Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/EndCombatLose.wav"); }

    if (victory)
    {
        ApplyPostCombatRewards();
        // Crear botón Continue (id 60)
        SDL_Rect btnBounds = CONTINUE_BOUNDS;
        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON, 60, "",
            btnBounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); },
            {}, continueWin, 0, btnBounds.w, btnBounds.h);
    }
    else
    {
        // Daño al barco — puedes ajustar la fórmula
        shipDamage = 25;

        ApplyPostCombatRewards();
        // Crear botón Continue (id 60)
        SDL_Rect btnBounds = CONTINUE_BOUNDS;
        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON, 60, "",
            btnBounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); },
            {}, continueLose, 0, btnBounds.w, btnBounds.h);
    }


    
}

void CombatScene::DrawResultPanel()
{
    if (!resultPanelActive) { return; }

    // Fondo semitransparente
    SDL_Rect overlay = { 340, 150, 600, 430 };
    Engine::GetInstance().render->DrawRectangle(overlay, 0, 0, 0, 180, true, false);

    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color yellow = { 255, 220,   0, 255 };
    SDL_Color green = { 80, 255,  80, 255 };

    if (resultPanelIsVictory)
    {
        //Engine::GetInstance().render->DrawText("VICTORY", 460, 170, 220, 50, yellow);
        Engine::GetInstance().render->DrawTexture(backWin, 0, 0);
        // Oro ganado
        std::string goldText = "Gold gained: " + std::to_string(goldGained);
        Engine::GetInstance().render->DrawText(goldText.c_str(), 400, 240, 300, 30, white);

        // XP por personaje
        int lineY = 285;
        for (const CharXPSnapshot& snap : xpSnapshots)
        {
            std::string line = snap.name + ":  "
                + std::to_string(snap.xpBefore) + "  -  "
                + std::to_string(snap.xpAfter);

            if (snap.leveledUp)
            {
                line += "  [LEVEL UP]";
            }

            Engine::GetInstance().render->DrawText(line.c_str(), 380, lineY, 520, 28, snap.leveledUp ? green : white);
            lineY += 40;
        }
    }
    else
    {
        Engine::GetInstance().render->DrawTexture(backLose, 0, 0);
        //Engine::GetInstance().render->DrawText("DEFEAT", 470, 170, 200, 50, { 255, 60, 60, 255 });

        std::string dmgText = "Ship damage: -" + std::to_string(shipDamage);
        Engine::GetInstance().render->DrawText(dmgText.c_str(), 420, 260, 300, 30, white);
    }
}

void CombatScene::ApplyPostCombatRewards()
{
    xpSnapshots.clear();

    int xpReward = enemyParty->GetTotalXPReward();
    goldGained = enemyParty->GetTotalGoldReward();

    for (Character* c : alliedParty->GetMembers())
    {
        CharXPSnapshot snap;
        snap.name = c->GetName();
        snap.xpBefore = c->GetXP();

        int levelBefore = c->GetLevel();
        c->GainExperience(xpReward);
        int levelAfter = c->GetLevel();

        snap.xpAfter = c->GetXP();
        snap.leveledUp = (levelAfter > levelBefore);

        xpSnapshots.push_back(snap);
    }

    alliedParty->AddGold(goldGained);
}

void CombatScene::DrawAlliedPanels()
{
    auto& members = alliedParty->GetMembers();
    for (int i = 0; i < (int)members.size(); i++)
    {
        int panelY = ALLIED_PANEL_START_Y + i * PANEL_VERTICAL_GAP;
        DrawCharacterPanel(members[i], ALLIED_PANEL_X, panelY, true);
    }
}

void CombatScene::DrawEnemyPanels()
{
    auto& members = enemyParty->GetMembers();
    for (int i = 0; i < (int)members.size(); i++)
    {
        int panelY = ENEMY_PANEL_START_Y + i * PANEL_VERTICAL_GAP;
        DrawCharacterPanel(members[i], ENEMY_PANEL_X, panelY, false);
    }
}

void CombatScene::DrawCharacterPanel(Character* c, int panelX, int panelY, bool isAlly)
{
    // 1 — fondo del panel
    SDL_Rect panelDest = { panelX, panelY, PANEL_W, PANEL_H };
    Engine::GetInstance().render->DrawTexture(panelBaseTexture, panelX, panelY, nullptr, 1.0f, 0.0, INT_MAX, INT_MAX, !isAlly);

    // 2 — icono del personaje
    auto it = characterIcons.find(c->GetName());
    if (it != characterIcons.end() && it->second != nullptr)
    {
        int iconX;
        if (isAlly)
        {
            iconX = panelX + ICON_OFFSET_X;
        }
        else
        {
            iconX = panelX + PANEL_W - ICON_W;
        }
        int iconY = panelY + ICON_OFFSET_Y;
        Engine::GetInstance().render->DrawTexture(it->second, iconX, iconY, nullptr, false);
    }

    int hpBarX;
    int initBarX;
    if (isAlly)
    {
        hpBarX = panelX + HP_BAR_OFFSET_X;
        initBarX = panelX + INIT_BAR_OFFSET_X;
    }
    else
    {
        hpBarX = panelX + HP_BAR_OFFSET_X_ENEMY;
        initBarX = panelX + INIT_BAR_OFFSET_X_ENEMY;
    }

    int hpBarY = panelY + HP_BAR_OFFSET_Y;
    int initBarY = panelY + INIT_BAR_OFFSET_Y;

    DrawHealthBar(hpBarX, hpBarY, c->GetCurrentHP(), c->GetMaxHP(), isAlly);
    DrawInitiativeBar(initBarX, initBarY, c->GetCurrentInitiative(), isAlly);
}

void CombatScene::DrawHealthBar(int x, int y, int currentHP, int maxHP, bool leftToRight)
{
    if (maxHP <= 0) { return; }

    float hpPerChunk = maxHP / (float)HP_MAX_CHUNKS;
    int filledChunks = std::min((int)(currentHP / hpPerChunk), HP_MAX_CHUNKS);
    filledChunks = std::max(filledChunks, 0);

    for (int i = 0; i < filledChunks; i++)
    {
        int chunkX;
        if (leftToRight)
        {
            chunkX = x + i * (HP_CHUNK_W);
        }
        else
        {
            chunkX = x - i * (HP_CHUNK_W);
        }
        Engine::GetInstance().render->DrawTexture(hpBarChunkTexture, chunkX, y, nullptr, false);
    }
}

void CombatScene::DrawInitiativeBar(int x, int y, int currentInitiative, bool leftToRight)
{

    int clampedInit = std::max(std::min(currentInitiative, MAX_INITIATIVE), 0);
    int filledChunks = std::min((int)(clampedInit / (MAX_INITIATIVE / (float)INIT_MAX_CHUNKS)), INIT_MAX_CHUNKS);

    for (int i = 0; i < filledChunks; i++)
    {
        int chunkX;
        if (leftToRight)
        {
            chunkX = x + i * (INIT_CHUNK_W - BAR_CHUNK_OVERLAP);
        }
        else
        {
            chunkX = x - i * (INIT_CHUNK_W - BAR_CHUNK_OVERLAP);
        }
        Engine::GetInstance().render->DrawTexture(initiativeBarChunkTexture, chunkX, y, nullptr, false);
    }
}

#pragma endregion

//loads skills textures and buttons depending of the character attacking
void CombatScene::CreateSkillButtons(Character* c)
{
    std::string name = c->GetName();
    auto& skills = c->GetSkills();

    //load skills texture depending in the character
    std::string path = "Assets/Textures/CombatScene/" + name + "/AbilityIcons.png";

    abilityIcons2 = Engine::GetInstance().textures->Load(path.c_str());

    for (int i = 0; i < (int)skills.size(); ++i)
    {
        SDL_Rect bounds;
        bounds.x = (int)(Engine::GetInstance().window->width / 3) + (i * 70);
        bounds.y = 500;
        bounds.w = 64;
        bounds.h = 64;

        std::string label = skills[i].GetName();

        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON,
            i + 1, "",
            bounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, abilityIcons2, 0 + i, bounds.w, bounds.h
        );
    }
}

std::vector<std::string> CombatScene::GetEnemyNamesForFaction(IslandFaction faction) const
{
    switch (faction)
    {
    case IslandFaction::FISH:
        return { "Buck", "Fish1", "PeckandBubbles" };
    case IslandFaction::JELLYFISH:
        return { "ToxicJelly", "PinkJelly", "BlueJelly" };
    case IslandFaction::HUMANS:
        return { "Theresia", "Markus", "human3" };
    case IslandFaction::BIRD:
        return { "Jochi", "Ubo", "BigBird" };
    case IslandFaction::SIRENS:
        return { "Coral", "Maxine", "Pearl" };
    case IslandFaction::REPTILES:
        return { "Raptor", "Rex", "Chaman" };
    case IslandFaction::TRIBAL:
        return { "tribal3", "Ignis", "Gerbera" };
    default: // ELDRITCH / boss
        return { "Boss", "LilBuddy1", "LilBuddy2" };
    }
}

//  CreateEnemyParty
void CombatScene::CreateEnemyParty()
{
    enemyParty = new Party("Enemigos");

    std::vector<std::string> enemyNames = GetEnemyNamesForFaction(currentIslandFaction);

    for (int i = 0; i < 3; ++i)
    {
        Character* c = CharacterFactory::CreateDataOnly(enemyNames[i]);
        
        if (c != nullptr)
        {
            enemyParty->AddMember(c);
        }
        else
        {
            LOG("CombatScene: no se pudo crear el enemigo '%s'.", enemyNames[i]);
        }

        CharacterFactory::LoadVisualsFor(c, c->GetName());
        c->SetIsAllied(false);
    }

    // Recompensas del combate
    enemyParty->SetXPReward(50);
    enemyParty->SetGoldReward(20);

    LOG("CombatScene: party enemiga creada con %d miembros.", enemyParty->GetMemberCount());
}

void CombatScene::DestroyEnemyParty()
{
    if (enemyParty == nullptr) return;

    for (Character* c : enemyParty->GetMembers())
    {
        delete c;
    }

    delete enemyParty;
    enemyParty = nullptr;
}

void CombatScene::UpdateCombatUI()
{
    if (combat->GetWaitingForInput())
    {
        if (uiState == CombatUIState::HIDDEN)
        {
            ShowSkillButtons();
        }
    }
    else
    {
        if (uiState != CombatUIState::HIDDEN)
        {
            HideCombatUI();
        }
    }
}

void CombatScene::ShowSkillButtons()
{
    HideCombatUI();
    uiState = CombatUIState::SELECTING_SKILL;

    //Unloaad the previous ability icons and load the new ones
    Engine::GetInstance().textures->UnLoad(abilityIcons2);

    Character* actor = combat->GetCurrentActor();
    if (!actor) return;

    CreateSkillButtons(actor);

    // Botón de pasar turno — sin coste de iniciativa
    SDL_Rect passBounds = SKIP_BOUNDS;

    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON,
        40,
        "Pass",
        passBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); },
        {}, abilityIcons, 0, passBounds.w, passBounds.h
    );



    SDL_Rect potionBounds = POTION_BOUNDS;

    if(alliedParty->GetInventory().GetItemCount("consumable")!=0){
        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON, 50, "", potionBounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); },
            {}, potionIcon, 0, potionBounds.w, potionBounds.h
        );
    }

}

void CombatScene::ShowTargetPanel()
{
    HideCombatUI();
    uiState = CombatUIState::SELECTING_TARGET;

    auto enemies = combat->GetAliveEnemies();

    for (int i = 0; i < enemies.size(); i++)
    {
        SDL_Rect bounds;
        bounds.x = 260;
        bounds.y = 200 + i * 70;
        bounds.w = 64;
        bounds.h = 64;

        std::string label = enemies[i]->GetName();

        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON,
            10 + i, // IDs 10..12
            label.c_str(),
            bounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, abilityIcons, 0 + i, bounds.w, bounds.h
        );
    }

    SDL_Rect backBounds = { 260, 450, 64, 64 };

    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON,
        20,
        "",
        backBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {},  abilityIcons, 0, backBounds.w, backBounds.h
    );
}

void CombatScene::HideCombatUI()
{
    for (auto e : Engine::GetInstance().uiManager->UIElementsList)
    {
        e->CleanUp();
    }

    uiState = CombatUIState::HIDDEN;
}

void CombatScene::ShowCurrentHP()
{
    //allies
    for (int i = 0; i < alliedParty->GetMemberCount(); i++) {

        Character* c = alliedParty->GetMembers()[i];

        std::string name = c->GetName();
        int currentHp = c->GetCurrentHP();
        int maxHp = c->GetMaxHP();
        std::string text = "Character " + name + "|" + " Current hp " + std::to_string(currentHp) + "/" + std::to_string(maxHp);
        Engine::GetInstance().render->DrawText(text.c_str(), 20, 600 + (50*i), 200, 40, { 255, 255, 255, 255 });
    }

    //enemies
    for (int i = 0; i < enemyParty->GetMemberCount(); i++) {

        Character* c = enemyParty->GetMembers()[i];

        std::string name = c->GetName();
        int currentHp = c->GetCurrentHP();
        int maxHp = c->GetMaxHP();
        std::string text = "Character " + name + "|" + " Current hp " + std::to_string(currentHp) + "/" + std::to_string(maxHp);
        Engine::GetInstance().render->DrawText(text.c_str(), 500, 600 + (50 * i), 200, 40, { 255, 255, 255, 255 });
    }
}

void CombatScene::DrawSkillCosts()
{
    if (uiState != CombatUIState::SELECTING_SKILL) { return; }

    Character* actor = combat->GetCurrentActor();
    if (actor == nullptr) { return; }

    auto& skills = actor->GetSkills();
    int totalWidth = (int)skills.size() * SKILL_BTN_SPACING;
    int startX = (int)(Engine::GetInstance().window->width / 3);

    for (int i = 0; i < (int)skills.size(); ++i)
    {
        bool canAfford = actor->GetCurrentInitiative() >= skills[i].GetInitiativeCost();

        SDL_Rect costBg;
        costBg.x = startX + i * SKILL_BTN_SPACING;
        costBg.y = SKILL_BTN_Y + SKILL_BTN_H;
        costBg.w = SKILL_BTN_W;
        costBg.h = 20;

        // verde si puede usarla, rojo si no tiene iniciativa suficiente
        if (canAfford)
        {
            Engine::GetInstance().render->DrawRectangle(costBg, 0, 0, 0, 180, true, false);
        }
        else
        {
            Engine::GetInstance().render->DrawRectangle(costBg, 150, 0, 0, 180, true, false);
        }

        std::string costText = std::to_string(skills[i].GetInitiativeCost());
        Engine::GetInstance().render->DrawText(
            costText.c_str(),
            costBg.x, costBg.y,
            costBg.w, costBg.h,
            canAfford ? SDL_Color{ 255, 255, 255, 255 } : SDL_Color{ 255, 80, 80, 255 }
        );
    }
}

void CombatScene::UpdateNextRoundPause(float dt)
{
    if (!combat->IsNextRoundPause()) { return; }

    if (!nextRoundPauseActive)
    {
        // Primera vez que entramos — arranca el timer
        nextRoundPauseActive = true;
        nextRoundTimer = 0.0f;
    }

    nextRoundTimer += dt;
    DrawNextRoundBanner();

    if (nextRoundTimer >= NEXT_ROUND_PAUSE_DURATION)
    {
        nextRoundPauseActive = false;
        nextRoundTimer = 0.0f;
        combat->ResumeFromNextRoundPause();
    }
}

void CombatScene::DrawNextRoundBanner()
{
    Vector2D position{ 0.0f, Engine::GetInstance().window->height / 2.0f };
    Engine::GetInstance().render->DrawTexture(nextRound, (int)position.getX(), (int)position.getY());
}

void CombatScene::DrawStatusIcons()
{
    for (Character* c : combat->GetAllCombatants())
    {
        if (!c->GetIsAlive()) { continue; }

        int iconX = (int)c->GetPosition().getX() + STATUS_ICON_OFFSET_X;
        int iconY = (int)c->GetPosition().getY() - STATUS_ICON_Y_OFFSET;

        if (c->IsPoisoned())
        {
            Engine::GetInstance().render->DrawTexture(poisonIcon, iconX, iconY, nullptr, false);
            iconX += STATUS_ICON_SIZE + STATUS_ICON_GAP;
        }

        if (c->IsBurning())
        {
            Engine::GetInstance().render->DrawTexture(burnIcon, iconX, iconY, nullptr, false);
        }
    }
}

void CombatScene::DrawUILaneSelection(std::string charName)
{
    if(charName=="Gerbera"){ Engine::GetInstance().render->DrawTexture(laneGerbera, 0, 0); }
    else if (charName == "Markus") { Engine::GetInstance().render->DrawTexture(laneMarkus, 0, 0); }
    else if (charName == "Theresia") { Engine::GetInstance().render->DrawTexture(laneTheresia, 0, 0); }
    else if (charName == "Jochi") { Engine::GetInstance().render->DrawTexture(laneJochi, 0, 0); }
    else if (charName == "Fatuus") { Engine::GetInstance().render->DrawTexture(laneFatuus, 0, 0); }
    else if (charName == "Ignis") { Engine::GetInstance().render->DrawTexture(laneIgnis, 0, 0); }
   
 
}

void CombatScene::OnResume()
{
    CreateUI();
}

void CombatScene::OnPause()
{
    Engine::GetInstance().uiManager->CleanUp();
}

void CombatScene::CreateUI()
{
    if (uiState == CombatUIState::SELECTING_TARGET) {
        HideCombatUI();

        auto enemies = combat->GetAliveEnemies();

        for (int i = 0; i < enemies.size(); i++)
        {
            SDL_Rect bounds;
            bounds.x = 260;
            bounds.y = 200 + i * 70;
            bounds.w = 64;
            bounds.h = 64;

            std::string label = enemies[i]->GetName();

            Engine::GetInstance().uiManager->CreateUIElement(
                UIElementType::BUTTON,
                10 + i, // IDs 10..12
                label.c_str(),
                bounds,
                [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, abilityIcons, 0 + i, bounds.w, bounds.h
            );
        }

        SDL_Rect backBounds = { 260, 450, 64, 64 };

        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON,
            20,
            "< Back",
            backBounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, abilityIcons, 0, backBounds.w, backBounds.h
        );
    }
    else if (uiState == CombatUIState::SELECTING_SKILL) {
        HideCombatUI();

        Character* actor = combat->GetCurrentActor();
        if (!actor) return;

        auto& skills = actor->GetSkills();

        for (int i = 0; i < (int)skills.size(); ++i)
        {
            SDL_Rect bounds;
            bounds.x = 20;
            bounds.y = 200 + i * 70;
            bounds.w = 64;
            bounds.h = 64;

            std::string label = skills[i].GetName();

            Engine::GetInstance().uiManager->CreateUIElement(
                UIElementType::BUTTON,
                i + 1, // IDs 1..5
                label.c_str(),
                bounds,
                [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, abilityIcons, 0 + i, bounds.w, bounds.h
            );
        }

        // Botón Pass
        SDL_Rect passBounds;
        passBounds.x = 20;
        passBounds.y = 550;
        passBounds.w = 64;
        passBounds.h = 64;

        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON,
            40,
            "Pass",
            passBounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); },
            {}, abilityIcons, 0, passBounds.w, passBounds.h
        );
    }
    else if (uiState == CombatUIState::SELECTING_LANE)
    {
        ShowLaneSelectionFor(laneAssignmentCursor);
    }
}

void CombatScene::LoadSound() {
    buttonPress = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/UIfx/button_press.wav");
}
