#include "Dockyard.h"
#include "NPC.h"

Dockyard::Dockyard() : owner(nullptr), ship(nullptr)
{
	owner = new NPC("Dockyard owner", "npc_dockyard");
}

Dockyard::~Dockyard() 
{


}

void Dockyard::ImproveShip()
{

}

void Dockyard::AssignShip(Ship* _ship)
{
	ship = _ship;
}

void Dockyard::UnassignShip()
{
	ship = nullptr;
}
