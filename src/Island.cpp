#include "Island.h"
#include <vector>

Island::Island() {

}

int Island::getNextSize() {
	return nextIslands.size();
}

void Island::SetNext(vector<Island*> nxt) {
	nextIslands = nxt;
}

void Island::AddNext(Island* island) {
	if (nextIslands.size() < 2) {
		nextIslands.push_back(island);
	}
}