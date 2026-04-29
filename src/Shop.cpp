#include "Shop.h"
#include "ItemManager.h"
#include <algorithm>
#include <random>
#include "Engine.h"
#include "NPC.h"
#include "Island.h"

Shop::Shop(Island* _island) : island(_island)
{
    owner = new NPC("Shop owner", "npc_shop", island->GetIslandFaction(), "Shop");
}

Shop::~Shop()
{
    delete owner;
    owner = nullptr;
}

void Shop::GenerateItems(Faction faction)
{
    currentItems.clear();

    auto items = Engine::GetInstance().itemManager->GetItemsByFaction(faction);

    std::shuffle(items.begin(), items.end(), std::mt19937(std::random_device()()));

    for (int i = 0; i < 3 && i < items.size(); i++)
    {
        currentItems.push_back(new Item(*items[i])); // copia
    }
}
