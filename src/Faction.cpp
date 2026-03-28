#include "Faction.h"
#include "Engine.h"
#include "Textures.h"

Faction::Faction(bool hostile){
	isHostile = hostile;
}
Faction::~Faction() {
	if (islandTexture != nullptr) {
		//unload texture
		Engine::GetInstance().textures->UnLoad(islandTexture);
	}
}

void Faction::LoadFaction() {
	//load faction island texture depending on faction name/id
}

void Faction::setTexture(SDL_Texture* txt) {
	islandTexture = txt;
}
void Faction::ChangeReputation(bool hostile) {
	isHostile = hostile;
}

SDL_Texture* Faction::getTexture(){
	return islandTexture;
}
