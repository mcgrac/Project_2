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
    const std::vector<UpgradeTier>& GetTiers() const;
};