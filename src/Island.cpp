#include "Island.h"

Island::Island(int _id, const std::string& _name, IslandType _type, IslandFaction _islandFaction)
    : id(_id), 
      name(_name), 
      type(_type), 
      islandFaction(_islandFaction), 
      dockyard(nullptr),
      shop(nullptr),
      hostel(nullptr)
{
    if (type == IslandType::FRIENDLY) {
        if (!dockyard && !shop && !hostel)
        {
            CreateBuildings();
        }
    }
}

Island::~Island()
{
    DestroyBuildings();
}

void Island::SetRenderPos(float x, float y, float w, float h)
{
    position.setX(x);
    position.setY(y);
    size.setX(w);
    size.setY(h);
}

void Island::SetType(IslandType _type)
{
    //if (_type == IslandType::HOSTILE) {
    //    DestroyBuildings();
    //}
    //else if (_type == IslandType::FRIENDLY) {
    //    if(!dockyard && !shop && !hostel)
    //    {
    //        CreateBuildings();
    //    }
    //}

    type = _type;
}

void Island::CreateBuildings()
{
    shop = new Shop();
    hostel = new Hostel();
    dockyard = new Dockyard();
}

void Island::DestroyBuildings()
{
    if (dockyard) { 
        delete dockyard;
        dockyard = nullptr; 
    }
    if (shop) { 
        delete shop;
        shop = nullptr; 
    }
    if (hostel) { 
        delete hostel;
        hostel = nullptr; 
    }
}
