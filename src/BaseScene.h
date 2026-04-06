#include <string>
#pragma once

class UIElement;

class BaseScene
{
public:
    std::string sceneName = " ";
    BaseScene() {};
    ~BaseScene() {};

    virtual void Load() = 0;
    virtual void Update(float dt) = 0;
    virtual void PostUpdate(float dt) = 0;
    virtual void Unload() = 0;
    virtual void LoadTextures() = 0;

    virtual bool OnUIMouseClickEvent(UIElement* uiElement) { return true; }

    //helpers to activate/desactivate scene
    virtual void OnResume() {}
    virtual void OnPause() {}
};
