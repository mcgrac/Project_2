#pragma once
#include "Island.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>
//#include <SDL3/SDL.h>

struct SDL_Texture;

class WorldMap
{
public:
    WorldMap();

    bool LoadWorld (const std::string& xmlPath);
    bool Update (float dt);
    bool PostUpdate (float dt);
    void UnloadWorld ();

    void MakeAllIslandsHostile(IslandFaction faction);

    inline const Island& GetCurrentIsland() const{ return islands.at(currentIslandId); }

    //callback
    std::function<void(const Island&)> arrivalIsland;

private:
    void UpdateWorld();
    void RenderWorld(float dt);

    // return id's from the next two childs islands
    const std::vector<int>& GetNextIds(int islandId) const;

    // Current island and selection
    int currentIslandId = -1;
    int selectedChildIndex = 0;   // 0 or 1

    // All nodes of the three
    std::unordered_map<int, Island> islands;
    std::unordered_map<int, std::vector<int>> tree;    // id -> [hijoId, ...]

    static const std::vector<int> EMPTY;
};
