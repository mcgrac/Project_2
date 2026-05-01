#include "ItemManager.h"
#include "pugixml.hpp"
#include "Log.h"

ItemManager::ItemManager()
{
    name = "ItemsManager";
}

ItemManager::~ItemManager()
{
    //destroy list all items
    for (auto item : allItems)
    {
        delete item;
    }
    allItems.clear();
}

bool ItemManager::Start()
{
    LoadItemsFromXML(pathItems);

    return true;
}

bool ItemManager::CleanUp()
{
    for (auto item : allItems)
    {
        delete item;
    }
    allItems.clear();
    return true;
}

bool ItemManager::LoadItemsFromXML(const std::string& path)
{
    LOG("LOADING ITEMS");

    pugi::xml_document doc;

    if (!doc.load_file(path.c_str()))
    {
        return false;
    }

    for (pugi::xml_node itemNode : doc.child("items").children("item"))
    {
        std::string name = itemNode.attribute("name").as_string();
        std::string factionStr = itemNode.attribute("faction").as_string();
        int price = itemNode.attribute("price").as_int();

        Faction faction = StringToFaction(factionStr);

        Item* item = new Item(name, faction, price);

        for (pugi::xml_node statNode : itemNode.children("stat"))
        {
            std::string typeStr = statNode.attribute("type").as_string();
            int value = statNode.attribute("value").as_int();

            StatType type = StringToStat(typeStr);

            item->AddStat(type, value);
        }

        allItems.push_back(item);

#if _DEBUG
        //debug
        LOG("|ITEM: %s created|", item->GetName().c_str());
        LOG("STATS MODIFICATION");
        for (auto& stat : item->GetItemStats()) {
            LOG("Modificatin type: %d | value: %d", stat.type, stat.value);
        }
#endif
    }

    LOG("||TOTAL ITEMS CREATED: %d||", allItems.size());

    return true;
}

std::vector<Item*> ItemManager::GetItemsByFaction(Faction faction)
{
    std::vector<Item*> result;

    for (auto item : allItems)
    {
        if (item->GetFaction() == faction)
            result.push_back(item);
    }

    return result;
}

Item* ItemManager::GetItemByName(const std::string& name)
{
    for (auto item : allItems)
    {
        if (item->GetName() == name)
            return item;
    }

    return nullptr;
}

Faction ItemManager::StringToFaction(const std::string& str)
{
    if (str == "BIRD") return Faction::BIRD;
    if (str == "HUMAN") return Faction::HUMAN;
    if (str == "SIREN") return Faction::SIREN;

    return Faction::UNDEFINED;
}

StatType ItemManager::StringToStat(const std::string& str)
{
    if (str == "POWER") return StatType::POWER;
    if (str == "SPEED") return StatType::SPEED;
    if (str == "HEALTH") return StatType::HEALTH;
    if (str == "FIRE_POWER") return StatType::FIRE_POWER;
    if (str == "POISON_POWER") return StatType::POISON_POWER;
    if (str == "HEALING_POWER") return StatType::HEALING_POWER;
    if (str == "DURABILITY") return StatType::DURABILITY;
    if (str == "LIFESTEAL") return StatType::LIFESTEAL;

    return StatType::POWER;
}