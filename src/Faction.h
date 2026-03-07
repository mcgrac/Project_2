#pragma once
#include <string>

using namespace std;

class Faction
{
	//store units?
private:
	string* name;
	bool isHostile;
public:
	void ChangeReputation();
};

