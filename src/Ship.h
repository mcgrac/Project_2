#pragma once
class Ship
{
private:

	int health;
public:
	Ship();
	~Ship();
	int getHealth();
	void setHealth(int hlth);
	int dealDamage(int dmg);

};