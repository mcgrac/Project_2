#pragma once
#include"stdio.h"
#include <iostream>
#include <SDL3/SDL.h>
#include <vector>
#include "Vector2D.h"
#include "Animation.h"
#include "Skill.h"
#include "UpgradeTree.h"

struct AnimAlias
{
	int tile;
	bool loop;
};


struct DamagePopup
{
	int value = 0;
	Vector2D offset;
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
	int totalPower;
	int basePower;
	int bonusPower;
	int totalDurability;
	int baseDurability;
	int bonusDurability;
	int maxDurability;
	int totalSpeed;
	int baseSpeed;
	int bonusSpeed;
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

	bool isAllied;

	//animations
	AnimationSet anims;
	SDL_Texture* texture = nullptr;

	std::vector<Skill> skills;
	Character* killedBy = nullptr; //to know which was the character that killed
	Character* poisonedBy = nullptr; //to knwo if the character was dead by poisoning which was the character that posion
	Character* burnedBy = nullptr; //to knwo if the character was dead by poisoning which was the character that posion
	UpgradeTree* upgradeTree = nullptr;

public:

#pragma region PRE-COMBAT VALUES
	struct PreCombatValues {
		int _health;
		bool _isAlive;

		int _basePower;
		int _baseSpeed;
		int _baseDurability;

		float _healingPower;
		float _firePower;
		float _poisonPower;
	};

	PreCombatValues TakePreCombatValues() const {

		PreCombatValues snap;
		snap._health = health;
		snap._isAlive = isAlive;

		snap._basePower = basePower;
		snap._baseSpeed = baseSpeed;
		snap._baseDurability = baseDurability;

		snap._healingPower = healingPower;
		snap._firePower = firePower;
		snap._poisonPower = poisonPower;

		return snap;
	}

	void RestorePreCombatValues(const PreCombatValues& snap) {
		health = snap._health;
		isAlive = snap._isAlive;
		ClearStatusEffects();
		ResetCurrentInitiative();

	}

	void RestoreBaseStats(const PreCombatValues& snap)
	{
		basePower = snap._basePower;
		baseSpeed = snap._baseSpeed;
		baseDurability = snap._baseDurability;

		healingPower = snap._healingPower;
		firePower = snap._firePower;
		poisonPower = snap._poisonPower;

		SetTotalPower();
		SetTotalSpeed();
		SetTotalDurability();
	}
#pragma endregion



	Character(Vector2D _position, std::string _name, int _health, int _maxHealth, int _experience, int _initiative,
		      int _maxInitiative, int _basePower, int _bonusPower, int _totalPower, int _totalDurability,int _baseDurability, int _bonusDurability, int _maxDurability,
		      int _baseSpeed, int _bonusSpeed, int _totalSpeed, int _lifesteal, float _healingPower, float _poisonPower, float _firePower, int _poisonedStatMod,
		      int _burnedStatMod, int _level, int _maxHealthLevelScaling,
		      int _speedLevelScaling, int _powerLevelScaling);

	~Character();


	void Update(float dt);
	void Draw(float dt);

	void Heal(int amunt);
	void FullyHeal();
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
	inline void TakeBurnDamage() { ReceivePhysicalDamage(burnedStatMod, nullptr); }

	// Initiative (combat)
	inline void ResetCurrentInitiative() { initiative = 0; }

	// Position in the screen (start combat)
	inline void SetPosition(float x, float y) { position.setX(x); position.setY(y); }

	void LoadVisuals(const std::string& spriteSheet, const std::string& tsx,
					 const std::unordered_map<int, std::string>& aliases,
		             const std::unordered_map<std::string, AnimAlias>& animData);

	void PlayAnimation(const std::string& name);

	void ClearBonusStats();

#pragma region DAMAGE POP-UP
	void AddDamagePopup(int dmg);
	void OnHit();
	void DrawPopUp();
	void UpdatePopUp(float dt);
	void DeletePopUps();
	void ClearHitFlash();
#pragma endregion

#pragma region GETTERS
	inline Vector2D GetPosition() const { return position; }

