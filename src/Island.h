#pragma once
#include "Faction.h"
#include <vector>
#include <string>

using namespace std;

class Island
{
private:
	//variables
	Faction* faction;
	string* name;
	//saves the info of the rest of the nodes
	vector<Island*> nextIslands;
public:
	Island();
	int getNextSize();
	void SetNext(vector<Island*> nxt);
	void AddNext(Island* island);
};

