#pragma once

// Clase base abstracta para todas las escenas del juego.
// Cada pantalla (MainMenu, InGame, Pause, etc.) hereda de esta clase
// e implementa sus propios Load, Update, PostUpdate y Unload.
class UIElement;

class BaseScene
{
public:
    virtual ~BaseScene() = default;

    // Llamado una vez al cargar la escena
    virtual void Load() = 0;

    // Llamado cada frame mientras la escena está en el top de la pila
    virtual void Update(float dt) = 0;

    // Llamado cada frame después de Update (lógica de render / input tardío)
    virtual void PostUpdate(float dt) = 0;

    // Llamado una vez al descargar la escena
    virtual void Unload() = 0;

    // Llamado para cargar texturas de escenas
    virtual void LoadTextures() = 0;

    virtual bool OnUIMouseClickEvent(UIElement* uiElement) { return true; }
};
