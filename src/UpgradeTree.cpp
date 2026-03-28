#include "UpgradeTree.h"
#include "Log.h"

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

void UpgradeTree::PrintDebugInfo() const{
    LOG("  Upgrade Tree : %d tiers", (int)tiers.size());

    for (int i = 0; i < (int)tiers.size(); ++i)
    {
        const UpgradeTier& tier = tiers[i];
        LOG("  Tier [%d] : Required level: %d", i, tier.GetRequiredLevel());

        LOG("    Option A: %s or %s",
            tier.GetOptionA().name.c_str(),
            tier.GetOptionA().description.c_str()
        );

        LOG("    Option B: %s or %s",
            tier.GetOptionB().name.c_str(),
            tier.GetOptionB().description.c_str()
        );

        if (tier.GetChosen() != nullptr)
        {
            LOG("    Chosen:   %s", tier.GetChosen()->name.c_str());
        }
        else
        {
            LOG("    Chosen:   [none yet]");
        }
    }
}
