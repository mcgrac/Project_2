#pragma once
#include "Upgrade.h"

class Character;

class UpgradeTier {
private:
    int requiredLevel;
    Upgrade optionA;
    Upgrade optionB;
    Upgrade* chosenUpgrade;

public:
    UpgradeTier(int _requiredLevel, Upgrade _optionA, Upgrade _optionB);
    ~UpgradeTier();

    bool ChooseUpgrade(int choice, Character& character);

    bool IsUnlocked()           const;
    bool IsAvailable(int level) const;
    int  GetRequiredLevel()     const;
    const Upgrade& GetOptionA() const;
    const Upgrade& GetOptionB() const;
    const Upgrade* GetChosen()  const;
};