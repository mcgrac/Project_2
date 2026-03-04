#pragma once
#include <string>
#include <vector>
#include <functional>

class Character; // forward declaration

enum class DamageType { Physical, Magical, None };

//Each effect contains a function that is applied to the caster or target
struct SkillEffect {
    std::string description;
    std::function<void(Character& caster, Character& target)> apply;
};

class Skill {
public:
    std::string name;
    DamageType damageType;
    int baseDamage;          //base skill's damage (can be 0)
    float powerMultiplier;   // 1.0 = 100% power, 1.25 = +25%, 0 = sin daño base
    std::vector<SkillEffect> effects;
    int initiativeCost;

    Skill(std::string _name, DamageType _type, int _baseDamage, float _multiplier);
    ~Skill();

    void AddEffect(SkillEffect effect) {
        effects.push_back(effect);
    }

    void Use(Character& caster, Character& target);
};
