#include "Character.h"
#include "Engine.h"

Character::Character(std::string _name, int _health, int _maxHealth, int _experience, int _initiative,
	int _maxInitiative, int _power, int _durability, int _maxDurability, int _speed,
	int _lifesteal, int _healingPower, bool _isPoisoned, int _poisonedStatMod, int _burnedStatMod, 
	int _level, int _maxHealthLevelScaling, int _speedLevelScaling, int _powerLevelScaling) :
	name(_name), health(_health), maxHealth(_maxHealth), experience(_experience), initiative(_initiative), maxInitiative(_maxInitiative),
	power(_power), durability(_durability), maxDurability(_maxDurability), speed(_speed), lifesteal(_lifesteal), healingPower(_healingPower),
	isPoisoned(false), isBurned(false), poisonStatMod(_poisonedStatMod), burnedStatMod(_burnedStatMod), level(_level), 
	maxHealthLevelScaling(_maxHealthLevelScaling), powerLevelScaling(_powerLevelScaling), speedLevelScaling(_speedLevelScaling)
{

}

Character::~Character()
{

}

void Character::Attack(Character target)
{
	int currentHealth = target.health;
}

void Character::Heal(int amount)
{
	int currentHealth = health;
	currentHealth += amount * healingPower;
}

void Character::ReceivePhysicalDamage(int damageReceived)
{
	int currentHealth = health;
	currentHealth -= (damageReceived + durability);
	health = currentHealth;
}

void Character::ReceiveMagicalDamage(int damageReceived)
{
	int currentHealth = health;
	currentHealth -= damageReceived;
	health = currentHealth;
}

void Character::GainExperience(int amount)
{
	experience += amount;
}

void Character::LevelUp()
{
	level++;
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