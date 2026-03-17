#pragma once
#pragma once
#include "Character.h"

class Gerbera : public Character {
public:
    Gerbera();
    ~Gerbera();

private:
    void InitSkills();
    void InitUpgradeTree();
};
