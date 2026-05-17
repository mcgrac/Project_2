#pragma once
#include "BaseScene.h"
#include "Vector2D.h"
#include "SceneUtils.h"
#include <string>
#include <functional>
#include "SceneUtils.h"

struct SDL_Texture;

class DialogueScene : public BaseScene
{
public:

    DialogueScene(const std::string& dialogueId,
                  std::function<void()> onFinished = nullptr);
    ~DialogueScene();

    void Load() override;
    void Update(float dt) override;
    void PostUpdate(float dt) override;
    void Unload() override;
    void LoadTextures() override;

    bool OnUIMouseClickEvent(UIElement* uiElement) override;

    //helpers
    //void OnResume() override;
    //void OnPause() override;
    //void CreateUI();

private:


    std::string dialogueId;
    std::function<void()> onFinished; //callback to do an action when the dialogue finishes

    SDL_Texture* portraitTexture;
    SDL_Texture* panel;
    std::string currentPortraitPath = "";
   // SDL_Texture* backTexture;
    //std::string fullBackPath = "";
    SDL_Texture* button;

    TooltipRenderer tooltipRenderer;
    bool pendingRefresh;
  
    int positionX = 447;
    int positionY = 585;

    // Reconstruye los botones de opciones del nodo actual
    void RefreshOptionButtons();

    // Limpia solo los botones de opciones
    void ClearOptionButtons();

    // Carga el portrait del nodo actual si cambió
    //void UpdatePortrait(const std::string& portraitPath);

    // IDs de botones de opción: 10, 11, 12...
    static constexpr int OPTION_BTN_BASE = 10;
};
