#pragma once
#include <unordered_map>
#include <vector>
#include <string>

class EquippableItem;

class Inventory
{
public:
    Inventory() = default;
    ~Inventory();

    // --- Shared items ---
    void AddItem(const std::string& itemId, int amount = 1);
    bool ConsumeItem(const std::string& itemId, int amount = 1);
    int GetItemCount(const std::string& itemId) const;

    // --- Equippable slots por personaje ---
    bool EquipItem(const std::string& characterId, EquippableItem* item);
    std::vector<EquippableItem*>& GetEquipped(const std::string& characterId);

private:
    std::unordered_map<std::string, int> items;
    std::unordered_map<std::string, std::vector<EquippableItem*>> equippedItems;
};