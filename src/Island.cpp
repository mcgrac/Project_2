#include "Island.h"
#include <vector>

Island::Island() {

}

Island::~Island() {
	//call the delete of daughter islands if they have any, then delete itself
}

int Island::getNextSize() {
	return nextIslands.size();
}

Island* Island::getIslandIndex(int ind) {
	return nextIslands[ind];
}

void Island::SetNext(vector<Island*> nxt) {
	nextIslands = nxt;
}

void Island::AddNext(Island* island) {
	if (nextIslands.size() < 2) {
		nextIslands.push_back(island);
	}
}

void Island::setVisited(bool vis) {
	visited = vis;
}

bool Island::getVisited() {
	return visited;
}

Faction* Island::getFaction() {
	return faction;
}

void Island::setFaction(Faction* fact) {
	faction = fact;
}