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

void Markus::InitSkills()
{
    // Skill 1 - Baile Rojo: Deal 10(+25%Power) Magic Damage and Inflict 10 Fire
    Skill baileRojo("Red dance", DamageType::Magical, 10, 0.25f, 100);
    baileRojo.AddEffect(
        {
        "Inflict 10 Fire",
        [](Character* caster, Character* target) {
            target->SetBurned(true, 10);
        }
        });
    AddSkill(baileRojo);

    // Skill 2 - Divine Light: Heal or deal magic damage depending on target
    Skill divineLight("Divine Light", DamageType::Magical, 0, 0.5f, 100);
    divineLight.AddEffect(
        {
        "Heal ally or damage enemy",
        [](Character* caster, Character* target) {
            target->Heal((int)(caster->GetPower() * 0.5f));
        }
        });
    AddSkill(divineLight);

    // Skill 2 - Laser Upward: Pure magic damage //stiull include "or Heal 10(+30%Power+HealingMod) to an ally"
    Skill laserUpward("Laser Upward", DamageType::Magical, 15, 0.2f, 100);
    AddSkill(laserUpward);

    // Skill 4 - Ascend: Team buff (Physical Damage buff)
    Skill ascend("Ascend", DamageType::None, 0, 0.0f, 100);
    ascend.AddEffect(
        {
        "Buff team PhysDmg",
        [](Character* caster, Character* target) {
            // apply buff when team logic is created
        }
        });
    AddSkill(ascend);

    // Skill 5 - Healing Halo: Ally healing aura
    Skill healingHalo("Healing Halo", DamageType::None, 0, 0.0f, 100);
    healingHalo.AddEffect(
        {
        "Heal all allies over time",
        [](Character* caster, Character* target) {
            target->Heal((int)(caster->GetPower() * 0.3f));
        }
        });
    AddSkill(healingHalo);
}

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