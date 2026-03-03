#pragma once
#include"stdio.h"
#include <iostream>
#include <SDL3/SDL.h>

#include "SkillTree.h"
#include "Inventory.h"
#include "Animation.h"

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

	SkillTree* skillTree = nullptr;
	Inventory* inventory = nullptr;

	AnimationSet anims;

	SDL_Texture* texture = nullptr;

	int maxHealthLevelScaling;
	int speedLevelScaling;
	int powerLevelScaling;

public:

	Character(int _health, int _maxHealth, int _experience, int _initiative,
		      int _maxInitiative, int _power, int _durability, int _maxDurability, 
		      int _speed, int _lifesteal, int _healingPower, bool _isPoisoned, 
		      int _poisonedStatMod, int _burnedStatMod, int _level, int _maxHealthLevelScaling,
		      int _speedLevelScaling, int _powerLevelScaling);

	~Character();

	void Attack(Character target);
	void Heal(int amunt);
	void ReceivePhysicalDamage(int damageReceived);
	void ReceiveMagicalDamage(int damageReceived);
	void GainExperience(int amount);
	void LevelUp();

	void Draw(float dt);


};
