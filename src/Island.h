#pragma once
#include <string>
#include "Vector2D.h"

enum class IslandType { 
    FRIENDLY, 
    HOSTILE,
    UNDEFINED
};

class Island
{
public:
    Island() = default;
    Island(int _id, const std::string& _name, IslandType _type, Vector2D _position);

    ~Island();

    inline int GetId() const { return id; }
    inline std::string GetName() const { return name; }
    inline IslandType GetType() const { return type; }
    inline Vector2D GetPosition() const { return position; }
    inline int GetX() const { return position.getX(); }
    inline int GetY() const { return position.getY(); }

private:
    int id = -1;
    std::string name = "";
    IslandType type = IslandType::UNDEFINED;
    Vector2D position;
};
