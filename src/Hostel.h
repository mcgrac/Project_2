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
    inline int GetRestCost() const { return costRest; }
    inline int GetMealCost() const { return mealCost; }
    inline Island* GetIsland() const { return island; }

    void Rest(Party* party);
    void BuyXP(Party* party, int amount);
    void GetADrink(Party* party, std::string id);

    bool CheckGold(int amount, Party* party);
private:
    NPC* owner;
    
    int costRest;
    int mealCost;

    Island* island;
};
