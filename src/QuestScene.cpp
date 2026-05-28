#include "QuestScene.h"
#include "QuestManager.h"
#include "SceneUtils.h"
#include "Engine.h"
#include "Textures.h"
#include "Render.h"
#include "Window.h"
#include "UIManager.h"
#include "Scene.h"
#include "Log.h"
#include <SDL3/SDL.h>
#include <string>

void QuestScene::Load()
{
    sceneName = "QuestScene";
    LoadTextures();

    int screenW = 0;
    int screenH = 0;
    Engine::GetInstance().window->GetWindowSize(screenW, screenH);

    SDL_Rect backBounds = { screenW / 2 - 80, screenH - 60, 160, 40 };
    auto backBtn = Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, BACK_BUTTON_ID, "VOLVER", backBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); },
        {}, backButtonTex, 0, backBounds.w, backBounds.h
    );
    backBtn->isHUD = true;

    LOG("QuestScene cargada.");
}


void QuestScene::Update(float dt)
{
    DrawBackground();
    DrawPanels();
}

void QuestScene::PostUpdate(float dt)
{
}


void QuestScene::Unload()
{
    Engine::GetInstance().textures->UnLoad(backgroundTex);
    Engine::GetInstance().textures->UnLoad(panelTex);
    Engine::GetInstance().textures->UnLoad(entryTex);
    Engine::GetInstance().textures->UnLoad(backButtonTex);

    backgroundTex = nullptr;
    panelTex = nullptr;
    entryTex = nullptr;
    backButtonTex = nullptr;

    Engine::GetInstance().uiManager->CleanUp();

    LOG("QuestScene descargada.");
}


void QuestScene::LoadTextures()
{
    backgroundTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/quest_background.png");
    panelTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/quest_panel.png");
    entryTex = Engine::GetInstance().textures->Load("Assets/Textures/UI/quest_entry.png");
    backButtonTex = Engine::GetInstance().textures->Load("Assets/Textures/MainMap/emptyButton.png");
}


bool QuestScene::OnUIMouseClickEvent(UIElement* uiElement)
{
    if (uiElement->id == BACK_BUTTON_ID)
    {
        Engine::GetInstance().scene->PopScene();
    }
    return true;
}

void QuestScene::OnResume()
{
    int screenW = 0;
    int screenH = 0;
    Engine::GetInstance().window->GetWindowSize(screenW, screenH);

    SDL_Rect backBounds = { screenW / 2 - 80, screenH - 60, 160, 40 };
    auto backBtn = Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, BACK_BUTTON_ID, "VOLVER", backBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); },
        {}, backButtonTex, 0, backBounds.w, backBounds.h
    );
    backBtn->isHUD = true;
}

void QuestScene::OnPause()
{
    Engine::GetInstance().uiManager->CleanUp();
}


void QuestScene::DrawBackground()
{
    if (backgroundTex != nullptr)
    {
        Engine::GetInstance().render->DrawTexture(backgroundTex, 0, 0);
    }
    else
    {
        // Fallback: fondo solido oscuro
        SDL_Renderer* ren = Engine::GetInstance().render->renderer;
        SDL_SetRenderDrawColor(ren, 15, 15, 30, 255);
        int screenW = 0;
        int screenH = 0;
        Engine::GetInstance().window->GetWindowSize(screenW, screenH);
        SDL_FRect bg = { 0, 0, (float)screenW, (float)screenH };
        SDL_RenderFillRect(ren, &bg);
    }

    // Titulo centrado
    int screenW = 0;
    int screenH = 0;
    Engine::GetInstance().window->GetWindowSize(screenW, screenH);
    const char* title = "MISSIONS";
    int titleW = (int)strlen(title) * CHAR_W;
    SceneUtils::DrawAutoText(title, screenW / 2 - titleW / 2, 15, { 255, 255, 255, 255 });
}


