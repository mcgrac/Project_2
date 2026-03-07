#pragma once
#include "Island.h"
#include  <SDL3/SDL.h>
#include "Module.h"
//includes

class WorldMap : public Module
{
private:
	//pointer to the first node of the list
	Island* first;
	Island* actualIsland;
	SDL_Texture* background = nullptr;
	int islandsVisited;
	//create a list or node tree of islands
public:
	WorldMap();
	bool Update(float dt);
	bool PostUpdate();
	void LoadWorld();
	void UpdateWorld();
	void RenderWorld();
	void UnloadWorld();
};