#pragma once
#include "BaseScene.h"
#include <SDL3/SDL.h>
#include <string>
struct SDL_Texture;
struct SDL_Rect;

static float s_musicVolume = 1.0f;
static float s_sfxVolume = 1.0f;

class SettingsScene : public BaseScene
{
public:
    SettingsScene();
    ~SettingsScene();

    void Load() override;
    void Update(float dt) override;
    void PostUpdate(float dt) override;
    void Unload() override;
    void LoadTextures() override;
    bool OnUIMouseClickEvent(UIElement* uiElement) override;

private:
    SDL_Texture* buttonTexture;

    // Volúmenes actuales 0.0f - 1.0f
    float musicVolume;
    float sfxVolume;

    // Slider dragging state
    bool draggingMusic;
    bool draggingSFX;

    // Geometría de los sliders
    SDL_Rect musicTrack;
    SDL_Rect sfxTrack;
    static constexpr int TRACK_HEIGHT = 10;
    static constexpr int HANDLE_W = 20;
    static constexpr int HANDLE_H = 30;

    static constexpr int BACK_BTN_ID = 1;
    static constexpr int FULLSCREEN_BTN_ID = 2;

    // Helpers
    SDL_Rect GetMusicHandle() const;
    SDL_Rect GetSFXHandle()   const;
    SDL_Rect GetHandleFromVolume(const SDL_Rect& track, float volume) const;
    float GetVolumeFromMouseX(const SDL_Rect& track, int mouseX)   const;
};