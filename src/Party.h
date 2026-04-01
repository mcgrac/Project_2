#pragma once
#include <vector>
#include <string>

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
    std::vector<Item*> inventory;       

    // Rewards enemies (enemigos)
    int xpReward;
    int goldReward;
    std::vector<Item*> lootItems;
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
    int GetGold() const;

    void AddItem(Item* item);
    void RemoveItem(Item* item);
    std::vector<Item*>& GetInventory();

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