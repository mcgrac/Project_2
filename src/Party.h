#pragma once
#include <vector>
#include <string>
#include "Inventory.h"

// Forward declarations
class Character;
class Item;

class Party
{
private:
    std::string name;
    std::vector<Character*> members;    

    // Resources allies  (aliados)
    int gold;     

    // Rewards enemies (enemigos)
    int xpReward;
    int goldReward;
    std::vector<Item*> lootItems;

    Inventory inventory;

public:

    Party(const std::string& name);
    ~Party();

    // members
    bool AddMember(Character* character);   // false si ya hay 3
    void RemoveMember(Character* character);
    std::vector<Character*>& GetMembers();
    int GetMemberCount() const;

    // ── Inventario / Recursos (persisten entre combates) ─────────────────
    void AddGold(int amount);
    void SpendGold(int amount);             // no baja de 0
    inline int GetGold() const { return gold; }

    inline Inventory& GetInventory() { return inventory; }

    // rewards
    void SetXPReward(int xp);
    void SetGoldReward(int gold);
    void AddLootItem(Item* item);
    int GetTotalXPReward() const;
    int GetTotalGoldReward() const;
    std::vector<Item*> GetLootItems() const;

    // extra
    bool IsDefeated() const;        // all members are dead
    std::string GetName() const;
    void PrintStatus() const;       // debug
};