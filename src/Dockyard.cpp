#include "Dockyard.h"
#include "NPC.h"
#include "Ship.h"
#include "Island.h"

Dockyard::Dockyard(Island* _island) : owner(nullptr), ship(nullptr), island(_island)
{
	owner = new NPC("Dockyard owner", "npc_dockyard", island->GetIslandFaction(), "Dockyard");
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
