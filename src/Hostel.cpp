#include "Hostel.h"
#include "NPC.h"
#include "Party.h"
#include "Character.h"
#include "Log.h"

Hostel::Hostel() : costRest(50)
{
	owner = new NPC("Hostel owner", "npc_hostel");

}

Hostel::~Hostel()
{
	delete owner;
	owner = nullptr;
}

void Hostel::Rest(Party* party)
{
	LOG("Hostel: resting");

	if(party->GetGold() >= costRest)
	{
		for (auto member : party->GetMembers()) {
			//heal an amount
			LOG("Character: %s | Previous health: %d", member->GetName(), member->GetCurrentHP());
			member->FullyHeal();
			LOG("Current health: %d", member->GetCurrentHP());
		}
	}
	else {
		LOG("Not enough gold for resting!");
	}
}

void Hostel::BuyXP(Party* party, int amount)
{
	LOG("Hostel: Buying XP");

	for (auto member : party->GetMembers()) {

		LOG("Character: %s | Previous xp: %d", member->GetName(), member->GetXP());
		member->AddXP(amount);
		LOG("Current experience: %d", member->GetXP());
	}
}

void Hostel::GetADrink(Party* party, std::string id)
{
	LOG("HOSTEL: Getting a frink");

	for (auto member : party->GetMembers()) {
		if (member->GetName() == id) {
			LOG("HOSTEL: %s is getting a drink", id);

			LOG("Character: %s | Previous health: %d", id, member->GetCurrentHP());
			member->FullyHeal();
			LOG("Current health: %d", member->GetCurrentHP());

			LOG("Character: %s | Previous xp: %d", member->GetName(), member->GetXP());
			member->AddXP(80);
			LOG("Current experience: %d", member->GetXP());
		}
	}

}
