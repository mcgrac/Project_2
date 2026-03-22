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
	ship = new Ship;
}

WorldMap::~WorldMap() {
	delete ship;
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
	if (!traveling) {
		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN) {//when pressing tab change island
			firstIslandSelected = !firstIslandSelected;
		}

		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) {
			//travel to selected island
			traveling = true;
			if (actualIsland->getNextSize() == 2) {
				if (firstIslandSelected) {
					//travel to island 0
					actualIsland = actualIsland->getIslandIndex(0);
					target = { 150, 450 }; //avall
				}
				else {
					//travel to island 1
					actualIsland = actualIsland->getIslandIndex(1);
					target = { 150, 550 };//amunt
				}
			}
			else {//the list has 1 member
				//travel to islnd 0
				actualIsland = actualIsland->getIslandIndex(0);
				target = { 150, 500 };//recte
			}
		}
	}
	else {
		//make the ship travel
		//ckeck if the ship reached its destination, if it reached its destination -> travel = false
		ship->moveShip(target.getX(), target.getY());
		
		if (sqrt(pow((ship->getPosition().getX() - shipTarget.getX()), 2) + pow(ship->getPosition().getY() - shipTarget.getY(), 2)) < 25) {//funció per acabar
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
	
}
void WorldMap::RenderWorld() {
	SDL_Rect pos = { 50, 500, 100, 100 };
	RenderDaughter(actualIsland, &pos, 0);

	//draw selector
	if (actualIsland->getNextSize() == 1) {
		//selector shown on the only island
		pos = { 150, 500, 100, 100 };
	}
	else if(firstIslandSelected){
		//selector shown on the first island
		pos = { 150, 550, 100, 100 };
	}
	else {
		pos = { 150, 450, 100, 100 };
	}
	//draw rectangle
	Engine::GetInstance().render->DrawRectangle(pos, 0, 0, 255, 255);
	//draw ship

	Engine::GetInstance().render->DrawCircle(ship->getPosition().getX(), ship->getPosition().getY(), 255, 0, 0, 255);
		
	}

void WorldMap::RenderDaughter(Island* islnd, SDL_Rect* pos, int level) {
	//draw actual island
	Engine::GetInstance().render->DrawRectangle(*pos, 0, 255, 0, 255);
	if (level < 2) {
		SDL_Rect act;
		switch (islnd->getNextSize()) {
		case 0:
			//no daughters to print
			break;
		case 1:
			act = { pos->x + 100, pos->y,  pos->w, pos->h };
			RenderDaughter(islnd->getIslandIndex(0), &act, level++);
			break;
		case 2:
			//RenderDaughter(); illa 1
			act = { pos->x + 100, pos->y + 50,  pos->w, pos->h };
			RenderDaughter(islnd->getIslandIndex(0), &act, level++);
			//RenderDaughter(); illa 2
			act = { pos->x + 100, pos->y - 50,  pos->w, pos->h };
			RenderDaughter(islnd->getIslandIndex(1), &act, level++);
			break;
		default:
			//i don now
			break;
		}
	}
	

	return;
		

}
	
	//draw squares for other islands

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