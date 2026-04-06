#pragma once
#include "Skill.h"
#include <unordered_map>
#include <functional>
#include <string>

class SkillRegistry
{
public:
    static SkillRegistry& GetInstance();

    // Returns a skill created with the id given
    Skill Create(const std::string& id, int initiativeCost) const;

    // Register a skill: id -> contructor
    void Register(const std::string& id, std::function<Skill(int initiativeCost)> builder);

private:
    SkillRegistry();
    std::unordered_map<std::string, std::function<Skill(int initiativeCost)>> registry;
};
