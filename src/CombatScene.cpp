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

CombatScene::CombatScene(Party* _allied, int _shipLevel)
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
{
    sceneName = "CombatScene";
}

CombatScene::~CombatScene()
{
    DestroyEnemyParty();
}

void CombatScene::LoadSounds() {

    for (Character* c : alliedParty->GetMembers()) {
        for (int i = 0; i < c->GetSkills().size(); i++) {
            Skill skill = c->GetSkills()[i];

            AbilitiesSounds ability;

            std::string path = "Assets/Audio/Fx/" + c->GetName() + "/" + skill.GetAnimationId() + ".wav";
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

    // Al acabar el combate volvemos a InGameScene (que quedó suspendida)
    if(combat->CombatIsFinished())
    {
        HideCombatUI();
        bool won = (combat->GetResult() == CombatResult::VICTORY);
        if (onCombatEnd)
        {
            onCombatEnd(won);
        }
        Engine::GetInstance().scene->PopScene();
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

    // Cargar icono de cada personaje dinámicamente desde las parties
    auto loadIconForParty = [&](Party* party)
        {
            for (Character* c : party->GetMembers())
            {
                const std::string& name = c->GetName();
                if (characterIcons.find(name) != characterIcons.end()) { continue; } // ya cargado

                std::string path = "Assets/Textures/CombatScene/Icons/" + name + "_icon.png";
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

    // Three lane buttons stacked vertically in the center of the screen
    // Label shows the lane name and its bonus so the player can make an informed choice
    struct LaneOption
    {
        int id;
        LaneType type;
        const char* label;
    };

    LaneOption options[3] = {
        { 30, LaneType::BACK,  "Back  (+15 Power)" },
        { 31, LaneType::SIDE,  "Side  (+10 Power +10 Speed)" },
        { 32, LaneType::FRONT, "Front (+10 Speed +10 HP)"   }
    };

    for (int i = 0; i < 3; i++)
    {
        // Skip lanes already assigned to a previous character
        if (IsLaneTaken(options[i].type))
        {
            continue;
        }

        SDL_Rect bounds;
        bounds.x = 540;
        bounds.y = 220 + i * 80;
        bounds.w = 200;
        bounds.h = 60;

        Engine::GetInstance().uiManager->CreateUIElement(
            UIElementType::BUTTON,
            options[i].id,
            options[i].label,
            bounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); },
            {}, abilityIcons, 0, bounds.w, bounds.h
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
    //if (hoveredSkillIdx == -1) return;

    //Character* actor = combat->GetCurrentActor();
    //if (!actor) return;

    //auto& skills = actor->GetSkills();

    //if (hoveredSkillIdx >= skills.size()) return;

    //std::string text = skills[hoveredSkillIdx].GetFullDescription();

    //// fondo opcional
    //SDL_Rect bg = { 600, 400, 300, 150 };
    //Engine::GetInstance().render->DrawRectangle(bg, 0, 0, 0, 200);

    //// texto
    //Engine::GetInstance().render->DrawText(
    //    text.c_str(),
    //    610,
    //    410,
    //    100,
    //    80,
    //    { 255,255,255,255 }
    //);

    if (hoveredSkillIdx == -1) return;

    Character* actor = combat->GetCurrentActor();
    if (!actor) return;

    auto& skills = actor->GetSkills();
    if (hoveredSkillIdx >= (int)skills.size()) return;

    std::string text = skills[hoveredSkillIdx].GetFullDescription();

    int charWidth = 8;
    int lineHeight = 20;
    int padding = 10;

    int maxCharsPerLine = 35;
    auto lines = WrapText(text, maxCharsPerLine);

    // Calcular ancho del box según la línea más larga
    int maxLineLen = 0;
    for (const auto& line : lines)
    {
        if ((int)line.size() > maxLineLen)
        {
            maxLineLen = (int)line.size();
        }
    }

    int boxWidth = maxLineLen * charWidth + padding * 2;
    int boxHeight = (int)lines.size() * lineHeight + padding * 2;

    int startX = (int)Engine::GetInstance().window->width/3;
    int startY = 600;

    SDL_Rect bg = { startX, startY, boxWidth, boxHeight };

    // Fondo
    Engine::GetInstance().render->DrawRectangle(bg, 0, 0, 0, 200, true, false);

    // Borde
    Engine::GetInstance().render->DrawRectangle(bg, 255, 255, 255, 255, false, false);

    // Texto línea por línea
    int yOffset = 0;
    for (const auto& line : lines)
    {
        DrawColoredLine(line, startX + padding, startY + padding + yOffset);
        yOffset += lineHeight;
    }
}

std::vector<std::string> CombatScene :: WrapText(const std::string& text, int maxCharsPerLine)
{
    std::vector<std::string> lines;
    std::stringstream ss(text);
    std::string word;
    std::string currentLine;

    while (ss >> word)
    {
        if (currentLine.length() + word.length() + 1 > maxCharsPerLine)
        {
            lines.push_back(currentLine);
            currentLine = word;
        }
        else
        {
            if (!currentLine.empty()) currentLine += " ";
            currentLine += word;
        }
    }

    if (!currentLine.empty())
    {
        lines.push_back(currentLine);
    }

    return lines;
}

void CombatScene::DrawColoredLine(const std::string& line, int x, int y)
{
    //std::stringstream ss(line);
    //std::string word;

    //int offsetX = 0;

    //while (ss >> word)
    //{
    //    SDL_Color color = { 255, 255, 255, 255 };

    //    if (word.find("Heal") != std::string::npos)
    //    {
    //        color = { 0, 255, 0, 255 };
    //    }

    //    if (word.find("Damage") != std::string::npos)
    //    {
    //        color = { 255, 0, 0, 255 };
    //    }

    //    // dibujar palabra
    //    Engine::GetInstance().render->DrawText(
    //        word.c_str(),
    //        x + offsetX,
    //        y,
    //        80,   // ancho
    //        20,
    //        color
    //    );

    //    offsetX += 60; // espacio horizontal
    //}

    // Determinar color de toda la línea según palabra clave
    //SDL_Color color = { 255, 255, 255, 255 };

    //if (line.find("Heal") != std::string::npos)
    //{
    //    color = { 0, 255, 0, 255 };
    //}
    //else if (line.find("Damage") != std::string::npos)
    //{
    //    color = { 255, 80, 80, 255 };
    //}

    //// Dibujar la línea completa de una vez con tamaño fijo de carácter
    //int charWidth = 8;   // ajusta según tu fuente
    //int charHeight = 16;

    //int textW = (int)line.size() * charWidth;
    //int textH = charHeight;

    //Engine::GetInstance().render->DrawText(
    //    line.c_str(),
    //    x,
    //    y,
    //    textW,
    //    textH,
    //    color
    //);

    std::stringstream ss(line);
    std::string word;
    int offsetX = 0;
    int charWidth = 8;
    int charHeight = 16;

    while (ss >> word)
    {
        SDL_Color color = { 255, 255, 255, 255 };

        if (word.find("Heal") != std::string::npos)
        {
            color = { 0, 255, 0, 255 };
        }
        else if (word.find("Damage") != std::string::npos)
        {
            color = { 255, 80, 80, 255 };
        }

        int wordW = (int)word.size() * charWidth;

        Engine::GetInstance().render->DrawText(
            word.c_str(),
            x + offsetX,
            y,
            wordW,
            charHeight,
            color
        );

        offsetX += wordW + charWidth; // +charWidth = espacio entre palabras
    }
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

        //int iconX = panelX + ICON_OFFSET_X;
        //int iconY = panelY + ICON_OFFSET_Y;
        //SDL_Rect iconDest = { iconX, iconY, ICON_W, ICON_H };
        //Engine::GetInstance().render->DrawTexture(it->second, iconX, iconY, nullptr, false);
    }

    // 3 — barra de vida
    //int hpBarX = panelX + HP_BAR_OFFSET_X;
    //int hpBarY = panelY + HP_BAR_OFFSET_Y;


    //DrawHealthBar(hpBarX, hpBarY, c->GetCurrentHP(), c->GetMaxHP());

    //// 4 — barra de iniciativa
    //int initBarX = panelX + INIT_BAR_OFFSET_X;
    //int initBarY = panelY + INIT_BAR_OFFSET_Y;
    //DrawInitiativeBar(initBarX, initBarY, c->GetCurrentInitiative());

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
    //if (maxHP <= 0) return;

    //// Cada cuadradito es el 10% de la vida máxima del personaje
    //float hpPerChunk = maxHP / (float)HP_MAX_CHUNKS;
    //int filledChunks = (int)(currentHP / hpPerChunk);
    //filledChunks = std::min(filledChunks, HP_MAX_CHUNKS);
    //filledChunks = std::max(filledChunks, 0);

    //for (int i = 0; i < filledChunks; i++)
    //{
    //    int chunkX = x + i * (HP_CHUNK_W);
    //    Engine::GetInstance().render->DrawTexture(hpBarChunkTexture, chunkX, y, nullptr, false);
    //}

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
    // Cada cuadradito es el 10% de 250 (= 25 puntos de iniciativa)
    //int maxInitiative = 250;
    //int clampedInit = std::min(currentInitiative, maxInitiative);
    //clampedInit = std::max(clampedInit, 0);
    //int filledChunks = (int)(clampedInit / (maxInitiative / (float)INIT_MAX_CHUNKS));
    //filledChunks = std::min(filledChunks, INIT_MAX_CHUNKS);

    //for (int i = 0; i < filledChunks; i++)
    //{
    //    int chunkX = x + i * (INIT_CHUNK_W - 2);
    //    Engine::GetInstance().render->DrawTexture(initiativeBarChunkTexture, chunkX, y, nullptr, false);
    //}

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
            i + 1, // IDs 1..5
            label.c_str(),
            bounds,
            [this](UIElement* e) { return this->OnUIMouseClickEvent(e); }, {}, abilityIcons2, 0 + i, bounds.w, bounds.h
        );
    }
}

//  CreateEnemyParty
void CombatScene::CreateEnemyParty()
{
    enemyParty = new Party("Enemigos");

    //names of the enemies
    const char* enemyNames[] = { "Raptor", "Rex", "Chaman" };

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
    SDL_Rect passBounds;
    passBounds.x = 20;
    passBounds.y = 550;   // debajo de los botones de skill
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
