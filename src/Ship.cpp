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

void Ship::moveShip(int x, int y) {
	//setPosition(getPosition.getX() + 1, getPosition.getY() + 1);
	//moure's cap al target, fer un vector unitari des de la posició del baixdell al target
	//get vector
	float totalx = x - this->x;
	float totaly = y - this->y;
	//we find the magnitude of the vector
	float mag = sqrt(pow(x, 2) + pow(y, 2));
	totalx = totalx / mag;
	totaly = totaly / mag;
	//we calculate the to components of the unitary vector and add them to the psition of the ship
	this->x += 1 * totalx;
	this->y += 1 * totaly;
}