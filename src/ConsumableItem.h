#pragma once
#include "Item.h"

class ConsumableItem : public Item
{
public:
	ConsumableItem(const std::string& _name, int _price, int _healAmount);
	~ConsumableItem() override = default;

	void Use(Character* character) override;
	bool IsConsumable() const override { return true; }

#pragma region GETTERS
	inline int GetHealAmount() const { return healAmount; }
#pragma endregion

	inline Item* Clone() const override { return new ConsumableItem(*this); }
private:
	int healAmount;
};
