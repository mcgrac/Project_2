#include "SettingsScene.h"
#include "Engine.h"
#include "Scene.h"
#include "UIManager.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"
#include "Window.h"
#include "Input.h"
#include "Log.h"

SettingsScene::SettingsScene()
    : buttonTexture(nullptr),
    musicVolume(s_musicVolume), 
    sfxVolume(s_sfxVolume),   
    draggingMusic(false),
    draggingSFX(false),
    musicTrack({ 400, 300, 400, TRACK_HEIGHT }),
    sfxTrack({ 400, 400, 400, TRACK_HEIGHT })
{
    sceneName = "SettingsScene";
}

SettingsScene::~SettingsScene() {}

void SettingsScene::Load()
{
    LoadTextures();

    // Boton volver
    SDL_Rect backBounds = { 20, 20, 221, 85 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, BACK_BTN_ID, "Back", backBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); },
        {}, buttonTexture, 0, backBounds.w, backBounds.h
    );

    // Boton fullscreen
    SDL_Rect fsBounds = { 490, 480, 221, 85 };
    Engine::GetInstance().uiManager->CreateUIElement(
        UIElementType::BUTTON, FULLSCREEN_BTN_ID, "Fullscreen", fsBounds,
        [this](UIElement* e) { return this->OnUIMouseClickEvent(e); },
        {}, buttonTexture, 1, fsBounds.w, fsBounds.h
    );
}

void SettingsScene::Update(float dt)
{
    Vector2D mousePos = Engine::GetInstance().input->GetMousePosition();
    int mouseX = (int)mousePos.getX();
    int mouseY = (int)mousePos.getY();

    bool mouseDown = Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT
        || Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN;

    bool mousePressed = Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN;

    bool mouseReleased = Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP;

    if (mousePressed)
    {
        SDL_Rect musicHandle = GetMusicHandle();
        if (mouseX >= musicHandle.x && mouseX <= musicHandle.x + musicHandle.w &&
            mouseY >= musicHandle.y && mouseY <= musicHandle.y + musicHandle.h)
        {
            draggingMusic = true;
        }

        SDL_Rect sfxHandle = GetSFXHandle();
        if (mouseX >= sfxHandle.x && mouseX <= sfxHandle.x + sfxHandle.w &&
            mouseY >= sfxHandle.y && mouseY <= sfxHandle.y + sfxHandle.h)
        {
            draggingSFX = true;
        }
    }

    if (mouseDown)
    {
        if (draggingMusic)
        {
            musicVolume = GetVolumeFromMouseX(musicTrack, mouseX);
            s_musicVolume = musicVolume;
            Engine::GetInstance().audio->SetMusicVolume(musicVolume);
        }

        if (draggingSFX)
        {
            sfxVolume = GetVolumeFromMouseX(sfxTrack, mouseX);
            s_sfxVolume = sfxVolume;
            Engine::GetInstance().audio->SetSFXVolume(sfxVolume);
        }
    }

    if (mouseReleased)
    {
        draggingMusic = false;
        draggingSFX = false;
    }
}

void SettingsScene::PostUpdate(float dt)
{
    // Fondo semitransparente
    SDL_Rect bg = { 300, 200, 680, 380 };
    Engine::GetInstance().render->DrawRectangle(bg, 20, 20, 40, 230, true, false);

    // Titulo
    Engine::GetInstance().render->DrawText("SETTINGS", 540, 220, 200, 40, { 255, 215, 0, 255 });

    // --- Slider musica ---
    Engine::GetInstance().render->DrawText("Music", 300, 290, 80, 25, { 255, 255, 255, 255 });

    // Track
    Engine::GetInstance().render->DrawRectangle(musicTrack, 80, 80, 80, 255, true, false);

    // Parte rellena hasta el handle
    SDL_Rect musicFill = musicTrack;
    musicFill.w = (int)(musicVolume * musicTrack.w);
    Engine::GetInstance().render->DrawRectangle(musicFill, 100, 180, 255, 255, true, false);

    // Handle
    SDL_Rect musicHandle = GetMusicHandle();
    Engine::GetInstance().render->DrawRectangle(musicHandle, 220, 220, 255, 255, true, false);

    // Valor numerico
    std::string musicVal = std::to_string((int)(musicVolume * 100)) + "%";
    Engine::GetInstance().render->DrawText(musicVal.c_str(), 820, 290, 60, 25, { 255, 255, 255, 255 });

    // --- Slider SFX ---
    Engine::GetInstance().render->DrawText("SFX", 300, 390, 80, 25, { 255, 255, 255, 255 });

    // Track
    Engine::GetInstance().render->DrawRectangle(sfxTrack, 80, 80, 80, 255, true, false);

    // Parte rellena
    SDL_Rect sfxFill = sfxTrack;
    sfxFill.w = (int)(sfxVolume * sfxTrack.w);
    Engine::GetInstance().render->DrawRectangle(sfxFill, 100, 180, 255, 255, true, false);

    // Handle
    SDL_Rect sfxHandle = GetSFXHandle();
    Engine::GetInstance().render->DrawRectangle(sfxHandle, 220, 220, 255, 255, true, false);

    // Valor numerico
    std::string sfxVal = std::to_string((int)(sfxVolume * 100)) + "%";
    Engine::GetInstance().render->DrawText(sfxVal.c_str(), 820, 390, 60, 25, { 255, 255, 255, 255 });
}

void SettingsScene::Unload()
{
    Engine::GetInstance().uiManager->CleanUp();

    if (buttonTexture != nullptr)
    {
        Engine::GetInstance().textures->UnLoad(buttonTexture);
        buttonTexture = nullptr;
    }
}

void SettingsScene::LoadTextures()
{
    buttonTexture = Engine::GetInstance().textures->Load("Assets/Textures/Pause/ButtonsPause.png");
}

bool SettingsScene::OnUIMouseClickEvent(UIElement* uiElement)
{
    if (uiElement->id == BACK_BTN_ID)
    {
        Engine::GetInstance().scene->PopScene();
    }
    else if (uiElement->id == FULLSCREEN_BTN_ID)
    {
        Engine::GetInstance().window->ToggleFullscreen();
        Engine::GetInstance().render->UpdateCamera();
    }

    return true;
}

SDL_Rect SettingsScene::GetMusicHandle() const
{
    return GetHandleFromVolume(musicTrack, musicVolume);
}

SDL_Rect SettingsScene::GetSFXHandle() const
{
    return GetHandleFromVolume(sfxTrack, sfxVolume);
}

SDL_Rect SettingsScene::GetHandleFromVolume(const SDL_Rect& track, float volume) const
{
    int handleX = track.x + (int)(volume * track.w) - HANDLE_W / 2;
    int handleY = track.y + TRACK_HEIGHT / 2 - HANDLE_H / 2;
    return { handleX, handleY, HANDLE_W, HANDLE_H };
}

float SettingsScene::GetVolumeFromMouseX(const SDL_Rect& track, int mouseX) const
{
    float vol = (float)(mouseX - track.x) / (float)track.w;

    if (vol < 0.0f)
    {
        vol = 0.0f;
    }
    if (vol > 1.0f)
    {
        vol = 1.0f;
    }

    return vol;
}