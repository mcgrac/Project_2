#include "Upgrade.h"
#include "Character.h"

Upgrade::Upgrade(std::string _name, std::string _desc, std::function<void(Character&)> _apply)
    : name(_name), description(_desc), apply(_apply)
{}

Upgrade::~Upgrade()
{

}

void Upgrade::Apply(Character& character)
{
    apply(character);
}