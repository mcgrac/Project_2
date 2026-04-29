#include "Item.h"
#include "Character.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"

Item::Item(const std::string& _name, Faction _faction, int _price)
	: name(_name), faction(_faction), price(_price) , purchased(false)
{
}

Item::~Item() {}

bool Item::Awake() {
	return true;
}

bool Item::Start() {

    return true;
}

bool Item::Update(float dt)
{
    return true;
}

bool Item::CleanUp()
{

	return true;
}

bool Item::Destroy()
{
    return true;
}

void Item::AddStat(StatType type, int value)
{
	stats.push_back({ type, value });
}

void Item::ApplyEffect(Character* character)
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

    character->PrintDebugInfo(); //debug all stats

    // recalcular stats finales
    character->SetTotalPower();
    character->SetTotalSpeed();
    character->SetTotalDurability();

    character->PrintDebugInfo(); //debug stats after equipping
}
