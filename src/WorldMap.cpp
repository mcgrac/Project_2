#include "WorldMap.h"
#include "Engine.h"
#include "Input.h"
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
	//ckeck for enter key and arrow key
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN) {//when pressing tab change island
		firstIslandSelected = !firstIslandSelected;
	}

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) {
		//travel to selected island
		if (actualIsland->getNextSize() == 2) {
			if (firstIslandSelected) {
				//travel to island 0
				actualIsland = actualIsland->getIslandIndex(0);
			}
			else {
				//travel to island 1
				actualIsland = actualIsland->getIslandIndex(1);
			}
		}
		else {//the list has 1 member
			//travel to islnd 0
			actualIsland = actualIsland->getIslandIndex(0);
		}
	}
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
		//draw marker on selected island
		act = {200, 300, 25, 25};
		Engine::GetInstance().render->DrawRectangle(act, 255, 255, 0, 255);
	}
	else if (actualIsland->getNextSize() == 2) {
		act = { 200, 300, 100, 100 };
		Engine::GetInstance().render->DrawRectangle(act, 0, 255, 0, 255);
		act = { 500, 300, 100, 100 };
		Engine::GetInstance().render->DrawRectangle(act, 255, 0, 0, 255);

		//draw marker on selected island
		if (firstIslandSelected) {
			act = { 200, 300, 25, 25 };
		}
		else {
			act = { 500, 300, 25, 25 };
		}
		Engine::GetInstance().render->DrawRectangle(act, 255, 255, 0, 255);
	}
	else if (actualIsland->getNextSize() == 0) {
		LOG("actual island has no other islands to go to");
	}
	else {
		LOG("error: more than 2 islands in the next vector, or less than 0");
	}
	//draw squares for other islands
}
void WorldMap::UnloadWorld() {
	//to do
}
bool WorldMap::Update(float dt) {
	UpdateWorld();
	RenderWorld();
	return true;
}
bool WorldMap::PostUpdate() {
	return true;
}