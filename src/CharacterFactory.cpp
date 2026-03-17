#include "CharacterFactory.h"
#include "Character.h"
#include "Markus.h"
#include "Theresia.h"
#include "Gerbera.h"
#include "Log.h"

CharacterFactory::CharacterFactory()
{
}

CharacterFactory::~CharacterFactory()
{
}

Character* CharacterFactory::Create(const std::string& name)
{
    if (name == "Markus")  return new Markus();
    if (name == "Theresia") return new Theresia();
    if (name == "Gerbera") return new Gerbera();

    LOG("CharacterFactory: character '%s' not recognized.", name.c_str());
    return nullptr;
}
