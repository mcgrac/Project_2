#include "ConsumableItem.h"
#include "Character.h"
#include "Log.h"

ConsumableItem::ConsumableItem(const std::string& _name, int _price, int _healAmount)
	: Item(_name, _price), healAmount(_healAmount)
{
}

void ConsumableItem::Use(Character* character)
{
	character->ModifyCurrentHealth(healAmount);
	LOG("Used consumable '%s' on '%s': restored %d HP", name.c_str(), character->GetName().c_str(), healAmount);
}
