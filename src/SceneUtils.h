#pragma once
#include <string>
#include "Island.h"
#include "SDL3/SDL.h"
#include "Vector2D.h"
#include "Animation.h"

struct SDL_Texture;

struct GoldCounter {

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
};

class SceneUtils
{
public:
	static std::string GetFactionString(IslandFaction faction);
	static std::string GetBuildingTexturePath(IslandFaction faction, const std::string& building);
	static std::string GetIslandTexturePath(IslandFaction faction);
	static std::string GetPortraitPath(const std::string& dialogueId);
};
