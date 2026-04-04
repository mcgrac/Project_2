#include "CharacterFactory.h"
#include "Character.h"
#include "Markus.h"
#include "CharacterFactory.h"
#include "Character.h"
#include "SkillRegistry.h"
#include "UpgradeTree.h"
#include "UpgradeTier.h"
#include "Upgrade.h"
#include "pugixml.hpp"
#include "Log.h"
#include <string>
#include <unordered_map>

CharacterFactory::CharacterFactory()
{
}

CharacterFactory::~CharacterFactory()
{
}

Character* CharacterFactory::Create(const std::string& name)
{


    std::string path = "Assets/Characters/" + name + ".xml";

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(path.c_str());

    if (!result)
    {
        LOG("CharacterFactory: no se pudo cargar '%s': %s", path.c_str(), result.description());
        return nullptr;
    }

    pugi::xml_node root = doc.child("character");

    //  Stats 
    pugi::xml_node stats = root.child("stats");

    int health = stats.attribute("health").as_int();
    int maxHealth = stats.attribute("maxHealth").as_int();
    int power = stats.attribute("power").as_int();
    int durability = stats.attribute("durability").as_int();
    int maxDurability = stats.attribute("maxDurability").as_int();
    int speed = stats.attribute("speed").as_int();
    int lifesteal = stats.attribute("lifesteal").as_int();
    float healingPower = stats.attribute("healingPower").as_float();
    float poisonPower = stats.attribute("poisonPower").as_float();
    float firePower = stats.attribute("firePower").as_float();
    int maxInitiative = stats.attribute("maxInitiative").as_int();

    //  Level scaling 
    pugi::xml_node scaling = root.child("levelScaling");

    int hpScaling = scaling.attribute("maxHealth").as_int();
    int pwrScaling = scaling.attribute("power").as_int();
    int spdScaling = scaling.attribute("speed").as_int();

    //  Build character
    Character* character = new Character(
        Vector2D(0.0f, 0.0f),
        name,
        health, maxHealth,
        0,              // experience
        0,              // initiative
        maxInitiative,
        power,
        durability, maxDurability,
        speed,
        lifesteal,
        healingPower,
        poisonPower,
        firePower,
        0,              // poisonStatMod
        0,              // burnedStatMod
        1,              // level
        hpScaling,
        spdScaling,
        pwrScaling
    );

    //  Visuals (animations)
    pugi::xml_node visuals = root.child("visuals");

    std::string spritesheet = visuals.attribute("spritesheet").as_string();
    std::string tsx = visuals.attribute("tsx").as_string();

    std::unordered_map<std::string, AnimAlias> animData;
    std::unordered_map<int, std::string> aliases;

    pugi::xml_node animations = visuals.child("animations");

    for (pugi::xml_node anim : animations.children("anim"))
    {
        std::string name = anim.attribute("name").as_string();
        int tile = anim.attribute("tile").as_int();
        bool loop = anim.attribute("loop").as_bool(true); // true por defecto

        animData[name] = { tile, loop }; //anim data -> tile + loop (false or true)
        aliases[tile] = name; //aliases, id -> tile
    }

    character->LoadVisuals(spritesheet, tsx, aliases, animData);

    //  Skills 
    SkillRegistry& skillRegistry = SkillRegistry::GetInstance();

    for (pugi::xml_node skillNode : root.child("skills").children("skill"))
    {
        std::string skillId = skillNode.attribute("id").as_string();
        int skillCost = skillNode.attribute("initiativeCost").as_int();

        Skill skill = skillRegistry.Create(skillId, skillCost);
        character->AddSkill(skill);
    }

    //  Upgrade tree 
    for (pugi::xml_node tierNode : root.child("upgradeTree").children("tier"))
    {
        int requiredLevel = tierNode.attribute("requiredLevel").as_int();

        pugi::xml_node nodeA = tierNode.child("optionA");
        pugi::xml_node nodeB = tierNode.child("optionB");

        Upgrade optionA(
            nodeA.attribute("name").as_string(),
            nodeA.attribute("description").as_string(),
            ParseUpgradeEffects(nodeA.attribute("effect").as_string(),
                                nodeA.attribute("effect2").as_string())
        );

        Upgrade optionB(
            nodeB.attribute("name").as_string(),
            nodeB.attribute("description").as_string(),
            ParseUpgradeEffects(nodeB.attribute("effect").as_string(),
                                nodeB.attribute("effect2").as_string())
        );

        character->AddUpgradeTier(UpgradeTier(requiredLevel, optionA, optionB));
    }

    LOG("CharacterFactory: '%s' creado correctamente.", name.c_str());

    return character;
}

//  Converts the effect attribute of the xml into a lambda function.
//  "stat+value" (e.g.: "power+15", "maxHealth+20")
std::function<void(Character&)> CharacterFactory::ParseUpgradeEffect(const std::string& effect)
{
    size_t plusPos = effect.find('+');
    if (plusPos == std::string::npos)
    {
        LOG("CharacterFactory: efecto de upgrade no reconocido: '%s'", effect.c_str());
        return [](Character&) {};
    }

    std::string stat = effect.substr(0, plusPos);
    int value = std::stoi(effect.substr(plusPos + 1));

    if (stat == "power") return [value](Character& c) { c.ModifyPower(value); };
    if (stat == "maxHealth") return [value](Character& c) { c.ModifyMaxHealth(value); };
    if (stat == "speed") return [value](Character& c) { c.ModifySpeed(value); };
    if (stat == "healingPower") return [value](Character& c) { c.ModifyHealingPower(value); };

    LOG("CharacterFactory: stat de upgrade no reconocida: '%s'", stat.c_str());
    return [](Character&) {};
}

std::function<void(Character&)> CharacterFactory::ParseUpgradeEffects(const std::string& effect1, const std::string& effect2)
{
    std::function<void(Character&)> fn1 = ParseUpgradeEffect(effect1);
    std::function<void(Character&)> fn2 = ParseUpgradeEffect(effect2);

    return [fn1, fn2](Character& c)
    {
            fn1(c);
            fn2(c);
    };
}
