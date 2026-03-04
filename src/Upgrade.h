#pragma once
#include <string>
#include <functional>

class Character;

class Upgrade {
public:
    std::string name;
    std::string description;
    std::function<void(Character& character)> apply;

    Upgrade(std::string _name, std::string _desc, std::function<void(Character&)> _apply);
    ~Upgrade();

    void Apply(Character& character);
};