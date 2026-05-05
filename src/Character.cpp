#include "Character.h"
#include "Engine.h"
#include "Render.h"
#include "Textures.h"
#include "Log.h"
#include "Item.h"

Character::Character(Vector2D _position, std::string _name, int _health, int _maxHealth, int _experience, int _initiative,
	int _maxInitiative, int _basePower, int _bonusPower, int _totalPower, int _totalDurability, int _baseDurability, int _bonusDurability, int _maxDurability, int _baseSpeed,
	int _bonusSpeed, int _totalSpeed, int _lifesteal, float _healingPower, float _poisonPower, float _firePower, int _poisonedStatMod, int _burnedStatMod,
	int _level, int _maxHealthLevelScaling, int _speedLevelScaling, int _powerLevelScaling) :
	position(_position), name(_name), health(_health), maxHealth(_maxHealth), experience(_experience), initiative(_initiative), maxInitiative(_maxInitiative),
	basePower(_basePower), bonusPower(_bonusPower), totalPower(_totalPower),totalDurability(_totalDurability), baseDurability(_baseDurability), bonusDurability(_bonusDurability), 
	maxDurability(_maxDurability), baseSpeed(_baseSpeed), bonusSpeed(_bonusSpeed), totalSpeed(_totalSpeed), lifesteal(_lifesteal), healingPower(_healingPower), poisonPower(_poisonPower), firePower(_firePower),
	isPoisoned(false), isBurned(false), poisonStatMod(_poisonedStatMod), burnedStatMod(_burnedStatMod), level(_level), 
	maxHealthLevelScaling(_maxHealthLevelScaling), powerLevelScaling(_powerLevelScaling), speedLevelScaling(_speedLevelScaling), isAlive(true), isAllied(false)
{
	SetTotalPower();
	SetTotalSpeed();
	SetTotalDurability();

	//initialize upgrade tree
	upgradeTree = new UpgradeTree();
}

Character::~Character()
{
#if _DEBUG
	LOG("\033[1;32m|Destructor Character: %s\033[0m", name.c_str());
#endif // _DEBUG

	if (texture != nullptr)
	{
		Engine::GetInstance().textures->UnLoad(texture);
		texture = nullptr;
	}

	killedBy = nullptr;
	poisonedBy = nullptr;
	burnedBy = nullptr;

	//upgradeTree
	delete upgradeTree;
	upgradeTree = nullptr;

}

void Character::Update(float dt)
{
	Draw(dt);

	//check death animation
	if (pendingToDie) {
		anims.SetCurrent("dead");
		if (anims.IsCurrentFinished()) {
			isAlive = false;
			pendingToDie = false;
		}
	}
}

void Character::Heal(int amount)
{
	int currentHealth = health;
	currentHealth += amount * (1 + healingPower);
	health = std::min(maxHealth, currentHealth);
}

void Character::FullyHeal()
{
	int currentHealth;
	currentHealth = maxHealth;
	health = currentHealth;
}

void Character::ReceivePhysicalDamage(int damageReceived, Character* attacker)
{
	if (!isAlive) { return; } //dont do anything if the character is already dead
	int scaledDamage = (int)(damageReceived * incomingDamageMultiplier);

	// LOG DE COMPROBACIÓN
	if (incomingDamageMultiplier != 1.0f) {
		LOG("DAMAGE CALC [%s]: Base: %d | Multiplier: %.2f | Final: %d (Reduced: %d)",
			name.c_str(), damageReceived, incomingDamageMultiplier, scaledDamage, (damageReceived - scaledDamage));
	}

	int currentHealth = health;
	currentHealth -= std::max(0, scaledDamage - totalDurability); //avoids that damage < 0
	health = std::max(0, currentHealth); //avoids having negative health

	//check if character is dead
	if (health <= 0) {
		pendingToDie = true;
		//isAlive = false;
		if (attacker != nullptr) {
			killedBy = attacker;
		}
	}
}

void Character::ReceiveMagicalDamage(int damageReceived, Character* attacker)
{
	if (!isAlive) { return; } //dont do anything if the character is already dead
	int scaledDamage = (int)(damageReceived * incomingDamageMultiplier);

	// LOG DE COMPROBACIÓN
	if (incomingDamageMultiplier != 1.0f) {
		LOG("DAMAGE CALC [%s]: Base: %d | Multiplier: %.2f | Final: %d (Reduced: %d)",
			name.c_str(), damageReceived, incomingDamageMultiplier, scaledDamage, (damageReceived - scaledDamage));
	}

	int currentHealth = health;
	currentHealth -= scaledDamage;
	health = std::max(0, currentHealth); //avoids having negative health

	//check if character is dead
	if (health <= 0) {
		pendingToDie = true;
		//isAlive = false;
		if (attacker != nullptr) {
			killedBy = attacker;
		}
	}
}

void Character::GainExperience(int amount)
{
	experience += amount;

	if (experience >= 100) {
		experience -= 100;		//if gained xp is 150, do level up and mantain that 50xp that is left.
		LevelUp();
	}
}

void Character::LevelUp()
{
	level++;

	//update stats
	maxHealth += maxHealthLevelScaling;
	health += maxHealthLevelScaling;
	basePower += powerLevelScaling;
	baseSpeed += speedLevelScaling;
}

void Character::Draw(float dt) 
{
	anims.Update(dt);

	const SDL_Rect& animFrame = anims.GetCurrentFrame();

	int drawX = (int)position.getX() - animFrame.w / 2;
	int drawY = (int)position.getY() - animFrame.h / 2;
	
	double rotation = 0.0f;
	if (!isAllied) { rotation = -1.0; }

	float speed = 1.0f;

	Engine::GetInstance().render->DrawTexture(
		texture,
		drawX,
		drawY,
		&animFrame,
		speed,
		rotation
	);
}

