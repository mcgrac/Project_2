#pragma once
#include <string>

using namespace std;

class Faction
{
private:
	string* name;
	bool isHostile;
public:
	void ChangeReputation();
};

