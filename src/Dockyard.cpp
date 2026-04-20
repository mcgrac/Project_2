#include "Dockyard.h"
#include "NPC.h"
#include "Ship.h"

Dockyard::Dockyard() : owner(nullptr), ship(nullptr)
{
	owner = new NPC("Dockyard owner", "npc_dockyard");
}

Dockyard::~Dockyard()
{
	delete owner;
	owner = nullptr;

	delete ship;
	ship = nullptr;
}

void Dockyard::ImproveShip()
{
	ship->LevelUp();
}

void Dockyard::AssignShip(Ship* _ship)
{
	ship = _ship;
}

void Dockyard::UnassignShip()
{
	delete ship;
	ship = nullptr;
}
