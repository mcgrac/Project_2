#pragma once
#include "Character.h"
#include "Party.h"

class Theresia : public Character {
public:
    Theresia();
    ~Theresia();

private:
    void InitSkills();
    void InitUpgradeTree();
};
