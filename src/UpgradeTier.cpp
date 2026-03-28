#include "UpgradeTier.h"
#include "Character.h"

UpgradeTier::UpgradeTier(int _requiredLevel, Upgrade _optionA, Upgrade _optionB)
    : requiredLevel(_requiredLevel), optionA(_optionA), optionB(_optionB),
    chosenUpgrade(nullptr)
{}

UpgradeTier::~UpgradeTier()
{
}

bool UpgradeTier::ChooseUpgrade(int choice, Character& character)
{
    if (IsUnlocked()) return false;

    if (choice == 0) chosenUpgrade = &optionA;
    else if (choice == 1) chosenUpgrade = &optionB;
    else return false;

    chosenUpgrade->Apply(character);
    return true;
}

bool UpgradeTier::IsUnlocked() const { return chosenUpgrade != nullptr; }

bool UpgradeTier::IsAvailable(int level) const { return level >= requiredLevel && !IsUnlocked(); }

int  UpgradeTier::GetRequiredLevel() const { return requiredLevel; }

const Upgrade& UpgradeTier::GetOptionA() const { return optionA; }

const Upgrade& UpgradeTier::GetOptionB() const { return optionB; }

const Upgrade* UpgradeTier::GetChosen() const { return chosenUpgrade; }