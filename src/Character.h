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

struct AnimAlias
{
	int tile;
	bool loop;
};

class Character {
protected:

	Vector2D position;
	std::string name;

#pragma region STATS
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
	float poisonPower;
	float firePower;
	bool isPoisoned;
	bool isBurned;
	int poisonStatMod;
	int burnedStatMod;
	int maxHealthLevelScaling;
	int speedLevelScaling;
	int powerLevelScaling;
	int level;
	bool isAlive;
#pragma endregion

	Inventory* inventory = nullptr;

	//animations
	AnimationSet anims;
	SDL_Texture* texture = nullptr;

	std::vector<Skill> skills;
	Character* killedBy = nullptr; //to know which was the character that killed
	Character* poisonedBy = nullptr; //to knwo if the character was dead by poisoning which was the character that posion
	Character* burnedBy = nullptr; //to knwo if the character was dead by poisoning which was the character that posion
	UpgradeTree* upgradeTree = nullptr;

public:

	//PreCombatValues
	struct PreCombatValues {
		int _health;
		bool _isAlive;
	};

	PreCombatValues TakePreCombatValues() const {

		PreCombatValues snap;
		snap._health = health;
		snap._isAlive = isAlive;
		return snap;
	}

	void RestorePreCombatValues(const PreCombatValues& snap) {
		health = snap._health;
		isAlive = snap._isAlive;
		ClearStatusEffects();
		ResetCurrentInitiative();

	}

	Character(Vector2D _position, std::string _name, int _health, int _maxHealth, int _experience, int _initiative,
		      int _maxInitiative, int _power, int _durability, int _maxDurability, 
		      int _speed, int _lifesteal, float _healingPower, float _poisonPower, float _firePower, int _poisonedStatMod, 
		      int _burnedStatMod, int _level, int _maxHealthLevelScaling,
		      int _speedLevelScaling, int _powerLevelScaling);

	~Character();


	void Update(float dt);
	void Draw(float dt);

	void Heal(int amunt);
	void ReceivePhysicalDamage(int damageReceived, Character* attacker);
	void ReceiveMagicalDamage(int damageReceived, Character* attacker);
	void GainExperience(int amount);
	void LevelUp();

	void AddSkill(Skill skill);
	void UseSkill(int index, Character* target);

	void SetBurned(bool state, int damage, Character* attacker);
	void SetPoisoned(bool state, int damage, Character* attacker);

	inline void SetKilledBy(Character* killer) { killedBy = killer; }

	void ClearStatusEffects();

	inline void AddUpgradeTier(UpgradeTier tier) { upgradeTree->AddTier(tier); }
	inline void TakePoisonDamage() { ReceiveMagicalDamage(poisonStatMod, nullptr); }
	inline void TakeBurnDamage() { ReceiveMagicalDamage(burnedStatMod, nullptr); }

	// Initiative (combat)
	inline void ResetCurrentInitiative() { initiative = 0; }

	// Position in the screen (start combat)
	inline void SetPosition(float x, float y) { position.setX(x); position.setY(y); }

	void LoadVisuals(const std::string& spriteSheet, const std::string& tsx,
					 const std::unordered_map<int, std::string>& aliases,
		             const std::unordered_map<std::string, AnimAlias>& animData);

	void PlayAnimation(const std::string& name);

#pragma region GETTERS
	inline int GetPower() const { return power; }
	inline int GetLifesteal() const { return lifesteal; }
	inline int GetLevel() const { return level; }
	inline bool GetIsAlive() const { return isAlive; }
	inline Character* GetKilledBy() const { return killedBy; }
	inline int GetSpeed() const { return speed; }
	inline int GetCurrentHP() const { return health; }
	inline int GetCurrentInitiative() const { return initiative; }
	inline bool IsPoisoned() const { return isPoisoned; }
	inline bool IsBurning() const { return isBurned; }
	inline int GetPoisonDamage() const { return poisonStatMod; }
	inline int GetBurnDamage() const { return burnedStatMod; }
	inline std::string GetName() const { return name; }
	inline std::vector<Skill>& GetSkills() { return skills; }
	inline float GetFirePower() const { return firePower; }
	inline float GetPoisonPower() const { return pisonPower; }
	inline float GetHealingPower() const { return healingPower; }
	inline bool GetAnimationFinished() const { return anims.IsCurrentFinished(); }
	inline std::string GetCurrentAnimation() const { return anims.GetCurrentName(); }
#pragma endregion

#pragma region MODIFIERS
	inline void ModifyPower(int amount) { power += amount; }
	inline void ModifySpeed(int amount) { speed += amount; }
	inline void ModifyMaxHealth(int amount) { maxHealth += amount; health += amount; }
	inline void ModifyHealingPower(int amount) { healingPower += amount; }
	void ModifyDurability(int amount);
	inline void AddInitiative(int amount) { initiative += amount; }
	inline void ModifyFirePower(float amount) { firePower += amount; }
	inline void ModifyPoisonPower(float amount) { pisonPower += amount; }
#pragma endregion

#pragma region TEST
	void PrintDebugInfo();
#pragma endregion
};
