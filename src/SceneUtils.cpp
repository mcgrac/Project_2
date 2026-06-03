#include "SceneUtils.h"
#include "Engine.h"
#include "Textures.h"
#include "Render.h"
#include "Input.h"
#include "Log.h"
#include <cstdio>
#include <direct.h>
#include <sstream>

std::string SceneUtils::GetFactionString(IslandFaction faction)
{
	switch (faction)
	{
	case IslandFaction::HUMANS:    return "human";
	case IslandFaction::BIRD:      return "bird";
	case IslandFaction::SIRENS:    return "siren";
	case IslandFaction::REPTILES:  return "reptile";
	case IslandFaction::FISH:      return "fish";
	case IslandFaction::JELLYFISH: return "jellyfish";
	case IslandFaction::TRIBAL:    return "tribal";
	default:                       return "human"; // fallback
	}
}

std::string SceneUtils::GetBuildingTexturePath(IslandFaction faction, const std::string& building)
{
	return "Assets/Textures/" + building + "/" + SceneUtils::GetFactionString(faction) + ".png";
}

std::string SceneUtils::GetIslandTexturePath(IslandFaction faction)
{
	return "Assets/Textures/IslandInteriorBackground/" + SceneUtils::GetFactionString(faction) + ".png";
}

std::string SceneUtils::GetPortraitPath(const std::string& dialogueId)
{
	// npc_hostel_human -> Assets/Textures/Portraits/hostel/human.png
	// npc_dockyard_siren -> Assets/Textures/Portraits/dockyard/siren.png
	// formato esperado: npc_{location}_{faction}
	if(dialogueId=="intro_boss"){
		return "Assets/Textures/PortraitsDialogues/intro_boss.png";
	}
	else {
std::string stripped = dialogueId.substr(4); // quita "npc_"
	size_t underscore = stripped.find('_');
	if (underscore == std::string::npos)
	{
		return "";
	}
	std::string location = stripped.substr(0, underscore);
	std::string faction = stripped.substr(underscore + 1);
	return "Assets/Textures/PortraitsDialogues/" + location + "/" + faction + ".png";
	}
	
}

void SceneUtils::DrawAutoText(const char* text, int x, int y, SDL_Color color, int charW, int lineH)
{
	if (text == nullptr) { return; }
	int len = (int)strlen(text);
	int w = len * charW;
	Engine::GetInstance().render->DrawText(text, x, y, w, lineH, color);
}

bool SceneUtils::PointInRect(int x, int y, const SDL_Rect& r)
{
	return x > r.x &&
		x < r.x + r.w &&
		y > r.y &&
		y < r.y + r.h;
}

SDL_Point SceneUtils::GetMousePosition()
{
	Vector2D mousePos = Engine::GetInstance().input->GetMousePosition();

	return
	{
		(int)mousePos.getX(),
		(int)mousePos.getY()
	};
}

#pragma region GOLD COUNTER
GoldCounter::GoldCounter(int _amount, std::string _path, int _positionX, int _positionY) : amount(_amount)
{
	SetTextureCoin(_path);
	LoadAnimation();

	position.setX(_positionX);
	position.setY(_positionY);
}


void GoldCounter::Update(int currentGoldAmount, float dt) {

	UpdateAmount(currentGoldAmount);
	Draw(dt);
}

void GoldCounter::Draw(float dt)
{
	anims.Update(dt);

	const SDL_Rect& animFrame = anims.GetCurrentFrame();

	int drawX = (int)position.getX() - animFrame.w / 2;
	int drawY = (int)position.getY() - animFrame.h / 2;

	Engine::GetInstance().render->DrawTexture(
		coinIcon,
		drawX,
		drawY,
		&animFrame,
		0.0f
	);

	RenderCounter(drawX + animFrame.w + 5, drawY);
}

void GoldCounter::LoadAnimation()
{
	// load
	std::unordered_map<int, std::string> aliases = { {0,"idle"} };
	anims.LoadFromTSX("Assets/Textures/Animations/coin.tsx", aliases);
	anims.SetCurrent("idle");
}

void GoldCounter::RenderCounter(int x, int y)
{
	//render text
	SDL_Color col = { 255,255,255,255 };
	std::string textAmount = std::to_string(amount);
	Engine::GetInstance().render->DrawText(textAmount.c_str(), x, y, 30, 30, col);
}
void GoldCounter::UpdateAmount(int i)
{
	//update ampunt
	amount = i;
}
void GoldCounter::SetTextureCoin(std::string path)
{
	coinIcon = Engine::GetInstance().textures->Load(path.c_str());
}
void GoldCounter::MoveCounter(int positionX, int positionY)
{
	GoldCounter::position.setX(positionX);
	GoldCounter::position.setY(positionY);
}
void GoldCounter::SetPosition(int x, int y)
{
	GoldCounter::position.setX(x);
	GoldCounter::position.setY(y);
}
#pragma endregion

