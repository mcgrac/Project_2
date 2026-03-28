#pragma once

// Forward declaration
class UIElement;

// Interfaz mínima para recibir eventos de UI.
// Tanto Module como BaseScene la implementan,
// así cualquiera puede ser observer de un UIElement.
class IUIObserver
{
public:
    virtual ~IUIObserver() = default;
    virtual bool OnUIMouseClickEvent(UIElement* uiElement) = 0;
};
