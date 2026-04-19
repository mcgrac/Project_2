#pragma once
#include <string>

class NPC;
class Party;

class Hostel
{
public:
    Hostel();
    ~Hostel();

    inline NPC* GetOwner() const { return owner; }

    void Rest(Party* party);
    void BuyXP(Party* party, int amount);
    void GetADrink(Party* party, std::string id);

private:
    NPC* owner;
    
    int costRest;
};