void Character::AddSkill(Skill skill)
{
	if (skills.size() < 5)
	{
		skills.push_back(skill);
	}
}

void Character::UseSkill(int index, Character* target)
{
	if (index >= 0 && index < skills.size())
	{
		Skill& skill = skills[index];

		if (initiative >= skill.GetInitiativeCost())
		{
			initiative -= skill.GetInitiativeCost();
			skill.Use(this, target);
		}
	}
}

void Character::ModifyDurability(int amount)
{
	totalDurability = std::max(0, std::min(maxDurability, totalDurability + amount));
}

void Character::AddInitiative(int amount)
{
	initiative += amount;
	if (initiative < 0) { initiative = 0; }
}

void Character::SetBurned(bool state, int damage, Character* attacker)
{
	isBurned = state;

	if(isBurned) //if character burns
	{
		if (burnedStatMod > 0) { //if already burned
			burnedStatMod += damage;
		}
		else {
			burnedStatMod = damage;
		}

		burnedBy = attacker; //the one that takes the kill by burning is the last character that burn
	}
	else {//burning = false (clean burning effect)
		burnedStatMod = 0;
		burnedBy = nullptr;
	}
}

void Character::SetPoisoned(bool state, int damage, Character* attacker)
{
	isPoisoned = state;

	if(isPoisoned)
	{
		if (poisonStatMod > 0) { //if already poisoned
			poisonStatMod += damage;
		}
		else {
			poisonStatMod = damage;
		}

		poisonedBy = attacker; //the one that takes the kill by poisoning is the last character that poison
	}
	else {
		poisonStatMod = 0;
		poisonedBy = nullptr;
	}
}

void Character::ClearStatusEffects()
{
	isPoisoned = false;
	poisonStatMod = 0;
	isBurned = false;
	burnedStatMod = 0;
}

void Character::LoadVisuals(const std::string& spriteSheet, const std::string& tsx,
							const std::unordered_map<int, std::string>& aliases,
							const std::unordered_map<std::string, AnimAlias>& animData)
{
	anims.LoadFromTSX(tsx.c_str(), aliases);

	// aplicar loop a cada animación
	for (auto it = animData.begin(); it != animData.end(); ++it)
	{
		const std::string& name = it->first;
		const AnimAlias& data = it->second;

		anims.SetLoop(name, data.loop);
	}

	anims.SetCurrent("idle");

	texture = Engine::GetInstance().textures->Load(spriteSheet.c_str());
	if (texture == nullptr) { LOG("Fail texture loading"); }
}

void Character::PlayAnimation(const std::string& name)
{
	if (anims.Has(name))
	{
		anims.SetCurrent(name);
	}
	else
	{
		LOG("Animation '%s' not found for character %s", name.c_str(), GetName().c_str());
		anims.SetCurrent("idle");
	}
}

void Character::ClearBonusStats()
{
	bonusDurability = 0;
	bonusPower = 0;
	bonusSpeed = 0;
}

//bool Character::EquipItem(Item* item)
//{
//	if (equippedItems.size() >= MAX_EQUIPPED_ITEMS) { return false; }
//
//	equippedItems.push_back(item);
//
//	item->Use(this);
//#if _DEBUG
//	DebugInventory();
//#endif // _DEBUG
//	return true;
//}

//void Character::DebugInventory()
//{
//	LOG("\033[1;32m-----------INVENTORY OF %s --------------\033[0m", name.c_str());
//	for (auto& item : equippedItems) {
//		LOG("\033[1;33mItem: %s equipped\033[0m", item->GetName().c_str());
//		EquippableItem* equippable = dynamic_cast<EquippableItem*>(item);
//		for (auto& stat : equippable->GetItemStats()) {
//			LOG("\033[1;33mStat: %d Value: %d\033[0m", stat.type, stat.value);
//		}
//	}
//	LOG("------------------------------------------");
//}


void Character::PrintDebugInfo(){
	LOG("========================================");
	LOG("\033[1;32mCHARACTER: %s | Level %d\033[0m", name.c_str(), level);
	LOG("  \033[1;33mHP: %d/%d  Power: %d  Speed: %d  Durability: %d", health, maxHealth, totalPower, totalSpeed, totalDurability);
	LOG("  Initiative: %d/%d  Lifesteal: %d", initiative, maxInitiative, lifesteal);
	LOG("  HealingPower: %.2f  PoisonPower: %.2f  FirePower: %.2f", healingPower, poisonPower, firePower);

	LOG("--- SKILLS (%d) ---", (int)skills.size());
	for (int i = 0; i < (int)skills.size(); ++i)
	{
		LOG("  [%d] %s | InitiativeCost: %d | Effects: %d",
			i,
			skills[i].GetName().c_str(),
			skills[i].GetInitiativeCost(),
			(int)skills[i].GetEffects().size()
		);
		for (int j = 0; j < (int)skills[i].GetEffects().size(); ++j)
		{
			LOG("      Effect [%d]: %s", j, skills[i].GetEffects()[j].description.c_str());
		}
	}

	LOG("--- UPGRADE TREE ---\033[0m");
	if (upgradeTree == nullptr)
	{
		LOG("  [NULL] No upgrade tree.");
	}
	else
	{
		upgradeTree->PrintDebugInfo();
	}

	LOG("========================================");
}
