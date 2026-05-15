#include "Inventory.h"
#include "EquippableItem.h"
#include "Log.h"

Inventory::~Inventory()
{
    // Los punteros de equippedItems son propiedad del ItemManager, que se encarga de su delete. Aquí solo limpiamos las estructuras.
    items.clear();

    for (auto& pair : equippedItems)
    {
        pair.second.clear();
    }
    equippedItems.clear();
}

void Inventory::AddItem(const std::string& itemId, int amount)
{
    items[itemId] += amount;

#if _DEBUG
    LOG("|Inventory Update|");

    if (items.empty())
    {
        LOG("Inventory empty");
    }
    else
    {
        for (const auto& pair : items) {
            // pair.first es la clave (string), pair.second es el valor (int)
            LOG("ID: %s | amount: %d", pair.first.c_str(), pair.second);
        }
        LOG("------------------");
    }
#endif // _DEBUG

}

bool Inventory::ConsumeItem(const std::string& itemId, int amount)
{
    auto it = items.find(itemId);
    if (it == items.end() || it->second < amount) { return false; }
    it->second -= amount;
    return true;
}

int Inventory::GetItemCount(const std::string& itemId) const
{
    auto it = items.find(itemId);
    if (it == items.end()) { return 0; }
    return it->second;
}

bool Inventory::EquipItem(const std::string& characterId, EquippableItem* item)
{
    if (item == nullptr) 
    { 
#if _DEBUG
        LOG("EquipItem: item es nullptr");
#endif // _DEBUG
        return false; 
    }

    std::vector<EquippableItem*>& slots = equippedItems[characterId];
    if (slots.size() >= 3) { return false; }

    EquippableItem* owned = static_cast<EquippableItem*>(item->Clone());
    slots.push_back(owned);

#if _DEBUG
    LOG("--- [DEBUG] Item Equipped ---");
    LOG("Character: %s", characterId.c_str());
    LOG("Current Loadout:");

    for (size_t i = 0; i < slots.size(); ++i)
    {
        EquippableItem* currentItem = slots[i];
        LOG("Slot [%d]: %s", i, currentItem->GetName().c_str());
    }
    LOG("-----------------------------");
#endif // _DEBUG

    return true;
}

std::vector<EquippableItem*>& Inventory::GetEquipped(const std::string& characterId)
{
    return equippedItems[characterId];
}