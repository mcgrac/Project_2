#pragma once
#include "BaseScene.h"
#include "Quest.h"
#include <vector>

struct SDL_Texture;

class QuestScene : public BaseScene
{
public:
    QuestScene() = default;
    ~QuestScene() = default;

    void Load()            override;
    void Update(float dt)  override;
    void PostUpdate(float dt) override;
    void Unload()          override;
    void LoadTextures()    override;
    bool OnUIMouseClickEvent(UIElement* uiElement) override;

    void OnResume() override;
    void OnPause() override;

private:
    void DrawBackground();
    void DrawPanels();
    void DrawQuestEntry(const Quest& quest, int x, int y, int width, bool isCompleted);

    SDL_Texture* backgroundTex = nullptr;
    SDL_Texture* panelTex = nullptr;
    SDL_Texture* entryTex = nullptr;
    SDL_Texture* backButtonTex = nullptr;
    SDL_Texture* background = nullptr;
    SDL_Texture* arrowLeftTex = nullptr;
    SDL_Texture* arrowRightTex = nullptr;

    int currentPage = 0;
    static constexpr int ENTRIES_PER_PAGE = 5;

    static constexpr int BACK_BUTTON_ID = 1;
    static constexpr int PAGE_PREV_ID = 2;
    static constexpr int PAGE_NEXT_ID = 3;

    //missions text size
    static constexpr int PANEL_MARGIN = 20;
    static constexpr int ENTRY_HEIGHT = 90;
    static constexpr int ENTRY_PADDING = 10;
    static constexpr int TOP_OFFSET = 112;
    static constexpr int BOTTOM_OFFSET = 38;
    static constexpr int CHAR_W = 8;
    static constexpr int LINE_H = 20;

};