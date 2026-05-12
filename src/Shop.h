#pragma once
#include <vector>
#include "EquippableItem.h"

class NPC;
class Party;
class Island;
class Item;

class Shop
{
public:
    Shop(Island* _island);
    ~Shop();

    void GenerateItems(Faction faction);

    inline const std::vector<Item*>& GetCurrentItems() const { return currentItems; }
    inline Island* GetIsland() const { return island; }
    inline NPC* GetOwner() const { return owner; }

private:
    NPC* owner;
    std::vector<Item*> currentItems;
    Island* island;

    void ClearCurrentItems();
};
