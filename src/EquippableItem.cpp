#include "EquippableItem.h"
#include "Character.h"
#include "Log.h"

EquippableItem::EquippableItem(const std::string& _name, Faction _faction, int _price)
	: Item(_name, _price), faction(_faction)
{
}

void EquippableItem::AddStat(StatType type, int value)
{
	stats.push_back({ type, value });
}

void EquippableItem::Use(Character* character)
{
	for (const auto& stat : stats)
	{
		switch (stat.type)
		{
		case StatType::POWER:
			character->ModifyBasePower(stat.value);
			break;
		case StatType::SPEED:
			character->ModifyBaseSpeed(stat.value);
			break;
		case StatType::HEALTH:
			character->ModifyMaxHealth(stat.value);
			break;
		case StatType::HEALING_POWER:
			character->ModifyHealingPower(stat.value);
			break;
		case StatType::FIRE_POWER:
			character->ModifyFirePower(stat.value);
			break;
		case StatType::POISON_POWER:
			character->ModifyPoisonPower(stat.value);
			break;
		case StatType::DURABILITY:
			character->ModifyBonusDurability(stat.value);
			break;
		case StatType::LIFESTEAL:
			character->ModifyLifesteal(stat.value);
			break;
		default:
			break;
		}
	}

	character->PrintDebugInfo();
	character->SetTotalPower();
	character->SetTotalSpeed();
	character->SetTotalDurability();
	character->PrintDebugInfo();
}
