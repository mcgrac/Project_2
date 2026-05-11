#pragma once
#include "Island.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>

struct SDL_Texture;

class WorldMap
{
public:
    WorldMap();

    bool LoadWorldData (const std::string& xmlPath); //only data, no visuals
    void ConnectVisuals(SDL_Texture* humanTex, SDL_Texture* reptileTex, SDL_Texture* skullTex); // hilo principal

    bool Update (float dt);
    bool PostUpdate (float dt);
    void UnloadWorld ();

    void MakeAllIslandsHostile(IslandFaction faction);

    // Travel to a specific island by id and fire arrivalIsland callback
    void TravelTo(int islandId);

    inline const Island* GetCurrentIsland() const{ return islands.at(currentIslandId); }
    inline int GetCurrentIslandId() const { return currentIslandId; }
    inline const std::unordered_map<int, Island*>& GetAllIslands() const { return islands; }
    inline const std::unordered_map<int, std::vector<int>>& GetTree() const { return tree; }
    // return id's from the next two childs islands
    const std::vector<int>& GetNextIds(int islandId) const;

    // Returns whether islandId is reachable from the current island
    bool IsReachable(int islandId) const;

    //setter
    inline void SetCurrentIsland(int islandId) { currentIslandId = islandId; }

    //callback
    std::function<void(Island*)> arrivalIsland;

    void LoadNPCTextures();

    void SetPendingIsland(int islandId) { pendingIslandId = islandId; }
    int GetPendingIslandId() const { return pendingIslandId; }
    void ConfirmTravel();   // confirma el viaje al pendingIslandId

    inline void SetShipReturnPosition(Vector2D pos) { shipReturnPosition = pos; }
    inline Vector2D GetShipReturnPosition() const { return shipReturnPosition; }

private:
    //
    //void UpdateWorld();
    //


    void RenderWorld(float dt);

    // Current island and selection
    int currentIslandId = -1;
    int pendingIslandId;  // isla a la que el jugador intenta viajar, aún no confirmada

    //
    //int selectedChildIndex = 0;   // 0 or 1
    //
    
    Vector2D shipReturnPosition;

    // All nodes of the three
    std::unordered_map<int, Island*> islands;
    std::unordered_map<int, std::vector<int>> tree;    // id -> [hijoId, ...]

    static const std::vector<int> EMPTY;
};
