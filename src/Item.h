//#pragma once
//#include <SDL3/SDL.h>
//#include <string>
//#include <vector>
//
//struct SDL_Texture;
//class Character;
//
//enum class StatType
//{
//	POWER,
//	SPEED,
//	HEALTH,
//	HEALING_POWER,
//	FIRE_POWER,
//	POISON_POWER,
//	DURABILITY,
//	LIFESTEAL
//};
//
//enum class ItemType
//{
//	POTION,
//	OBJECT
//};
//
//enum class Faction {
//	UNDEFINED,
//	BIRD,
//	HUMAN,
//	SIREN
//};
//
//struct ItemStat
//{
//	StatType type;
//	int value;
//};
//
//class Item
//{
//public:
//
//	Item(const std::string& _name, Faction _faction, int _price);
//
//	~Item();
//
//	bool Awake();
//
//	bool Start();
//
//	bool Update(float dt);
//
//	bool CleanUp();
//
//	bool Destroy();
//
//
//	void AddStat(StatType type, int value);
//	void ApplyEffect(Character* character);
//
//	inline const std::string& GetName() const { return name; }
//	inline const Faction GetFaction() const { return faction; }
//	inline const int GetPrice() const { return price; }
//	inline const bool IsPurchased() const { return purchased; }
//	inline void SetPurchased(bool value) { purchased = value; }
//
//	//debug
//	inline const std::vector<ItemStat> GetItemStats() const { return stats; }
//
//private:
//
//	bool purchased;
//	std::string name;
//	int price;
//	std::vector<ItemStat> stats;
//	Faction faction;
//};

#pragma once
#include <string>
class Character;

class Item
{
public:
	Item(const std::string& _name, int _price);
	virtual ~Item() = default;

	virtual void Use(Character* character) = 0;
	virtual bool IsConsumable() const { return false; }
	virtual Item* Clone() const = 0;

	inline const std::string& GetName() const { return name; }
	inline int GetPrice() const { return price; }
	inline bool IsPurchased() const { return purchased; }
	inline void SetPurchased(bool value) { purchased = value; }

protected:
	std::string name;
	int price;
	bool purchased = false;
};
