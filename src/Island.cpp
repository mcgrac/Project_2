#include "Island.h"
#include <vector>

Island::Island() {

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