#pragma once
#include "Island.h"
#include  <SDL3/SDL.h>
#include "Module.h"
#include "ship.h"
//includes

class WorldMap : public Module
{
private:
	//pointer to the first node of the list
	Island* first;
	Island* actualIsland;
	SDL_Texture* background = nullptr;
	int islandsVisited;
	bool firstIslandSelected;
	//create a list or node tree of islands
	bool traveling = false;
	Ship* ship = nullptr;
public:
	WorldMap();
	~WorldMap();
	bool Update(float dt);
	bool PostUpdate();
	void LoadWorld();
	void UpdateWorld();
	void RenderWorld();
	void RenderDaughter(Island* islnd, SDL_Rect* pos, int level);
	void UnloadWorld();
};