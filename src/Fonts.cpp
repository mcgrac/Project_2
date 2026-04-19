#include "Fonts.h"
#include "Engine.h"
#include "Render.h"
#include "Log.h"

Fonts::Fonts() : Module()
{
    name = "Fonts";
}

Fonts::~Fonts() {}

bool Fonts::Start()
{
    if (TTF_Init() == -1)
    {
        LOG("Error inicializando SDL_ttf: %s", SDL_GetError());
        return false;
    }

    LOG("SDL_ttf inicializado correctamente");
    return true;
}

bool Fonts::CleanUp()
{
    for (auto& fontPair : loadedFonts)
    {
        if (fontPair.second != nullptr)
        {
            TTF_CloseFont(fontPair.second);
        }
    }

    loadedFonts.clear();

    TTF_Quit();

    return true;
}

TTF_Font* Fonts::LoadFont(const std::string& path, int size)
{
    std::string key = path + "_" + std::to_string(size);

    // Si ya está cargada, la devolvemos
    if (loadedFonts.find(key) != loadedFonts.end())
    {
        return loadedFonts[key];
    }

    TTF_Font* font = TTF_OpenFont(path.c_str(), size);

    if (font == nullptr)
    {
        //LOG("Error cargando fuente %s: %s", path.c_str(), TTF_GetError());
        return nullptr;
    }

    loadedFonts[key] = font;

    LOG("Fuente cargada: %s size %d", path.c_str(), size);

    return font;
}

SDL_Texture* Fonts::CreateTextTexture(const std::string& text, SDL_Color color, TTF_Font* font)
{
    if (font == nullptr) return nullptr;

    SDL_Surface* surface = TTF_RenderText_Blended(
        font,
        text.c_str(),
        text.length(),
        color
    );

    if (surface == nullptr)
    {
        LOG("Error creando surface de texto: %s", SDL_GetError());
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(
        Engine::GetInstance().render->renderer,
        surface
    );

    SDL_DestroySurface(surface);

    if (texture == nullptr)
    {
        LOG("Error creando textura de texto: %s", SDL_GetError());
    }

    return texture;
}