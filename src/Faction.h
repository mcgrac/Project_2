#pragma once
#include <string>
#include <SDL3/SDL.h>

using namespace std;

class Faction
{
	//store units?
private:
	string* name;
	bool isHostile;
	SDL_Texture* islandTexture = nullptr;
public:
	Faction(bool hostile);
	~Faction();
	void setTexture(SDL_Texture* txt);
	void ChangeReputation(bool hostile);
};