void QuestScene::DrawPanels()
{
    int screenW = 0;
    int screenH = 0;
    Engine::GetInstance().window->GetWindowSize(screenW, screenH);

    const int panelH = screenH - TOP_OFFSET - BOTTOM_OFFSET;
    const int halfW = screenW / 2 - PANEL_MARGIN * 2;

    const int leftX = PANEL_MARGIN;
    const int leftY = TOP_OFFSET;
    const int rightX = screenW / 2 + PANEL_MARGIN;
    const int rightY = TOP_OFFSET;

    SDL_Renderer* ren = Engine::GetInstance().render->renderer;

    auto DrawPanel = [&](int x, int y, int w, int h)
        {
            if (panelTex != nullptr)
            {
                SDL_Rect src = { 0, 0, w, h };
                Engine::GetInstance().render->DrawTexture(panelTex, x, y, &src);
            }
            else
            {
                SDL_SetRenderDrawColor(ren, 30, 30, 55, 220);
                SDL_FRect rect = { (float)x, (float)y, (float)w, (float)h };
                SDL_RenderFillRect(ren, &rect);
                SDL_SetRenderDrawColor(ren, 80, 80, 130, 255);
                SDL_RenderRect(ren, &rect);
            }
        };

    DrawPanel(leftX, leftY, halfW, panelH);
    DrawPanel(rightX, rightY, halfW, panelH);

    // Cabeceras de columna
    SceneUtils::DrawAutoText("ACTIVES", leftX + ENTRY_PADDING, leftY + ENTRY_PADDING, { 255, 215, 0,  255 });
    SceneUtils::DrawAutoText("COMPLETED", rightX + ENTRY_PADDING, rightY + ENTRY_PADDING, { 80,  200, 80, 255 });

    // Clasificar quests
    const std::vector<Quest>& allQuests = QuestManager::GetInstance().GetQuests();

    std::vector<const Quest*> active;
    std::vector<const Quest*> completed;

    for (const Quest& q : allQuests)
    {
        if (q.status == QuestStatus::ACTIVE)
        {
            active.push_back(&q);
        }
        else if (q.status == QuestStatus::COMPLETED)
        {
            completed.push_back(&q);
        }
    }

    const int entryStartY = TOP_OFFSET + 40;
    const int maxEntryBotY = TOP_OFFSET + panelH - ENTRY_PADDING;

    // Columna izquierda: activas
    int curY = entryStartY;
    for (const Quest* q : active)
    {
        if (curY + ENTRY_HEIGHT > maxEntryBotY)
        {
            break;
        }
        DrawQuestEntry(*q, leftX + ENTRY_PADDING, curY, halfW - ENTRY_PADDING * 2, false);
        curY += ENTRY_HEIGHT + ENTRY_PADDING;
    }

    // Columna derecha: completadas
    curY = entryStartY;
    for (const Quest* q : completed)
    {
        if (curY + ENTRY_HEIGHT > maxEntryBotY)
        {
            break;
        }
        DrawQuestEntry(*q, rightX + ENTRY_PADDING, curY, halfW - ENTRY_PADDING * 2, true);
        curY += ENTRY_HEIGHT + ENTRY_PADDING;
    }
}


void QuestScene::DrawQuestEntry(const Quest& quest, int x, int y, int width, bool isCompleted)
{
    SDL_Renderer* ren = Engine::GetInstance().render->renderer;

    // Fondo de la entrada
    if (entryTex != nullptr)
    {
        SDL_Rect src = { 0, 0, width, ENTRY_HEIGHT };
        Engine::GetInstance().render->DrawTexture(entryTex, x, y, &src);
    }
    else
    {
        SDL_SetRenderDrawColor(ren, 45, 45, 75, 255);
        SDL_FRect bg = { (float)x, (float)y, (float)width, (float)ENTRY_HEIGHT };
        SDL_RenderFillRect(ren, &bg);

        if (isCompleted)
        {
            SDL_SetRenderDrawColor(ren, 80, 200, 80, 255);
        }
        else
        {
            SDL_SetRenderDrawColor(ren, 100, 100, 160, 255);
        }
        SDL_RenderRect(ren, &bg);
    }

    const int textX = x + 8;
    int       textY = y + 6;

    // --- Nombre ---
    std::string displayName = quest.name;
    if (isCompleted)
    {
        displayName += "  (COMPLETED)";
    }

    SDL_Color nameColor;
    if (isCompleted)
    {
        nameColor = { 80, 200, 80, 255 };
    }
    else
    {
        nameColor = { 255, 215, 0, 255 };
    }
    SceneUtils::DrawAutoText(displayName.c_str(), textX, textY, nameColor);
    textY += LINE_H;

    // --- Descripcion ---
    SceneUtils::DrawAutoText(quest.description.c_str(), textX, textY, { 200, 200, 200, 255 });
    textY += LINE_H;

    // --- Progreso numerico (solo activas acumulativas) ---
    if (!isCompleted)
    {
        bool hasProgress = (quest.condition.type == QuestConditionType::DAMAGE_IN_COMBAT ||
            quest.condition.type == QuestConditionType::TOTAL_DAMAGE);
        if (hasProgress)
        {
            std::string progressStr = std::to_string(quest.progress) + " / "
                + std::to_string(quest.condition.amount);
            SceneUtils::DrawAutoText(progressStr.c_str(), textX, textY, { 160, 160, 255, 255 });
            textY += LINE_H;
        }
    }

    // --- Recompensa ---
    std::string rewardStr = "Reward: " + std::to_string(quest.rewardGold) + " gold";
    SceneUtils::DrawAutoText(rewardStr.c_str(), textX, textY, { 255, 200, 50, 255 });
}