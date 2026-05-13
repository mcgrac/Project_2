#pragma once
#include <string>
#include "Island.h"
#include "SDL3/SDL.h"
#include "Vector2D.h"

struct SDL_Texture;

static struct GoldCounter {

	SDL_Texture* coinIcon = nullptr;
	int amount;
	static Vector2D position;

	GoldCounter(int _amount, std::string _path, int _positionX, int _positionY);

	void Update(int currentGoldAmount);
	void RenderCounter();
	void UpdateAmount(int i);
	void SetTextureCoin(std::string path);
};

class SceneUtils
{
public:
	static std::string GetFactionString(IslandFaction faction);
	static std::string GetBuildingTexturePath(IslandFaction faction, const std::string& building);
	static std::string GetIslandTexturePath(IslandFaction faction);
	static std::string GetPortraitPath(const std::string& dialogueId);
};
