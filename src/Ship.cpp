#include "Ship.h"
#include <SDL3/SDL.h>

using namespace std;

Ship::Ship() {
	health = 10;
}

Ship::~Ship() {

}

int Ship::getHealth() {
	return health;
}

void Ship::setHealth(int hlth) {
	health = hlth;
}

int Ship::dealDamage(int dmg) {
	setHealth(getHealth() - 1);
	return getHealth();
}

int Ship::getPosX() {
	return x;
}

int Ship::getPosY() {
	return y;
}

void Ship::setPosition(int x, int y) {
	this->x = x;
	this->y = y;
}

void Ship::moveShip(int tx, int ty) {
	//moure's cap al target, fer un vector unitari des de la posició del baixdell al target
	//get vector
	float totalx = tx - x;
	float totaly = ty - y;
	//we find the magnitude of the vector
	int mag = sqrt(pow(totalx, 2) + pow(totaly, 2));
	totalx = totalx / mag;
	totaly = totaly / mag;
	//we calculate the to components of the unitary vector and add them to the psition of the ship
	cout << "totalx = " << totalx << ", total y = " << totaly << endl;
	x += 1 * totalx;
	y += 1 * totaly;
}