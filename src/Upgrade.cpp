#include "Upgrade.h"
#include "Character.h"

Upgrade::Upgrade(std::string _name, std::string _desc, std::function<void(Character&)> _apply)
    : name(_name), description(_desc), apply(_apply)
{}

void Upgrade::Apply(Character& character)
{
    apply(character);
}

std::string Upgrade::GetFullDescription() const
{
    return name + "\n" + description;
}
