#pragma once
#include <string>

class Character;

class CharacterFactory
{
public:
    CharacterFactory();
    ~CharacterFactory();

    static Character* Create(const std::string& name);
};
