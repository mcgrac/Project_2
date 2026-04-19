#pragma once

#include "Module.h"
#include <map>
#include <string>

#include "SDL3_ttf/SDL_ttf.h"


class Fonts : public Module
{
public:

    Fonts();
    virtual ~Fonts();

    bool Start() override;
    bool CleanUp() override;

    // Cargar fuente
    TTF_Font* LoadFont(const std::string& path, int size);

    // Crear textura de texto
    SDL_Texture* CreateTextTexture(const std::string& text, SDL_Color color, TTF_Font* font);

private:

    std::map<std::string, TTF_Font*> loadedFonts;
};