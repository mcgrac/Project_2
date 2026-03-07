#include "WorldMap.h"
#include "Engine.h"
#include "Textures.h"
#include "Render.h"
#include "LOG.h"
#include  <SDL3/SDL.h>
#include <vector>

WorldMap::WorldMap() {
	//smth
	islandsVisited = 0;
}

void WorldMap::LoadWorld() {
	//Load background texture
	background = Engine::GetInstance().textures->Load("Assets/Textures/waterPlaceholder.jpg");
	first = new Island;
	actualIsland = first;
	Island* islnd = new Island;
	first->AddNext(islnd);
	first->AddNext(islnd);
	delete islnd;
}
void WorldMap::UpdateWorld() {
	
}
void WorldMap::RenderWorld() {
	//draw background
	//Engine::GetInstance().render->SetBackgroundColor();
	//draw square for island
	SDL_Rect act = { 300, 100, 100, 100 };
	Engine::GetInstance().render->DrawRectangle(act, 0, 0, 255, 255);
	if (actualIsland->getNextSize() == 1) {
		act = { 200, 300, 100, 100 };
		Engine::GetInstance().render->DrawRectangle(act, 0, 255, 0, 255);
	}
	else if (actualIsland->getNextSize() == 2) {
		act = { 200, 300, 100, 100 };
		Engine::GetInstance().render->DrawRectangle(act, 0, 255, 0, 255);
		act = { 500, 300, 100, 100 };
		Engine::GetInstance().render->DrawRectangle(act, 255, 0, 0, 255);
	}
	else {
		LOG("error: more than 2 islands in the next vector, or less than 1");
	}
	//draw squares for other islands
}
void WorldMap::UnloadWorld() {
	//to do
}
bool WorldMap::Update(float dt) {
	RenderWorld();
	return true;
}
bool WorldMap::PostUpdate() {
	return true;
}