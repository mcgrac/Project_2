#include "Ship.h"

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