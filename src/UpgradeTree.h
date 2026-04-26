#pragma once
#include "UpgradeTier.h"
#include <vector>

class UpgradeTree {
private:
    std::vector<UpgradeTier> tiers;

public:
    UpgradeTree();
    ~UpgradeTree();

    void AddTier(UpgradeTier tier);
    UpgradeTier* GetAvailableTier(int currentLevel);
    std::vector<UpgradeTier>& GetTiers();
    const std::vector<UpgradeTier>& GetTiers() const;

    //---------test debug--------
    void PrintDebugInfo() const;
    //---------------------------
};