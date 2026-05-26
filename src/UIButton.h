#pragma once

#include "UIElement.h"
#include "Vector2D.h"

struct SDL_Texture;

class UIButton : public UIElement
{

public:

	UIButton(int id, SDL_Rect bounds, const char* text, SDL_Texture* _texture, int _spriteCol, int _btnWidth, int _btnHeight);
	virtual ~UIButton();

	// Called each loop iteration
	bool Update(float dt) override;

	bool CleanUp() override;

	void SetDisabledRow(int row);  // permite elegir qué fila dibujar cuando está disabled
	void SetDisabled(bool disabled);
	bool IsDisabled() const;

private:

	bool canClick = true;
	bool drawBasic = false;

	//textures
	SDL_Texture* spritesheet;

	//font 
	TTF_Font* font = nullptr;
	SDL_Color textColor = { 255, 255, 255, 255 };

	int spriteCol;

	int widthtexture;
	int heightTexture;

	// Animacion post-click
	bool  playingAnim;
	int   animFrame;                // frame actual (0-3)
	float animTimer;                // tiempo acumulado en el frame actual
	float animFrameDuration;        // segundos por frame

	// Returns SDL_rect spritesheet
	SDL_Rect GetFrameRect(int row) const;

	// Apply tint
	void SetTint(Uint8 r, Uint8 g, Uint8 b) const;
	void ResetTint() const;

	bool Draw() override;

	int buttonWidth;
	int buttonHeight;

	int buttonRowNormal = 0;
	int buttonRowFocused = 1;
	int buttonRowDisabeled = 2;
	//int buttonAnimStart = 2;   // primera fila de animacion
	//int butonAnimFrames = 4;   // cuantos frames tiene la animacion

	//sound
	int PressedFx;
	int SelectedFx;
};
#pragma once