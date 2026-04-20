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
    Island(int _id, const std::string& _name, IslandType _type, IslandFaction _islandFaction);

    ~Island();

#pragma region GETTERS
    inline int GetId() const { return id; }
    inline std::string GetName() const { return name; }
    inline IslandType GetType() const { return type; }
    inline Vector2D GetPosition() const { return position; }
    inline int GetX() const { return (int)position.getX(); }
    inline int GetY() const { return (int)position.getY(); }
    inline int GetWidth() const { return (int)size.getX(); }
    inline int GetHeight() const { return (int)size.getY(); }

    inline IslandFaction GetIslandFaction() const { return islandFaction; }

    inline Shop* GetShop() const { return shop; }
    inline Dockyard* GetDockyard() const { return dockyard; }
    inline Hostel* GetHostel() const { return hostel; }
#pragma endregion

    void SetRenderPos(float x, float y, float w, float h);

    void SetType(IslandType _type);

private:
    int id = -1;
    std::string name = "";
    IslandType type = IslandType::UNDEFINED;
    Vector2D position;
    Vector2D size;
    IslandFaction islandFaction = IslandFaction::UNDEFINED;

    Shop* shop;
    Dockyard* dockyard;
    Hostel* hostel;

    void CreateBuildings();
    void DestroyBuildings();
};
