#include "Shop.h"
#include "ItemManager.h"
#include <algorithm>
#include <random>
#include "Engine.h"
#include "KeyItem.h"
#include "ConsumableItem.h"
#include "EquippableItem.h"
#include "NPC.h"
#include "Island.h"
#include "Log.h"
#include "SceneUtils.h"

Shop::Shop(Island* _island) : island(_island)
{
    std::string dialogueId = "npc_shop_" + SceneUtils::GetFactionString(island->GetIslandFaction());
    owner = new NPC("Shop owner", dialogueId, island->GetIslandFaction(), "Shop");
}

Shop::~Shop()
{
    delete owner;
    owner = nullptr;

    ClearCurrentItems();
}

void Shop::GenerateItems(Faction faction)
{
    if (!currentItems.empty()) { return; }

    ClearCurrentItems();

    std::vector<EquippableItem*> equippables = Engine::GetInstance().itemManager->GetEquippablesByFaction(faction);
    std::vector<ConsumableItem*> consumables;
    std::vector<KeyItem*> keys;

    for (Item* item : Engine::GetInstance().itemManager->GetAllItems())
    {
        ConsumableItem* consumable = dynamic_cast<ConsumableItem*>(item);
        if (consumable != nullptr)
        {
            consumables.push_back(consumable);
            continue;
        }

        KeyItem* key = dynamic_cast<KeyItem*>(item);
        if (key != nullptr)
        {
            keys.push_back(key);
        }
    }

    std::mt19937 rng(std::random_device{}());

    std::shuffle(equippables.begin(), equippables.end(), rng);
    std::shuffle(consumables.begin(), consumables.end(), rng);
    std::shuffle(keys.begin(), keys.end(), rng);

    for (int i = 0; i < 3 && i < (int)equippables.size(); i++)
    {
        currentItems.push_back(equippables[i]->Clone());
    }

    for (int i = 0; i < 2 && i < (int)consumables.size(); i++)
    {
        currentItems.push_back(consumables[0]->Clone());
    }

    if (!keys.empty())
    {
        currentItems.push_back(keys[0]->Clone());
    }

    LOG("SHOP: generados %d items", (int)currentItems.size());
}

void Shop::ClearCurrentItems()
{
    for (Item* item : currentItems)
    {
        delete item;
    }
    currentItems.clear();
}
