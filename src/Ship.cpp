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

Vector2D Ship::getPosition() {
	return position;
}

void Ship::setPosition(int x, int y) {
	position.setX(x);
	position.setY(y);
}

void Ship::moveShip(int x, int y) {
	//setPosition(getPosition.getX() + 1, getPosition.getY() + 1);
	//moure's cap al target, fer un vector unitari des de la posició del baixdell al target
	
}