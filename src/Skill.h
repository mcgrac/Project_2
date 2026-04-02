#pragma once
#include <string>
#include <vector>
#include <functional>

class Character; // forward declaration
class Party;

enum class DamageType { Physical, Magical, None };

//Each effect contains a function that is applied to the caster or target
struct SkillEffect {
    std::string description;
    std::function<void(Character* caster, Character* target)> apply;
};

class Skill {
private:
    std::string name;
    DamageType damageType;
    int baseDamage;          //base skill's damage (can be 0)
    float powerMultiplier;   // 1.0 = 100% power, 1.25 = +25%, 0 = sin daño base
    std::vector<SkillEffect> effects;
    int initiativeCost;
    bool hasAreaEffect;
    bool areaEffectTargetAllies;
    std::string animationId;

public:
    Skill(std::string _name, DamageType _type, int _baseDamage, float _multiplier, int _initiativeCost, std::string _animationId = "none");
    ~Skill();

    void AddEffect(SkillEffect effect) {
        effects.push_back(effect);
    }

    void Use(Character* caster, Character* target);

#pragma region GETTERS
    inline std::string GetName() const { return name; }
    inline int GetInitiativeCost() const  { return initiativeCost; }
    inline bool GetHasAreaEffect() const { return hasAreaEffect; }
    inline bool GetAreaEffectTargetAllies() const { return areaEffectTargetAllies; }
    inline std::string GetAnimationId() const { return animationId; }

    inline void SetHasAreaEffect(bool b) { hasAreaEffect = b; }
    inline void SetAreaEffectTargetAllies(bool b) { areaEffectTargetAllies = b; }

    //---------Test debug-------
    const std::vector<SkillEffect>& GetEffects() const { return effects; }
    //--------------------------
#pragma endregion

};
