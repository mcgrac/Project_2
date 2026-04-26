#pragma once
#include <string>

class NPC;
class Party;
class Island;

class Hostel
{
public:
    Hostel(Island* _island);
    ~Hostel();

    inline NPC* GetOwner() const { return owner; }

    void Rest(Party* party);
    void BuyXP(Party* party, int amount);
    void GetADrink(Party* party, std::string id);

private:
    NPC* owner;
    
    int costRest;

    Island* island;
};
