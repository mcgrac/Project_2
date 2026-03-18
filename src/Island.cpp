#include "Island.h"

Island::Island(int _id, const std::string& _name, IslandType _type, Vector2D _position)
    : id(_id), name(_name), type(_type), position(_position) {
}

Island::~Island()
{
    //unload island
}
