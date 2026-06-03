#pragma once
#include <string>
#include "Island.h"
#include "SDL3/SDL.h"
#include "Vector2D.h"
#include "Animation.h"

struct SDL_Texture;

struct TooltipRenderer
{
	int charWidth = 8;
	int lineHeight = 20;
	int padding = 10;
	int maxCharsPerLine = 35;
	bool isDialogue = false;

	// Dibuja el tooltip completo en (x, y)
	void Draw(const std::string& text, int x, int y) const;

private:
	std::vector<std::string> WrapText(const std::string& text) const;
	void DrawColoredLine(const std::string& line, int x, int y) const;
};

struct GoldCounter 
{

	SDL_Texture* coinIcon = nullptr;
	int amount;
	Vector2D position;
	AnimationSet anims;

	GoldCounter() = default;

	GoldCounter(int _amount, std::string _path, int _positionX, int _positionY);
	void Update(int currentGoldAmount, float dt);


	void Draw(float dt);
	void LoadAnimation();
	void RenderCounter(int x, int y);
	void UpdateAmount(int i);
	void SetTextureCoin(std::string path);

	void MoveCounter(int positionX, int positionY);
	void SetPosition(int x, int y);
};

struct DynamicBar
{
	Vector2D position = { 0.0f, 0.0f };
	bool leftToRight = true;

	// Chunk dimensions — ajustar por instancia si se necesita
	int chunkW = 10;
	int chunkH = 12;
	int chunkOverlap = 0;
	int maxChunks = 10;

	void LoadTexture(const std::string& path);
	void Draw(int current, int max) const;
	void UnloadTexture();

private:
	SDL_Texture* chunkTex = nullptr;
};

struct UIPanelAnimation
{
	float x = 0.0f;
	float y = 0.0f;

	float targetX = 0.0f;
	float targetY = 0.0f;

	float speed = 8.0f; //initial speed

	bool active = false;

	void Start(float startX, float startY, float endX, float endY);

	void Update(float dt);

	bool IsFinished() const;
};

class SceneUtils
{
public:
	static std::string GetFactionString(IslandFaction faction);
	static std::string GetBuildingTexturePath(IslandFaction faction, const std::string& building);
	static std::string GetIslandTexturePath(IslandFaction faction);
	static std::string GetPortraitPath(const std::string& dialogueId);
	static void DrawAutoText(const char* text, int x, int y, SDL_Color color, int charW = 8, int lineH = 20);

	static bool PointInRect(int x, int y, const SDL_Rect& r);
	static SDL_Point GetMousePosition();
};
