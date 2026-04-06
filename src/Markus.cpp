#include "Markus.h"

// HP: 90(+5)  Power: 70(+2)  Durability: 0  Speed: 38(+1)
// Class: Incantor  Role: Mage/Support  Lane: Side

Markus::Markus() : Character(
    {0,0},      // position
    "Markus",   // name
    90,         // health
    90,         // maxHealth
    0,          // experience
    0,          // initiative
    300,        // maxInitiative
    70,         // power
    0,          // durability
    0,          // maxDurability
    38,         // speed
    0,          // lifesteal
    0,          // healingPower
    0,          // poisonPower
    0,          // firePower
    0,          // poisonStatMod
    0,          // burnedStatMod
    1,          // level
    5,          // maxHealthLevelScaling  (+5 per level)
    1,          // speedLevelScaling      (+1 per level)
    2           // powerLevelScaling      (+2 per level)
)
{
    //InitSkills();
    //InitUpgradeTree();
}

Markus::~Markus()
{}


void Markus::InitUpgradeTree()
{
    // Tier 1 - Nivel 5
    upgradeTree->AddTier(UpgradeTier(5,
        Upgrade("Arcane Focus", "Power +15", [](Character& c) { c.ModifyPower(15); }),
        Upgrade("Vital Surge", "Max Health +20", [](Character& c) { c.ModifyMaxHealth(20); })
    ));

    // Tier 2 - Nivel 10
    upgradeTree->AddTier(UpgradeTier(10,
        Upgrade("Swift Cast", "Speed +8", [](Character& c) { c.ModifySpeed(8); }),
        Upgrade("Mending Light", "Healing Power +1", [](Character& c) { c.ModifyHealingPower(1); })
    ));

    // Tier 3 - Nivel 15
    upgradeTree->AddTier(UpgradeTier(15,
        Upgrade("Overload", "Power +25", [](Character& c) { c.ModifyPower(25); }),
        Upgrade("Inner Shield", "Max Health +40", [](Character& c) { c.ModifyMaxHealth(40); })
    ));
}