#pragma region TOOLTIP
void TooltipRenderer::Draw(const std::string& text, int x, int y) const
{
	if (text.empty()) { return; }

	std::vector<std::string> lines = WrapText(text);

	int maxLineLen = 0;
	for (const std::string& line : lines)
	{
		if ((int)line.size() > maxLineLen)
		{
			maxLineLen = (int)line.size();
		}
	}

	int boxWidth = maxLineLen * charWidth + padding * 2;
	int boxHeight = (int)lines.size() * lineHeight + padding * 2;

	if (!isDialogue) {
		SDL_Rect bg = { x, y, boxWidth, boxHeight };
		Engine::GetInstance().render->DrawRectangle(bg, 0, 0, 0, 200, true, false);
		Engine::GetInstance().render->DrawRectangle(bg, 255, 255, 255, 255, false, false);
	}

	int yOffset = 0;
	for (const std::string& line : lines)
	{
		DrawColoredLine(line, x + padding, y + padding + yOffset);
		yOffset += lineHeight;
	}
}
std::vector<std::string> TooltipRenderer::WrapText(const std::string& text) const
{
	std::vector<std::string> lines;
	std::stringstream ss(text);
	std::string word;
	std::string currentLine;

	while (ss >> word)
	{
		if ((int)(currentLine.length() + word.length() + 1) > maxCharsPerLine)
		{
			lines.push_back(currentLine);
			currentLine = word;
		}
		else
		{
			if (!currentLine.empty()) { currentLine += " "; }
			currentLine += word;
		}
	}

	if (!currentLine.empty())
	{
		lines.push_back(currentLine);
	}

	return lines;
}
void TooltipRenderer::DrawColoredLine(const std::string& line, int x, int y) const
{
	std::stringstream ss(line);
	std::string word;
	int offsetX = 0;

	while (ss >> word)
	{
		SDL_Color color = { 255, 255, 255, 255 };

		if (word.find("Heal") != std::string::npos)
		{
			color = { 0, 255, 0, 255 };
		}
		else if (word.find("Damage") != std::string::npos)
		{
			color = { 255, 80, 80, 255 };
		}

		int wordW = (int)word.size() * charWidth;
		Engine::GetInstance().render->DrawText(word.c_str(), x + offsetX, y, wordW, lineHeight, color);
		offsetX += wordW + charWidth;
	}
}
#pragma endregion

#pragma region DYNAMIC BAR
void DynamicBar::LoadTexture(const std::string& path)
{
	chunkTex = Engine::GetInstance().textures->Load(path.c_str());
}

void DynamicBar::Draw(int current, int max) const
{
	if (chunkTex == nullptr) { return; }
	if (max <= 0) { return; }

	int clampedCurrent = current;
	if (clampedCurrent < 0) { clampedCurrent = 0; }
	if (clampedCurrent > max) { clampedCurrent = max; }

	int filledChunks = (clampedCurrent * maxChunks) / max;
	if (filledChunks > maxChunks) { filledChunks = maxChunks; }

	int baseX = (int)position.getX();
	int baseY = (int)position.getY();

	for (int i = 0; i < filledChunks; i++)
	{
		int chunkX;
		if (leftToRight)
		{
			chunkX = baseX + i * (chunkW - chunkOverlap);
		}
		else
		{
			chunkX = baseX - i * (chunkW - chunkOverlap);
		}

		Engine::GetInstance().render->DrawTexture(chunkTex, chunkX, baseY, nullptr, false);
	}
}

void DynamicBar::UnloadTexture()
{
	Engine::GetInstance().textures->UnLoad(chunkTex);
	chunkTex = nullptr;
}
#pragma endregion

#pragma region UI PANEL ANIMATION
void UIPanelAnimation::Start(float startX, float startY, float endX, float endY)
{
	x = startX;
	y = startY;

	targetX = endX;
	targetY = endY;

	active = true;
}

void UIPanelAnimation::Update(float dt)
{
	dt /= 1000.0f;

	if (!active) return;

	//movement
	x += (targetX - x) * speed * dt;
	y += (targetY - y) * speed * dt;

	if (fabs(targetY - y) < 1.0f)
	{
		y = targetY;
	}

	if (fabs(targetX - x) < 1.0f)
	{
		x = targetX;
	}

	if (x == targetX && y == targetY)
	{
		active = false;
	}
}

bool UIPanelAnimation::IsFinished() const
{
	return !active;
}

#pragma endregion


