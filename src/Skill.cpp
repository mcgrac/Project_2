#include "Skill.h"
#include "Character.h"

Skill::Skill(std::string _name, DamageType _type, int _baseDamage, float _multiplier, int _initiativeCost)
    : name(_name), damageType(_type), baseDamage(_baseDamage), 
    powerMultiplier(_multiplier), initiativeCost(_initiativeCost),
    hasAreaEffect(false), areaEffectTargetAllies(false)
{}

Skill::~Skill()
{
}

void Skill::Use(Character* caster, Character* target)
{
    int totalDamage = baseDamage + (int)(caster->GetPower() * powerMultiplier);

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
}