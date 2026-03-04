#include "UpgradeTree.h"

UpgradeTree::UpgradeTree()
{
}

UpgradeTree::~UpgradeTree()
{
}

void UpgradeTree::AddTier(UpgradeTier tier)
{
    tiers.push_back(tier);
}

UpgradeTier* UpgradeTree::GetAvailableTier(int currentLevel)
{
    for (auto& tier : tiers) {
        if (tier.IsAvailable(currentLevel))
            return &tier;
    }
    return nullptr;
}

const std::vector<UpgradeTier>& UpgradeTree::GetTiers() const
{
    return tiers;
}