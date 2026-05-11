#pragma once
#include "Item.h"

class KeyItem : public Item
{
public:
	KeyItem(const std::string& _name, int _price);
	~KeyItem() override = default;

	void Use(Character* character) override {}

	Item* Clone() const override { return new KeyItem(*this); }
};
