#pragma once
#include <string>
#include "Vector2D.h"
#include "Shop.h"
#include "Dockyard.h"
#include "Hostel.h"

enum class IslandType { 
    FRIENDLY, 
    HOSTILE,
    UNDEFINED
};

enum class IslandFaction {
    HUMANS,
    REPTILES,
    UNDEFINED
};

class Island
{
public:
    Island() = default;
    Island(int _id, const std::string& _name, IslandType _type, IslandFaction _islandFaction, Vector2D _position);

    ~Island();

#pragma region GETTERS
    inline int GetId() const { return id; }
    inline std::string GetName() const { return name; }
    inline IslandType GetType() const { return type; }
    inline Vector2D GetPosition() const { return position; }
    inline int GetX() const { return position.getX(); }
    inline int GetY() const { return position.getY(); }
    inline IslandFaction GetIslandFaction() const { return islandFaction; }

    inline Shop GetShop() const { return shop; }
    inline Dockyard GetDockyard() const { return dockyard; }
    inline Hostel GetHostel() const { return hostel; }

#pragma endregion

    void SetType(IslandType _type);

private:
    int id = -1;
    std::string name = "";
    IslandType type = IslandType::UNDEFINED;
    Vector2D position;
    IslandFaction islandFaction = IslandFaction::UNDEFINED;

    Shop shop;
    Dockyard dockyard;
    Hostel hostel;

    //void CreateBuildings();
    //void DestroyBuildings();
};
