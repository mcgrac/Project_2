#pragma once
#include"stdio.h"
#include <iostream>
#include <SDL3/SDL.h>
#include <vector>
#include "Vector2D.h"

#include "Inventory.h"
#include "Animation.h"
#include "Skill.h"
#include "UpgradeTree.h"

class Character {
protected:
	Vector2D position;

	std::string name;
	int health;
	int maxHealth;
	int experience;
	int initiative;
	int maxInitiative;
	int power;
	int durability;
	int maxDurability;
	int speed;
	int lifesteal;
	float healingPower;
	float pisonPower;
	float firePower;

	bool isPoisoned;
	bool isBurned;
	int poisonStatMod;
	int burnedStatMod;

	int level;

	bool isAlive;

	Inventory* inventory = nullptr;


	AnimationSet anims;

	SDL_Texture* texture = nullptr;

	int maxHealthLevelScaling;
	int speedLevelScaling;
	int powerLevelScaling;

	std::vector<Skill> skills;

	Character* killedBy = nullptr; //to know which was the character that killed


	UpgradeTree* upgradeTree = nullptr;

public:

	Character(Vector2D _position, std::string _name, int _health, int _maxHealth, int _experience, int _initiative,
		      int _maxInitiative, int _power, int _durability, int _maxDurability, 
		      int _speed, int _lifesteal, float _healingPower, float _poisonPower, float _firePower, int _poisonedStatMod, 
		      int _burnedStatMod, int _level, int _maxHealthLevelScaling,
		      int _speedLevelScaling, int _powerLevelScaling);

	~Character();

	//void AttackPhysical(Character& target, int damage);
	//void AttackMagical(Character& target, int damage);

	void Heal(int amunt);
	void ReceivePhysicalDamage(int damageReceived, Character* attacker);
	void ReceiveMagicalDamage(int damageReceived, Character* attacker);
	void GainExperience(int amount);
	void LevelUp();

	void Draw(float dt);

	void AddSkill(Skill skill);
	void UseSkill(int index, Character* target);

	void ModifyDurability(int amount);
	void SetBurned(bool state, int damage);
	void SetPoisoned(bool state, int damage);

	inline void SetKilledBy(Character* killer) { killedBy = killer; }

	void ClearStatusEffects();

	inline void AddUpgradeTier(UpgradeTier tier) { upgradeTree->AddTier(tier); }

#pragma region GETTERS
	int GetPower() { return power; }
	int GetLifesteal() { return lifesteal; }
	int GetLevel() { return level; }
	bool GetIsAlive() { return isAlive; }
	Character* GetKilledBy() const { return killedBy; }

	void ModifyPower(int amount) { power += amount; }
	void ModifySpeed(int amount) { speed += amount; }
	void ModifyMaxHealth(int amount) { maxHealth += amount; health += amount; }
	void ModifyHealingPower(int amount) { healingPower += amount; }

	// Requeridos por Combat
	int GetSpeed() { return speed; }
	int GetCurrentHP() { return health; }
	int GetCurrentInitiative() { return initiative; }
	bool IsPoisoned() { return isPoisoned; }
	bool IsBurning() { return isBurned; }
	int GetPoisonDamage() { return poisonStatMod; }
	int GetBurnDamage() { return burnedStatMod; }
	std::string GetName() { return name; }
	std::vector<Skill>& GetSkills() { return skills; }
	void TakePoisonDamage() { ReceiveMagicalDamage(poisonStatMod, nullptr); }
	void TakeBurnDamage() { ReceiveMagicalDamage(burnedStatMod, nullptr); }

	float GetFirePower() { return firePower; }
	float GetPoisonPower() { return pisonPower; }
	float GetHealingPower() { return healingPower; }

	// Manipulación de iniciativa
	void AddInitiative(int amount) { initiative += amount; }
	void ResetCurrentInitiative() { initiative = 0; }

	// Posición en pantalla (para StartCombat)
	void SetPosition(float x, float y) { position.setX(x); position.setY(y); }
#pragma endregion

#pragma region TEST
	void PrintDebugInfo();
#pragma endregion
};
