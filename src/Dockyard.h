#pragma once
class NPC;
class Ship;
class Island;

class Dockyard
{
public:
    Dockyard(Island* _island);
    ~Dockyard();

    void ImproveShip();
    void AssignShip(Ship* _ship);
    void UnassignShip();

    inline NPC* GetOwner() const { return owner; }
    inline Ship* GetShip() const { return ship; }
    inline Island* GetIsland() const { return island; }

private:
    NPC* owner;
    Ship* ship;

    Island* island;
};
