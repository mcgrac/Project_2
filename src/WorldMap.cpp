#include "WorldMap.h"
#include "Engine.h"
#include "Input.h"
#include "Textures.h"
#include "Render.h"
#include "LOG.h"
#include "pugixml.hpp"
#include <SDL3/SDL.h>
#include <queue>

const std::vector<int> WorldMap::EMPTY = {};

WorldMap::WorldMap() : pendingIslandId(-1)
{}

bool WorldMap::LoadWorldData(const std::string& xmlPath)
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

        std::string faction = node.attribute("faction").as_string();
        IslandFaction islandFaction;
        if (faction == "human") {

            islandFaction = IslandFaction::HUMANS;
        }
        else if (faction == "reptile") {

            islandFaction = IslandFaction::REPTILES;
        }
        else if (faction == "bird") {

            islandFaction = IslandFaction::BIRD;
        }
        else if (faction == "siren") {

            islandFaction = IslandFaction::SIRENS;
        }
        else if (faction == "tribal") {

            islandFaction = IslandFaction::TRIBAL;
        }
        else if (faction == "jellyfish") {

            islandFaction = IslandFaction::JELLYFISH;
        }
        else if (faction == "fish") {

            islandFaction = IslandFaction::FISH;
        }
        else if (faction == "boss") {

            islandFaction = IslandFaction::BOSS;
        }
        else {

            islandFaction = IslandFaction::UNDEFINED;
        }

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

        islands[id] = new Island(id, name, type, islandFaction); //ISLAND CREATION
        tree[id] = {};

        LOG("WorldMap: loaded island — id=%d name='%s' type='%s' faction='%s'",
            id, name.c_str(), typeStr.c_str(), faction.c_str());
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

    // 3. Asignar nivel a cada isla segun su columna en el arbol
    // Columna 0 = nivel 1, columna 1 = nivel 2, etc.
    std::unordered_map<int, int> islandColumn;
    std::queue<int> bfsQueue;
    bfsQueue.push(0);
    islandColumn[0] = 0;

    while (!bfsQueue.empty())
    {
        int cur = bfsQueue.front();
        bfsQueue.pop();

        int col = islandColumn[cur];

        auto islandIt = islands.find(cur);
        if (islandIt != islands.end())
        {
            islandIt->second->SetLevel(col + 1);
        }

        auto treeIt = tree.find(cur);
        if (treeIt != tree.end())
        {
            for (int childId : treeIt->second)
            {
                if (islandColumn.find(childId) == islandColumn.end())
                {
                    islandColumn[childId] = col + 1;
                    bfsQueue.push(childId);
                }
            }
        }
    }

    // 4. Initial island is the one with id = 0
    currentIslandId = 0;

    LOG("WorldMap: initial island established: id=%d name='%s'",
        currentIslandId, islands.at(currentIslandId)->GetName().c_str());

    doc.reset();

    return true;
}

void WorldMap::ConnectVisuals(SDL_Texture* humanTex, SDL_Texture* reptileTex, SDL_Texture* skullTex)
{
    for (auto& pair : islands)
    {
        Island* island = pair.second;

        if (island->GetIslandFaction() == IslandFaction::HUMANS)
        {
            island->SetSprite(humanTex);
        }
        else if (island->GetIslandFaction() == IslandFaction::REPTILES)
        {
            island->SetSprite(reptileTex);
        }

        island->SetSkullSprite(skullTex);
    }
}

Vector2D WorldMap::GetIslandShipPosition(int islandId) const
{
    auto it = islandShipPositions.find(islandId);
    if (it != islandShipPositions.end()) { return it->second; }
    return Vector2D(0, 0);
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

bool WorldMap::IsReachable(int islandId) const
{
    const std::vector<int>& nexts = GetNextIds(currentIslandId);
    for (int id : nexts)
    {
        if (id == islandId)
        {
            return true;
        }
    }
    return false;
}

void WorldMap::ConfirmTravel()
{
    if (pendingIslandId == -1)
    {
        LOG("WorldMap::ConfirmTravel — no hay isla pendiente.");
        return;
    }

    currentIslandId = pendingIslandId;
    pendingIslandId = -1;

    LOG("WorldMap: viaje confirmado a isla id=%d (%s)", currentIslandId, islands.at(currentIslandId)->GetName().c_str());
}

void WorldMap::RenderWorld(float dt, bool playerInTutorial)
{
    
    // 1. Dibujar icono de hostil (calavera)
    if (!playerInTutorial) {
        for (auto const& pair : islands)
    {
        Island* island = pair.second;

        if (island->GetType() == IslandType::HOSTILE)
        {
            int x = (int)island->GetX();
            int y = (int)island->GetY();

            int offsetX = -63;
            int offsetY = 5;

            if (island->GetSkullSprite() != nullptr)
            {
                Engine::GetInstance().render->DrawTexture(
                    island->GetSkullSprite(),
                    x + offsetX,
                    y + offsetY
                );
            }
        }
    }

    // 2. Dibujar un indicador de "Jugador Actual" (Opcional)
    // Ya que los botones ya se dibujan por el UIManager, aquí solo dibujamos 
    // efectos visuales extra, como un recuadro de selección.
    if (islands.count(currentIslandId))
    {
        Island* current = islands.at(currentIslandId);
        SDL_Rect highlight = {
            current->GetX() - 5,
            current->GetY() - 5,
            current->GetWidth() + 10,
            current->GetHeight() + 10
        };
        // Un recuadro amarillo brillante para saber dónde estamos
        Engine::GetInstance().render->DrawRectangle(highlight, 255, 255, 0, 100);
    }
    }
    
}

void WorldMap::UnloadWorld()
{
    for (auto& pair : islands)
    {
        delete pair.second;
        pair.second = nullptr;
    }

    islands.clear();
    tree.clear();
    currentIslandId = -1;
}

void WorldMap::MakeAllIslandsHostile(IslandFaction faction)
{

    for (auto& pair : islands) {
        Island* island = pair.second;
        if (island->GetIslandFaction() == faction && island->GetType() == IslandType::FRIENDLY) {
            island->SetType(IslandType::HOSTILE);
            LOG("WorldMap: island '%s' is now Hostile", island->GetName().c_str());
        }
    }
}

void WorldMap::TravelTo(int islandId)
{
    if (!IsReachable(islandId))
    {
        LOG("WorldMap::TravelTo — island %d is not reachable from current island %d", islandId, currentIslandId);
        return;
    }

    //currentIslandId = islandId;
    pendingIslandId = islandId;

    LOG("WorldMap: navegando hacia isla id=%d (%s) — pendiente de confirmar",pendingIslandId, islands.at(pendingIslandId)->GetName().c_str());

    if (arrivalIsland)
    {
        //arrivalIsland(islands.at(currentIslandId));
        arrivalIsland(islands.at(pendingIslandId));
    }
}

Island* WorldMap::GetIslandById(int id) const
{
    auto it = islands.find(id);
    if (it != islands.end()) { return it->second; }
    return nullptr;
}

bool WorldMap::Update(float dt)
{
    return true;
}

bool WorldMap::PostUpdate(float dt, bool playerInTutorial)
{
    RenderWorld(dt, playerInTutorial);
    return true;
}