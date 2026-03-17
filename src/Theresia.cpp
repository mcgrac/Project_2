#include "Theresia.h"

// HP: 90(+5)  Power: 70(+2)  Durability: 0  Speed: 38(+1)
// Class: Incantor  Role: Mage/Support  Lane: Side

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
    InitSkills();
    InitUpgradeTree();
}

Theresia::~Theresia()
{}

void Theresia::InitSkills()
{
    // Skill 1 - Grant your team 5 Durability and 10 Initiative
    Skill encourage("Encourage", DamageType::None, 0, 0.0f, 100);
    encourage.AddEffect(
        {
        "Grant your team 5 Durability and 10 Initiative",
        [](Character* caster, Character* target) {
            //PREGUNTAR
            //acces to team?
        }
        });
    AddSkill(encourage);

    // Skill 2 - Grant your team 5% Power and Speed
    Skill battleFury("Battle Fury", DamageType::Magical, 0, 0.5f, 100);
    battleFury.AddEffect(
        {
        "Grant your team 5% Power and Speed",
        [](Character* caster, Character* target) {
            //PREGUNTAR
            //access to team?
        }
        });
    AddSkill(battleFury);

    // Skill 3 - Deal 10(+10%Power) physical damage and steal 5 Durability
    Skill slash("Slash", DamageType::Physical, 10, 0.1f, 100);
    slash.AddEffect(
        {
        "Steal 5 durability",
        [](Character* caster, Character* target) {
            caster->ModifyDurability(5);
            target->ModifyDurability(-5);
        }
        });
    AddSkill(slash);

    //PREGUNTAR
    // Skill 4 - Deal 5(+Durability) [durability dificil implementar] and reduce inititive by 15(+level)
    Skill shieldBash("Shield Bash", DamageType::Physical, 5, 0.0f, 100);
    shieldBash.AddEffect(
        {
        "Reduce inititive by 15(+level)",
        [](Character* caster, Character* target) {
            target->AddInitiative(-15 - caster->GetLevel());
        }
        });
    AddSkill(shieldBash);

    // PREGUNTAR
    // Skill 5 - Deal 10(+25%Power) Physical Damage and Inflict 0(+FireMod) Fire
    Skill doubleBlade("Double Blade", DamageType::Physical, 10, 0.25f, 100);
    doubleBlade.AddEffect(
        {
        "Inflict 0(+FireMod) Fire",
        [](Character* caster, Character* target) {
            target->Heal((int)(caster->GetPower() * 0.3f));
        }
        });
    AddSkill(doubleBlade);
}

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