	inline int GetTotalPower() const { return totalPower; }
	inline int GetBasePower() const { return basePower; }

	inline int GetLifesteal() const { return lifesteal; }
	inline int GetXP() const { return experience; }
	inline int GetLevel() const { return level; }
	inline bool GetIsAlive() const { return isAlive; }
	inline bool GetPendingToDie() const { return pendingToDie; }
	inline Character* GetKilledBy() const { return killedBy; }

	inline int GetTotalSpeed() const { return totalSpeed; }
	inline int GetBaseSpeed() const { return baseSpeed; }

	inline int GetCurrentHP() const { return health; }
	inline int GetMaxHP() const { return maxHealth; }

	inline int GetExperience() const { return experience; }
	inline int GetCurrentInitiative() const { return initiative; }
	inline int GetMaxInitiative() const { return maxInitiative; }
	inline bool IsPoisoned() const { return isPoisoned; }
	inline bool IsBurning() const { return isBurned; }
	inline int GetPoisonDamage() const { return poisonStatMod; }
	inline int GetBurnDamage() const { return burnedStatMod; }
	inline std::string GetName() const { return name; }
	inline std::vector<Skill>& GetSkills() { return skills; }
	inline float GetFirePower() const { return firePower; }
	inline float GetPoisonPower() const { return poisonPower; }
	inline float GetHealingPower() const { return healingPower; }
	inline bool GetAnimationFinished() const { return anims.IsCurrentFinished(); }
	inline std::string GetCurrentAnimation() const { return anims.GetCurrentName(); }

	inline int GetTotalDurability() const { return totalDurability; }

	inline UpgradeTree* GetUpgradeTree() const { return upgradeTree; }
	inline bool GetIsAllied() const { return isAllied; }
#pragma endregion

#pragma region MODIFIERS
	inline void ModifyBasePower(int amount) { basePower += amount; }
	inline void ModifyBaseSpeed(int amount) { baseSpeed += amount; }
	inline void ModifyBonusPower(int amount) { bonusPower += amount; }
	inline void ModifyBonusSpeed(int amount) { bonusSpeed += amount; }
	void ModifyMaxHealth(int amount);
	inline void ModifyCurrentHealth(int amount) { health += amount; }
	inline void ModifyHealingPower(int amount) { healingPower += amount; }
	void ModifyDurability(int amount);
	inline void ModifyBonusDurability(int amount) { bonusDurability += amount; }
	void AddInitiative(int amount);
	inline void ModifyFirePower(float amount) { firePower += amount; }
	inline void ModifyPoisonPower(float amount) { poisonPower += amount; }
	inline void AddXP(int amount) { experience += amount; }
	inline void ModifyLifesteal(int amount) { lifesteal += amount; }
	void ModifyMaxDurability(int amount);
	void ModifyBaseDurability(int amount);

	inline void SetTotalPower() { totalPower = basePower + bonusPower; }
	inline void SetTotalDurability() { totalDurability = baseDurability + bonusDurability; }
	inline void SetTotalSpeed() { totalSpeed = baseSpeed + bonusSpeed; }

	inline void SetIncomingDamageMultiplier(float f) { incomingDamageMultiplier = f; }
	inline void SetIsAllied(bool b) { isAllied = b; }
#pragma endregion

#pragma region TEST
	void PrintDebugInfo();
#pragma endregion

private:

	//-----------FUNCTIONS--------------//


	//-----------VARIABLES------------//
	float incomingDamageMultiplier = 1.0f;
	bool pendingToDie = false;

	static constexpr int MAX_DURABILITY_POSSIBLE = 70;

#pragma region DAMAGE POPUP
	std::vector<DamagePopup> damagePopups;
	bool hitFlash = false;
	float blinkTimer = 0.0f;
	bool blinkVisible = true;
	const float BLINK_INTERVAL = 80.0f; // ajusta esto
#pragma endregion

};
