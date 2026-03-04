#pragma once
#include"stdio.h"
#include <iostream>
#include <SDL3/SDL.h>
#include <vector>

#include "SkillTree.h"
#include "Inventory.h"
#include "Animation.h"
#include "Skill.h"

class Character {
private:
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
	int healingPower;

	bool isPoisoned;
	bool isBurned;
	int poisonStatMod;
	int burnedStatMod;

	int level;

	bool isAlive;

	SkillTree* skillTree = nullptr;
	Inventory* inventory = nullptr;

	AnimationSet anims;

	SDL_Texture* texture = nullptr;

	int maxHealthLevelScaling;
	int speedLevelScaling;
	int powerLevelScaling;

	std::vector<Skill> skills;

	Character* killedBy = nullptr; //to know which was the character that killed

public:

	Character(std::string _name, int _health, int _maxHealth, int _experience, int _initiative,
		      int _maxInitiative, int _power, int _durability, int _maxDurability, 
		      int _speed, int _lifesteal, int _healingPower, int _poisonedStatMod, 
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
	void UseSkill(int index, Character& target);

	void ModifyDurability(int amount);
	void SetBurned(bool state, int damage);
	void SetPoisoned(bool state, int damage);

	void SetKilledBy(Character* killer) { killedBy = killer; }

#pragma region GETTERS
	int GetPower() { return power; }
	int GetLifesteal() { return lifesteal; }
	int GetLevel() { return level; }
	bool GetIsAlive() { return isAlive; }
	Character* GetKilledBy() const { return killedBy; }
#pragma endregion


};
