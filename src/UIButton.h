#pragma once

#include "UIElement.h"
#include "Vector2D.h"

struct SDL_Texture;

class UIButton : public UIElement
{

public:

	UIButton(int id, SDL_Rect bounds, const char* text, SDL_Texture* _texture, int _spriteCol);
	virtual ~UIButton();

	// Called each loop iteration
	bool Update(float dt);

	bool CleanUp() override;

private:

	bool canClick = true;
	bool drawBasic = false;

	//textures
	SDL_Texture* spritesheet;

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

	void DrawButton() const;

	int buttonWidth = 154;
	int buttonHeight = 60;

	int buttonRowFocused = 0;
	int buttonRowNormal = 1;
	int buttonAnimStart = 2;   // primera fila de animacion
	int butonAnimFrames = 4;   // cuantos frames tiene la animacion
};

#pragma once