#include "UIButton.h"
#include "Render.h"
#include "Engine.h"
#include "Audio.h"
#include "Log.h"
#include "Scene.h"
#include "Fonts.h"

UIButton::UIButton(int id, SDL_Rect bounds, const char* text, SDL_Texture* _texture, int _spriteCol, int _btnWidth, int _btnHeight) :
	UIElement(UIElementType::BUTTON, id), spritesheet(_texture), spriteCol(_spriteCol), playingAnim(false),
	animFrame(0), animTimer(0.0f), animFrameDuration(0.1f), buttonHeight(_btnHeight), buttonWidth(_btnWidth)
{
	this->bounds = bounds;
	this->text = text;

	canClick = true;
	drawBasic = false;

	//load sound
	PressedFx = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/UIfx/button_press.wav");
	SelectedFx = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/UIfx/select.wav");

	// Crear texto si existe
	if (!this->text.empty())
	{
		font = Engine::GetInstance().fonts->LoadFont("Assets/Fonts/PixelFont.ttf", 24);

		if (font != nullptr)
		{
			textTexture = Engine::GetInstance().fonts->CreateTextTexture(
				this->text,
				textColor,
				font
			);

			if (textTexture != nullptr)
			{

				float w, h;
				SDL_GetTextureSize(textTexture, &w, &h);

				textRect.w = (int)w;
				textRect.h = (int)h;

				// Centrado
				textRect.x = bounds.x + (bounds.w - textRect.w) / 2;
				textRect.y = bounds.y + (bounds.h - textRect.h) / 2;
			}
			else
			{
				LOG("ERROR: no se pudo crear textTexture");
			}
		}
		else
		{
			LOG("ERROR: font es nullptr");
		}
	}
}

UIButton::~UIButton()
{

}

bool UIButton::Update(float dt)
{
	// Skip input on the frame this button was created
	if (createdThisFrame)
	{
		createdThisFrame = false;
		return false;
	}

	if (Engine::GetInstance().scene->GetIgnoreInputThisFrame()) { return false; } //ignore frame

	if (state != UIElementState::DISABLED)
	{
		// L16: TODO 3: Update the state of the GUiButton according to the mouse position
		Vector2D mousePos = Engine::GetInstance().input->GetMousePosition();

		//adjust bounds camera 
		//int camX = Engine::GetInstance().render->camera.x;
		int camX = isHUD ? 0 : Engine::GetInstance().render->camera.x;

		// Adjust bounds to screen space using camera offset
		int screenX = bounds.x + camX;
		int screenY = bounds.y;


		//If the position of the mouse is inside the bounds of the button 
		if (mousePos.getX() > screenX && mousePos.getX() < screenX + bounds.w &&
			mousePos.getY() > screenY && mousePos.getY() < screenY + bounds.h) {

			if (state != UIElementState::SELECTED)
			{
				if (state != UIElementState::FOCUSED && state != UIElementState::PRESSED)
				{
					Engine::GetInstance().audio->PlayFx(SelectedFx);
				}

				state = UIElementState::FOCUSED;
			}

			if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
				state = UIElementState::PRESSED;
				//Engine::GetInstance().audio->PlayFx(PressedFx);
			}

			if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
				NotifyObserver();
			}
		}
		else {
			if (state != UIElementState::SELECTED)
			{
				state = UIElementState::NORMAL;
			}
		}
	}

	//DrawButton();

	return false;
}

bool UIButton::CleanUp()
{
	if (textTexture != nullptr)
	{
		SDL_DestroyTexture(textTexture);
		textTexture = nullptr;
	}

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

bool UIButton::Draw()
{
	if (spritesheet == nullptr) 
	{
		return false;
	}

	SDL_Rect frameRect;

	if (playingAnim)
	{
		// Frames de animacion post-click (filas 2-5)
		//frameRect = GetFrameRect(buttonAnimStart + animFrame);
		//ResetTint();
	}
	else if (state == UIElementState::DISABLED)
	{
		frameRect = GetFrameRect(buttonRowDisabeled);
		ResetTint();
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

	float speed = isHUD ? 0.0f : 1.0f;

	Engine::GetInstance().render->DrawTexture(
		spritesheet,
		bounds.x, bounds.y,
		&frameRect,
		speed    // sin parallax
	);

	//Dibujar texto encima
	if (textTexture != nullptr)
	{
		Engine::GetInstance().render->DrawTexture(
			textTexture,
			textRect.x,
			textRect.y,
			nullptr,
			speed
		);
	}

	ResetTint();

	return true;
}

void UIButton::SetDisabledRow(int row)
{
	buttonRowDisabeled = row;
}

void UIButton::SetDisabled(bool disabled)
{
	if (disabled)
	{
		state = UIElementState::DISABLED;
	}
	else
	{
		state = UIElementState::NORMAL;
	}
}

bool UIButton::IsDisabled() const
{
	return state == UIElementState::DISABLED;
}