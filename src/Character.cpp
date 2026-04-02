#include "Character.h"
#include "Engine.h"
#include "Render.h"
#include "Textures.h"
#include "Log.h"

Character::Character(Vector2D _position, std::string _name, int _health, int _maxHealth, int _experience, int _initiative,
	int _maxInitiative, int _power, int _durability, int _maxDurability, int _speed,
	int _lifesteal, float _healingPower, float _poisonPower, float _firePower, int _poisonedStatMod, int _burnedStatMod,
	int _level, int _maxHealthLevelScaling, int _speedLevelScaling, int _powerLevelScaling) :
	position(_position), name(_name), health(_health), maxHealth(_maxHealth), experience(_experience), initiative(_initiative), maxInitiative(_maxInitiative),
	power(_power), durability(_durability), maxDurability(_maxDurability), speed(_speed), lifesteal(_lifesteal), healingPower(_healingPower),
	pisonPower(_poisonPower), firePower(_firePower),
	isPoisoned(false), isBurned(false), poisonStatMod(_poisonedStatMod), burnedStatMod(_burnedStatMod), level(_level), 
	maxHealthLevelScaling(_maxHealthLevelScaling), powerLevelScaling(_powerLevelScaling), speedLevelScaling(_speedLevelScaling), isAlive(true)
{
	//add loading of the spritesheet

	//initialize upgrade tree
	upgradeTree = new UpgradeTree();
}

Character::~Character()
{
	//add cleanup of the textures and sounds
}

void Character::Update(float dt)
{
	Draw(dt);
}

void Character::Heal(int amount)
{
	int currentHealth = health;
	currentHealth += amount * healingPower;
	health = std::min(maxHealth, currentHealth);
}

void Character::ReceivePhysicalDamage(int damageReceived, Character* attacker)
{
	int currentHealth = health;
	currentHealth -= std::max(0, damageReceived - durability); //avoids that damage < 0
	health = std::max(0, currentHealth); //avoids having negative health

	//check if character is dead
	if (health <= 0) {
		isAlive = false;
		if (attacker != nullptr) {
			killedBy = attacker;
		}
	}
}

void Character::ReceiveMagicalDamage(int damageReceived, Character* attacker)
{
	int currentHealth = health;
	currentHealth -= damageReceived;
	health = std::max(0, currentHealth); //avoids having negative health

	//check if character is dead
	if (health <= 0) {
		isAlive = false;
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
	power += powerLevelScaling;
	speed += speedLevelScaling;
}

void Character::Draw(float dt) 
{
	anims.Update(dt);

	const SDL_Rect& animFrame = anims.GetCurrentFrame();

	int drawX = (int)position.getX() - animFrame.w / 2;
	int drawY = (int)position.getY() - animFrame.h / 2;

	Engine::GetInstance().render->DrawTexture(
		texture,
		drawX,
		drawY,
		&animFrame
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
	durability = std::max(0, std::min(maxDurability, durability + amount));
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

void Character::LoadVisuals(const std::string& spriteSheet, const std::string& tsx, const std::unordered_map<int, std::string>& aliases)
{
	anims.LoadFromTSX(tsx.c_str(), aliases);
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


void Character::PrintDebugInfo(){
	LOG("========================================");
	LOG("CHARACTER: %s | Level %d", name.c_str(), level);
	LOG("  HP: %d/%d  Power: %d  Speed: %d  Durability: %d", health, maxHealth, power, speed, durability);
	LOG("  Initiative: %d/%d  Lifesteal: %d", initiative, maxInitiative, lifesteal);
	LOG("  HealingPower: %.2f  PoisonPower: %.2f  FirePower: %.2f", healingPower, pisonPower, firePower);

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

	LOG("--- UPGRADE TREE ---");
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
