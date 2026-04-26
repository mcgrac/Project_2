#pragma once
#include "Module.h"
#include <vector>
#include <string>
#include "Item.h"

class ItemManager: public Module
{
public:

    ItemManager();
    virtual ~ItemManager();

    bool Start() override;
    bool CleanUp() override;

    bool LoadItemsFromXML(const std::string& path);
    std::vector<Item*> GetItemsByFaction(Faction faction);
    Item* GetItemByName(const std::string& name);

private:

    std::vector<Item*> allItems;

    Faction StringToFaction(const std::string& str);
    StatType StringToStat(const std::string& str);

    std::string pathItems = "Assets/Items/Items.xml";
};
