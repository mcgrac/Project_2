#pragma once
#include "Faction.h"
#include <vector>
#include <string>

using namespace std;

class Island
{
private:
	//variables
	Faction* faction = nullptr;
	string* name;
	//saves the info of the rest of the nodes
	vector<Island*> nextIslands;
public:
	Island();
	~Island();
	int getNextSize();
	Island* getIslandIndex(int ind);//return the pointer to a island from the vector next given an index (1 or 2)
	void SetNext(vector<Island*> nxt);
	void AddNext(Island* island);
};

