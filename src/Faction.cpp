#include "Faction.h"

Faction::Faction(bool hostile){
	isHostile = hostile;
}
Faction::~Faction() {
	if (islandTexture != nullptr) {
		//unload texture
	}
}
void Faction::setTexture(SDL_Texture* txt) {
	islandTexture = txt;
}
void Faction::ChangeReputation(bool hostile) {
	isHostile = hostile;
}

SDL_Texture* Faction::getTexture{
	return islandTexture;
}
