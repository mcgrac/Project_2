#pragma once

// Forward declaration
class UIElement;

class IUIObserver
{
public:
    virtual ~IUIObserver() = default;
    virtual bool OnUIMouseClickEvent(UIElement* uiElement) = 0;
};
