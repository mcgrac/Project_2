#include "Character.h"
#include "Engine.h"

Character::Character(std::string _name, int _health, int _maxHealth, int _experience, int _initiative,
	int _maxInitiative, int _power, int _durability, int _maxDurability, int _speed,
	int _lifesteal, int _healingPower, int _poisonedStatMod, int _burnedStatMod, 
	int _level, int _maxHealthLevelScaling, int _speedLevelScaling, int _powerLevelScaling) :
	name(_name), health(_health), maxHealth(_maxHealth), experience(_experience), initiative(_initiative), maxInitiative(_maxInitiative),
	power(_power), durability(_durability), maxDurability(_maxDurability), speed(_speed), lifesteal(_lifesteal), healingPower(_healingPower),
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

//void Character::AttackPhysical(Character& target, int damage)
//{
//	target.ReceivePhysicalDamage(damage);
//	Heal(damage * lifesteal);
//}
//
//void Character::AttackMagical(Character& target, int damage)
//{
//	target.ReceiveMagicalDamage(damage);
//	Heal(damage * lifesteal);
//}

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
	if (health == 0 && attacker != nullptr) {
		killedBy = attacker;
	}
}

void Character::ReceiveMagicalDamage(int damageReceived, Character* attacker)
{
	int currentHealth = health;
	currentHealth -= damageReceived;
	health = std::max(0, currentHealth); //avoids having negative health

	//check if character is dead
	if (health == 0 && attacker != nullptr) {
		killedBy = attacker;
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

	// Update render position using your PhysBody helper
	//int x, y;
	//pbody->GetPosition(x, y);
	//position.setX((float)x);
	//position.setY((float)y);

	//L10: TODO 7: Center the camera on the player
	//Vector2D mapSize = Engine::GetInstance().map->GetMapSizeInPixels();
	//float limitLeft = (float)Engine::GetInstance().render->camera.w / 4;
	//float limitRight = (float)mapSize.getX() - Engine::GetInstance().render->camera.w * 3 / 4;
	//if (position.getX() - limitLeft > 0 && position.getX() < limitRight) {
	//	Engine::GetInstance().render->camera.x = (int)-position.getX() + (int)(Engine::GetInstance().render->camera.w / 4);
	//}
	//else if (position.getX() <= limitLeft) {
	//	Engine::GetInstance().render->camera.x = 0;
	//}
	//else {
	//	Engine::GetInstance().render->camera.x = -(float)mapSize.getX() + Engine::GetInstance().render->camera.w;
	//}

	// L10: TODO 5: Draw the player using the texture and the current animation frame
	//Engine::GetInstance().render->DrawTexture(texture, x - texW / 2, y - texH / 2, &animFrame);

}

void Character::AddSkill(Skill skill)
{
	if (skills.size() < 5)
	{
		skills.push_back(skill);
	}
}

void Character::UseSkill(int index, Character& target)
{
	if (index >= 0 && index < skills.size())
	{
		Skill& skill = skills[index];

		if (initiative >= skill.initiativeCost)
		{
			initiative -= skill.initiativeCost;
			skill.Use(*this, target);
		}
	}
}

void Character::ModifyDurability(int amount)
{
	durability = std::max(0, std::min(maxDurability, durability + amount));
}

void Character::SetBurned(bool state, int damage)
{
	isBurned = state;
	burnedStatMod = damage;
}

void Character::SetPoisoned(bool state, int damage)
{
	isPoisoned = state;
	poisonStatMod = damage;
}
