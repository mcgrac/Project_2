#pragma once
class NPC;
class Ship;

class Dockyard
{
public:
    Dockyard();
    ~Dockyard();

    void ImproveShip();
    void AssignShip(Ship* _ship);
    void UnassignShip();

    inline NPC* GetOwner() const { return owner; }

private:
    NPC* owner;
    Ship* ship;
};
