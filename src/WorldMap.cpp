#include "WorldMap.h"
#include "Engine.h"
#include "Input.h"
#include "Textures.h"
#include "Render.h"
#include "LOG.h"
#include "pugixml.hpp"
#include <SDL3/SDL.h>

const std::vector<int> WorldMap::EMPTY = {};

WorldMap::WorldMap(){}

bool WorldMap::LoadWorld(const std::string& xmlPath)
{
    pugi::xml_document doc;
    if (!doc.load_file(xmlPath.c_str()))
    {
        LOG("WorldMap: failed to load %s", xmlPath.c_str());
        return false;
    }

    pugi::xml_node root = doc.child("world");

    int j = 0;

    for(int i = 0; i<7; i++){

        j++;
    
    }

    // 1. Load all islands
    for (pugi::xml_node node = root.child("island"); node; node = node.next_sibling("island"))
    {
        int id = node.attribute("id").as_int();
        std::string name = node.attribute("name").as_string();
        float x = node.attribute("x").as_float();
        float y = node.attribute("y").as_float();

        std::string typeStr = node.attribute("type").as_string();
        IslandType type;
        if (typeStr == "hostile")
        {
            type = IslandType::HOSTILE;
        }
        else if (typeStr == "friendly")
        {
            type = IslandType::FRIENDLY;
        }
        else
        {
            type = IslandType::UNDEFINED;
        }

        Vector2D position(x, y);
        islands.emplace(id, Island(id, name, type, position));
        tree[id] = {};

        LOG("WorldMap: loaded island — id=%d name='%s' type='%s' x=%.1f y=%.1f",
            id, name.c_str(), typeStr.c_str(), x, y);
    }

    LOG("WorldMap: total loaded islands: %d", (int)islands.size());

    // 2. Load navigation tree
    for (pugi::xml_node node = root.child("node"); node; node = node.next_sibling("node"))
    {
        int parentId = node.attribute("islandId").as_int();

        for (pugi::xml_node next = node.child("next"); next; next = next.next_sibling("next"))
        {
            int childId = next.text().as_int();
            tree[parentId].push_back(childId);

            LOG("WorldMap: conection — parent island %d -> child island %d", parentId, childId);
        }
    }

    LOG("WorldMap: navigation tree loaded.");

    // 3. Initial island is the one with id = 0
    currentIslandId = 0;
    selectedChildIndex = 0;

    LOG("WorldMap: initial island established: id=%d name='%s'",
        currentIslandId, islands.at(currentIslandId).GetName().c_str());

    return true;
}

const std::vector<int>& WorldMap::GetNextIds(int islandId) const
{
    auto it = tree.find(islandId);
    if (it != tree.end())
    {
        return it->second;
    }
    return EMPTY;
}

void WorldMap::UpdateWorld()
{
    const std::vector<int>& nexts = GetNextIds(currentIslandId);

    if (nexts.empty())
    {
        return;  // if final island, there is no navigation
    }

    // Change selection with TAB
    if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN)
    {
        if (nexts.size() > 1)
        {
            if (selectedChildIndex == 0)
            {
                selectedChildIndex = 1;
            }
            else
            {
                selectedChildIndex = 0;
            }
        }
    }

    // Travel with ENTER
    if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
    {
        int indexToTravel;
        if (nexts.size() > 1)
        {
            indexToTravel = selectedChildIndex;
        }
        else
        {
            indexToTravel = 0;
        }
        currentIslandId = nexts[indexToTravel];
        selectedChildIndex = 0;
        LOG("Viajando a isla id=%d (%s)", currentIslandId,
            islands.at(currentIslandId).GetName().c_str());
    }
}

void WorldMap::RenderWorld(float dt)
{
    const Island& current = islands.at(currentIslandId);
    const std::vector<int>& nexts = GetNextIds(currentIslandId);

    // render current island
    SDL_Rect act = { (int)current.GetX(), (int)current.GetY(), 100, 100 };
    Engine::GetInstance().render->DrawRectangle(act, 0, 0, 255, 255);

    // render next islands
    for (int i = 0; i < (int)nexts.size(); i++)
    {
        const Island& next = islands.at(nexts[i]);
        SDL_Rect r = { (int)next.GetX(), (int)next.GetY(), 100, 100 };

        Uint8 r_col;
        Uint8 g_col;
        if (next.GetType() == IslandType::HOSTILE)
        {
            r_col = 255;
            g_col = 0;
        }
        else
        {
            r_col = 0;
            g_col = 255;
        }
        Engine::GetInstance().render->DrawRectangle(r, r_col, g_col, 0, 255);

        // Marker
        if (i == selectedChildIndex)
        {
            SDL_Rect marker = { (int)next.GetX(), (int)next.GetY(), 25, 25 };
            Engine::GetInstance().render->DrawRectangle(marker, 255, 255, 0, 255);
        }
    }
}

void WorldMap::UnloadWorld()
{
    islands.clear();
    tree.clear();
    currentIslandId = -1;
    selectedChildIndex = 0;
}

bool WorldMap::Update(float dt)
{
    UpdateWorld();
    return true;
}

bool WorldMap::PostUpdate(float dt)
{
    RenderWorld(dt);
    return true;
}