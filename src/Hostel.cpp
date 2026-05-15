#include "Hostel.h"
#include "NPC.h"
#include "Party.h"
#include "Character.h"
#include "Log.h"
#include "Island.h"
#include "SceneUtils.h"

Hostel::Hostel(Island* _island) : costRest(50), mealCost(30),island(_island)
{
	std::string dialogueId = "npc_hostel_" + SceneUtils::GetFactionString(island->GetIslandFaction());
	owner = new NPC("Hostel owner", dialogueId, island->GetIslandFaction(), "Hostel");

}

Hostel::~Hostel()
{
	delete owner;
	owner = nullptr;
}

void Hostel::Rest(Party* party)
{
	LOG("Hostel: resting");
	for (auto member : party->GetMembers()) {
		//heal an amount
		LOG("Character: %s | Previous health: %d", member->GetName(), member->GetCurrentHP());
		member->FullyHeal();
		LOG("Current health: %d", member->GetCurrentHP());
	}
}

void Hostel::BuyXP(Party* party, int amount)
{
	LOG("Hostel: Buying XP");

	for (auto member : party->GetMembers()) {

		LOG("Character: %s | Previous xp: %d", member->GetName(), member->GetXP());
		member->GainExperience(amount);
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
			member->GainExperience(80);
			LOG("Current experience: %d", member->GetXP());

			//check if character can level Up

		}
	}

}

bool Hostel::CheckGold(int amount, Party* party)
{
	bool b = false;
	if (amount <= party->GetGold()) {
		b = true;
	}
	return b;
}
