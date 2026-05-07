#pragma once
#include "Item.h"
#include <vector>

struct SDL_Texture;

enum class StatType
{
	POWER,
	SPEED,
	HEALTH,
	HEALING_POWER,
	FIRE_POWER,
	POISON_POWER,
	DURABILITY,
	LIFESTEAL
};

enum class Faction
{
	UNDEFINED,
	BIRD,
	HUMAN,
	REPTILE,
	SIREN
};

struct ItemStat
{
	StatType type;
	int value;
};

class EquippableItem : public Item
{
public:
	EquippableItem(const std::string& _name, Faction _faction, int _price);
	~EquippableItem() override = default;

	void Use(Character* character) override;
	void AddStat(StatType type, int value);

#pragma region GETTERS
	inline Faction GetFaction() const { return faction; }
	inline const std::vector<ItemStat>& GetItemStats() const { return stats; }
#pragma endregion

	inline Item* Clone() const override { return new EquippableItem(*this); }
private:
	Faction faction;
	std::vector<ItemStat> stats;
	SDL_Texture* texture;
};
