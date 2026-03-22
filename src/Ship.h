#pragma once
#include <SDL3/SDL.h>
#include "Vector2D.h"
class Ship
{
private:

	int health;
	Vector2D position;
public:
	Ship();
	~Ship();
	int getHealth();
	void setHealth(int hlth);
	int dealDamage(int dmg);
	//void renderShip();
	void moveShip(int x, int y);
	Vector2D getPosition();
	void setPosition(int x, int y);

};