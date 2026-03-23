#pragma once
#include <string>
#include <functional>
#include "Character.h"

class Character;

class CharacterFactory
{
public:
    CharacterFactory();
    ~CharacterFactory();

    static Character* Create(const std::string& name);
    static std::function<void(Character&)> ParseUpgradeEffect(const std::string& effect);
};
