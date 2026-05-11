#include "Skill.h"
#include "Character.h"
#include "Log.h"

Skill::Skill(std::string _name, DamageType _type, int _baseDamage, float _multiplier, int _initiativeCost, std::string _animationId)
    : name(_name), damageType(_type), baseDamage(_baseDamage), 
    powerMultiplier(_multiplier), initiativeCost(_initiativeCost), animationId(_animationId),
    hasAreaEffect(false), areaEffectTargetAllies(false)
{}

Skill::~Skill()
{
}

void Skill::Use(Character* caster, Character* target)
{
    int totalDamage = baseDamage + (int)(caster->GetTotalPower() * powerMultiplier);

    if (damageType == DamageType::Physical)
    {
        target->ReceivePhysicalDamage(totalDamage, caster);
    }
    else if (damageType == DamageType::Magical)
    {
        target->ReceiveMagicalDamage(totalDamage, caster);
    }

    //apply lifesteal if necessary
    if (totalDamage > 0)
    {
        caster->Heal((int)(totalDamage * (caster->GetLifesteal() / 100.0f)));
    }

    for (auto& effect : effects)
    {
        effect.apply(caster, target); //execute lambda function
    }

    LOG("BEFORE USE SKILL CASTER |%s| totalPower: %d, totalSpeed: %d, totalDurability:%d", caster->GetName().c_str(), caster->GetTotalPower(), caster->GetTotalSpeed(), caster->GetTotalDurability());
    caster->SetTotalPower();
    caster->SetTotalDurability();
    caster->SetTotalSpeed();
    LOG("AFTER USE SKILL CASTER |%s| totalPower: %d, totalSpeed: %d, totalDurability:%d", caster->GetName().c_str(), caster->GetTotalPower(), caster->GetTotalSpeed(), caster->GetTotalDurability());

    LOG("BEFORE USE SKILL TARGET |%s| totalPower: %d, totalSpeed: %d, totalDurability:%d", target->GetName().c_str(), target->GetTotalPower(), target->GetTotalSpeed(), target->GetTotalDurability());
    target->SetTotalPower();
    target->SetTotalDurability();
    target->SetTotalSpeed();
    LOG("AFTER USE SKILL TARGET|%s| totalPower: %d, totalSpeed: %d, totalDurability:%d", target->GetName().c_str(), target->GetTotalPower(), target->GetTotalSpeed(), target->GetTotalDurability());
}

std::string Skill::GetFullDescription()
{
    std::string result = name + " " + skillDescription;

    for (const auto& e : effects) {
        result += "\n- " + e.description;
    }

    return result;
}