#include "Theresia.h"


Theresia::Theresia() : Character(
    {0,0},      // position
    "Theresia",   // name
    110,         // health
    110,         // maxHealth
    0,          // experience
    0,          // initiative
    300,        // maxInitiative
    62,         // power
    10,          // durability
    0,          // maxDurability
    30,         // speed
    0,          // lifesteal
    0,          // healingPower
    0,          // poisonPower
    0,          // firePower
    0,          // poisonStatMod
    0,          // burnedStatMod
    1,          // level
    6,          // maxHealthLevelScaling  (+5 per level)
    2,          // speedLevelScaling      (+1 per level)
    2           // powerLevelScaling      (+2 per level)
)
{
    //InitSkills();
    //InitUpgradeTree();
}

Theresia::~Theresia()
{}


void Theresia::InitUpgradeTree()
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