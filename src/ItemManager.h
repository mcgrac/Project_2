#pragma once
#include "Module.h"
#include <vector>
#include <string>
#include "Item.h"
#include "EquippableItem.h"

class KeyItem;
class ConsumableItem;

class ItemManager: public Module
{
public:

    ItemManager();
    virtual ~ItemManager();

    bool Start() override;
    bool CleanUp() override;

    bool LoadItemsFromXML(const std::string& path);

#pragma region GETTERS
    std::vector<Item*> GetAllItems() const;
    std::vector<EquippableItem*> GetEquippablesByFaction(Faction faction) const;
    Item* GetItemByName(const std::string& name);
#pragma endregion

    // Devuelven el unico item de ese tipo cargado del XML (nullptr si no existe)
    ConsumableItem* GetConsumable();
    KeyItem* GetKey();

private:

    std::vector<Item*> allItems;
    std::string pathItems = "Assets/Items/Items.xml";

    Faction StringToFaction(const std::string& str);
    StatType StringToStat(const std::string& str);

};