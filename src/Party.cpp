#include "Party.h"
#include "Character.h"
#include "Item.h"

#include <algorithm>
#include <iostream>

//  Constructor / Destructor
Party::Party(const std::string& name)
    : name(name)
    , gold(0)
    , xpReward(0)
    , goldReward(0)
{
}

Party::~Party()
{
    // Party NO es owner de Characters ni Items — no hace delete aquí
}

//  members
bool Party::AddMember(Character* character)
{
    if (members.size() >= 3)
    {
        std::cout << "[Party] " << name << " ya tiene 3 miembros.\n";
        return false;
    }
    members.push_back(character);
    return true;
}

void Party::RemoveMember(Character* character)
{
    auto it = std::find(members.begin(), members.end(), character);
    if (it != members.end())
    {
        members.erase(it);
    }
}

std::vector<Character*>& Party::GetMembers()
{
    return members;
}

int Party::GetMemberCount() const
{
    return static_cast<int>(members.size());
}

//  Recursos persistentes
void Party::AddGold(int amount)
{
    gold += amount;
}

void Party::SpendGold(int amount)
{
    gold = std::max(0, gold - amount);
}

int Party::GetGold() const
{
    return gold;
}

void Party::AddItem(Item* item)
{
    inventory.push_back(item);
}

void Party::RemoveItem(Item* item)
{
    auto it = std::find(inventory.begin(), inventory.end(), item);
    if (it != inventory.end())
        inventory.erase(it);
}

std::vector<Item*>& Party::GetInventory()
{
    return inventory;
}

//  Rewards

void Party::SetXPReward(int xp)
{
    xpReward = xp;
}

void Party::SetGoldReward(int gold)
{
    goldReward = gold;
}

void Party::AddLootItem(Item* item)
{
    lootItems.push_back(item);
}

int Party::GetTotalXPReward() const
{
    return xpReward;
}

int Party::GetTotalGoldReward() const
{
    return goldReward;
}

std::vector<Item*> Party::GetLootItems() const
{
    return lootItems;
}

//  Utilities
bool Party::IsDefeated() const
{
    for (Character* c : members)
    {
        if (c->GetIsAlive())
        {
            return false;
        }
    }
    return true;
}

std::string Party::GetName() const
{
    return name;
}

void Party::PrintStatus() const
{
    std::cout << "=== Party: " << name << " | Oro: " << gold << " ===\n";
    for (Character* c : members)
    {
        std::cout << "  - " << c->GetName()
            << "  HP: " << c->GetCurrentHP()
            << (!c->GetIsAlive() ? "  [MUERTO]" : "")
            << "\n";
    }
}