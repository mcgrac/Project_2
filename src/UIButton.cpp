#include "UIButton.h"
#include "Render.h"
#include "Engine.h"
#include "Audio.h"
#include "Log.h"

UIButton::UIButton(int id, SDL_Rect bounds, const char* text, SDL_Texture* _texture, int _spriteCol) :
	UIElement(UIElementType::BUTTON, id), spritesheet(_texture), spriteCol(_spriteCol), playingAnim(false),
	animFrame(0), animTimer(0.0f), animFrameDuration(0.1f)
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


	if (state != UIElementState::DISABLED)
	{
		// L16: TODO 3: Update the state of the GUiButton according to the mouse position
		Vector2D mousePos = Engine::GetInstance().input->GetMousePosition();

		//If the position of the mouse if inside the bounds of the button 
		if (mousePos.getX() > bounds.x && mousePos.getX() < bounds.x + bounds.w && mousePos.getY() > bounds.y && mousePos.getY() < bounds.y + bounds.h) {

			state = UIElementState::FOCUSED;

			if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
				state = UIElementState::PRESSED;
			}

			if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
				NotifyObserver();
			}
		}
		else {
			state = UIElementState::NORMAL;
		}

		//L16: TODO 4: Draw the button according the GuiControl State
		//switch (state)
		//{
		//case UIElementState::DISABLED:
		//	Engine::GetInstance().render->DrawRectangle(bounds, 200, 200, 200, 255, true, false);
		//	break;
		//case UIElementState::NORMAL:
		//	Engine::GetInstance().render->DrawRectangle(bounds, 0, 0, 255, 255, true, false);
		//	break;
		//case UIElementState::FOCUSED:
		//	Engine::GetInstance().render->DrawRectangle(bounds, 0, 0, 20, 255, true, false);
		//	break;
		//case UIElementState::PRESSED:
		//	Engine::GetInstance().render->DrawRectangle(bounds, 0, 255, 0, 255, true, false);
		//	break;
		//}

		//Engine::GetInstance().render->DrawText(text.c_str(), bounds.x, bounds.y, bounds.w, bounds.h, {255,255,255,255});

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
	if (spritesheet == nullptr) return;

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