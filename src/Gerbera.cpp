#include "Gerbera.h"

Gerbera::Gerbera() : Character(
    {0,0},      // position
    "Gerbera",   // name
    86,         // health
    86,         // maxHealth
    0,          // experience
    0,          // initiative
    300,        // maxInitiative
    75,         // power
    0,          // durability
    0,          // maxDurability
    40,         // speed
    0,          // lifesteal
    0,          // healingPower
    0,          // poisonPower
    0,          // firePower
    0,          // poisonStatMod
    0,          // burnedStatMod
    1,          // level
    4,          // maxHealthLevelScaling  (+5 per level)
    2,          // speedLevelScaling      (+1 per level)
    4           // powerLevelScaling      (+2 per level)
)
{
    //InitSkills();
    //InitUpgradeTree();
}

Gerbera::~Gerbera()
{}

void Gerbera::InitSkills()
{
    // Skill 1 - Deal 10(+35%Power) physical damage and burn for 5(+50%FireMod)
    Skill fireCharge("Fire charge", DamageType::Physical, 10, 0.35f, 100);
    fireCharge.AddEffect(
        {
        "Inflict 5 Fire",
        [](Character* caster, Character* target) {
            target->SetBurned(true, 5);
        }
        });
    AddSkill(fireCharge);

    // Skill 2 - Gain 20 Power and  30 Initiative
    Skill chargeArrow("Charge arrow", DamageType::None, 0, 0.0f, 100);
    chargeArrow.AddEffect(
        {
        "Gain 20 power and 30 initiative",
        [](Character* caster, Character* target) {
            caster->AddInitiative(30);
            caster->ModifyPower(20);
        }
        });
    AddSkill(chargeArrow);

    // Skill 3 - Deal 20(+10%Power) Physical damage. Inlict 10(+80%PoisonMod) Poison
    Skill greenArrow("Green arrow", DamageType::Physical, 20, 0.1f, 100);
    greenArrow.AddEffect(
        {
        "Inflict 10 poison",
        [](Character* caster, Character* target) {
            target->SetPoisoned(true, 10);
        }
        });
    AddSkill(greenArrow);

    // Skill 4 - Waste all Initiative and gain 50% of it as Power
    Skill chargeArrow2("Charged arrow", DamageType::None, 0, 0.0f, 100);
    chargeArrow2.AddEffect(
        {
        "Waste all Initiative and gain 50% of it as Power",
        [](Character* caster, Character* target) {
            caster->ModifyPower(caster->GetCurrentInitiative() * 0.5f);
        }
        });
    AddSkill(chargeArrow2);

    // Skill 5 - Deal 15(+10%Power) physical damage. Deal 1(+10%Power) more damage for every point of fire the enemy has. Reduce the fire to 0.
    Skill fireArrow2("Fire arrow 2", DamageType::Physical, 15, 0.1f, 100);
    fireArrow2.AddEffect(
        {
        "Deal 1(+10%Power) more damage for every point of fire the enemy has. Reduce the fire to 0.",
        [](Character* caster, Character* target) {
            target->ReceivePhysicalDamage((1 + (caster->GetPower() * 0.1)) * target->GetBurnDamage(), caster);
        }
        });
    AddSkill(fireArrow2);
}

void Gerbera::InitUpgradeTree()
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