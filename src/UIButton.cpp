#include "UIButton.h"
#include "Render.h"
#include "Engine.h"
#include "Audio.h"
#include "Log.h"
#include "Scene.h"

UIButton::UIButton(int id, SDL_Rect bounds, const char* text, SDL_Texture* _texture, int _spriteCol, int _btnWidth, int _btnHeight) :
	UIElement(UIElementType::BUTTON, id), spritesheet(_texture), spriteCol(_spriteCol), playingAnim(false),
	animFrame(0), animTimer(0.0f), animFrameDuration(0.1f), buttonHeight(_btnHeight), buttonWidth(_btnWidth)
{
	this->bounds = bounds;
	this->text = text;

	canClick = true;
	drawBasic = false;

	if (spritesheet) {
		LOG("spritesheet in button");
	}
	else {
		LOG("No spritesheet in buttons");
	}
}

UIButton::~UIButton()
{

}

bool UIButton::Update(float dt)
{

	if (Engine::GetInstance().scene->GetIgnoreInputThisFrame()) { return false; } //ignore frame

	if (state != UIElementState::DISABLED)
	{
		// L16: TODO 3: Update the state of the GUiButton according to the mouse position
		Vector2D mousePos = Engine::GetInstance().input->GetMousePosition();

		//If the position of the mouse if inside the bounds of the button 
		if (mousePos.getX() > bounds.x && mousePos.getX() < bounds.x + bounds.w && mousePos.getY() > bounds.y && mousePos.getY() < bounds.y + bounds.h) {

			if(state != UIElementState::SELECTED)
			{
				state = UIElementState::FOCUSED;
			}

			if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
				state = UIElementState::PRESSED;
			}

			if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
				NotifyObserver();
			}
		}
		else {
			if(state != UIElementState::SELECTED)
			{
				state = UIElementState::NORMAL;
			}
		}
	}

	DrawButton();

	return false;
}

bool UIButton::CleanUp()
{
	pendingToDelete = true;
	return true;
}

SDL_Rect UIButton::GetFrameRect(int row) const
{
	SDL_Rect rect;

	rect.x = spriteCol * buttonWidth;
	rect.y = row * buttonHeight;
	rect.w = buttonWidth;
	rect.h = buttonHeight;

	return rect;
}

void UIButton::SetTint(Uint8 r, Uint8 g, Uint8 b) const
{
	SDL_SetTextureColorMod(spritesheet, r, g, b);
}

void UIButton::ResetTint() const
{
	SDL_SetTextureColorMod(spritesheet, 255, 255, 255);
}

void UIButton::DrawButton() const
{
	if (spritesheet == nullptr) 
	{
		return;
	}

	SDL_Rect frameRect;

	if (playingAnim)
	{
		// Frames de animacion post-click (filas 2-5)
		frameRect = GetFrameRect(buttonAnimStart + animFrame);
		ResetTint();
	}
	else if (state == UIElementState::DISABLED)
	{
		frameRect = GetFrameRect(buttonRowNormal);
		SetTint(100, 100, 100);     // oscurecer para disabled
	}
	else if (state == UIElementState::PRESSED)
	{
		frameRect = GetFrameRect(buttonRowFocused);
		SetTint(200, 230, 255);     // tinte claro para pressed
	}
	else if (state == UIElementState::SELECTED) 
	{
		frameRect = GetFrameRect(buttonRowFocused);
		ResetTint();
	}
	else if (state == UIElementState::FOCUSED)
	{
		frameRect = GetFrameRect(buttonRowFocused);
		ResetTint();
	}
	else
	{
		// NORMAL
		frameRect = GetFrameRect(buttonRowNormal);
		ResetTint();
	}

	Engine::GetInstance().render->DrawTexture(
		spritesheet,
		bounds.x, bounds.y,
		&frameRect,
		1.0f    // sin parallax
	);

	ResetTint();
}