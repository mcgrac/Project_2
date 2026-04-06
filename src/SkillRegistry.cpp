#include "SkillRegistry.h"
#include "Character.h"
#include "Log.h"

//  Singleton
SkillRegistry& SkillRegistry::GetInstance()
{
    static SkillRegistry instance;
    return instance;
}

Skill SkillRegistry::Create(const std::string& id, int initiativeCost) const
{
    auto it = registry.find(id);
    if (it != registry.end())
    {
        return it->second(initiativeCost);
    }

    LOG("SkillRegistry: skill '%s' no encontrada.", id.c_str());
    return Skill("Unknown", DamageType::None, 0, 0.0f, initiativeCost);
}

void SkillRegistry::Register(const std::string& id, std::function<Skill(int)> builder)
{
    registry[id] = builder;
}

SkillRegistry::SkillRegistry()
{

#pragma region ALLIES

#pragma region MARKUS
    Register("red_dance", [](int cost)
        {
            Skill s("Red Dance", DamageType::Magical, 10, 0.15f, cost, "red_dance");
            s.AddEffect({
                "Inflict 10 Fire",
                [](Character* caster, Character* target) {
                    int damageFirePower = (int)(10 * (1 + caster->GetFirePower() / 100.0f));
                    target->SetBurned(true, damageFirePower, caster);
                }
                });
            return s;
        });

    //incomplete
    Register("laser_upward", [](int cost) {
        Skill s("Laser Upward", DamageType::Magical, 15, 0.2f, cost, "laser_upward");
        s.AddEffect({
            "Heal ally or damage enemy",
            [](Character* caster, Character* target) {
                //target->Heal((int)(caster->GetPower() * 0.3f));
            }
            });
        return s;
        });

    Register("blue_dance", [](int cost) {
        Skill s("Blue Dance", DamageType::Magical, 0, 0.0f, cost, "blue_dance");
        s.SetHasAreaEffect(true);
        s.SetAreaEffectTargetAllies(true);
        s.AddEffect({
            "Increate base team power by 20",
            [](Character* caster, Character* attacker) {
                caster->ModifyPower(20);
            }
            });
        return s;
        });

    Register("flame", [](int cost) {
        Skill s("Flame", DamageType::None, 0, 0.0f, cost, "flame");
        s.SetHasAreaEffect(true);
        s.SetAreaEffectTargetAllies(false);
        s.AddEffect({
            "Burn all enemies for 5 and reduce durability by 5 (+50% FireMod)",
            [](Character* caster, Character* target) {
                int damageFirePower = (int)(5 * (1 + caster->GetFirePower() / 100.0f));
                target->SetBurned(true, damageFirePower, caster);
            }
            });
        return s;
        });

    //incompleta
    Register("healing_halo", [](int cost) {
        Skill s("Healing Halo", DamageType::None, 0, 0.0f, cost, "healing_halo");
        s.AddEffect({
            "Heal and clean poison effect and burn effect to an ally",
            [](Character* caster, Character* target) {
                target->Heal((int)(caster->GetPower() * 0.15f));
                target->SetBurned(false, 0, caster);
                target->SetPoisoned(false, 0, caster);
            }
            });
        return s;
        });
#pragma endregion

#pragma region THERESIA

    Register("encourage", [](int cost) {
        Skill s("Encourage", DamageType::None, 0, 0.0f, cost, "encourage");
        s.SetHasAreaEffect(true);
        s.SetAreaEffectTargetAllies(true);
        s.AddEffect({
            "Grant your team 5 Durability and 10 Initiative",
            [](Character* caster, Character* target) {
                target->ModifyDurability(5);
                target->AddInitiative(10);
            }
            });
        return s;
        });

    Register("battle_fury", [](int cost) {
        Skill s("Battle Fury", DamageType::Magical, 0, 0.5f, cost, "battle_fury");
        s.SetHasAreaEffect(true);
        s.SetAreaEffectTargetAllies(true);
        s.AddEffect({
            "Grant your team 5% more Power and Speed",
            [](Character* caster, Character* target) {
                target->ModifyPower(target->GetPower() * 0.05f);
                target->ModifySpeed(target->GetSpeed() * 0.05f);
            }
            });
        return s;
        });

    Register("slash", [](int cost) {
        Skill s("Slash", DamageType::Physical, 10, 0.1f, cost, "slash");
        s.AddEffect({
            "Steal 5 durability",
            [](Character* caster, Character* target) {
                caster->ModifyDurability(5);
                target->ModifyDurability(-5);
            }
            });
        return s;
        });

    //probolema deal x + durability
    Register("shield_bash", [](int cost) {
        Skill s("Shield Bash", DamageType::Physical, 5, 0.0f, cost, "shield_bash");
        s.AddEffect({
            "Reduce initiative by 15 + level",
            [](Character* caster, Character* target) {
                target->AddInitiative(-15 - caster->GetLevel());
            }
            });
        return s;
        });

    Register("double_blade", [](int cost) {
        Skill s("Double Blade", DamageType::Physical, 10, 0.25f, cost, "double_blade");
        s.AddEffect({
            "Inflict 10 fire",
            [](Character* caster, Character* target) {
                if ((int)caster->GetFirePower() > 0)
                {
                    int damageFirePower = (int)(10 * (1 + caster->GetFirePower() / 100.0f));
                    target->SetBurned(true, damageFirePower, caster);
                }
            }
            });
        return s;
        });
#pragma endregion

#pragma region GERBERA

    Register("fire_charge", [](int cost) {
        Skill s("Fire Charge", DamageType::Physical, 10, 0.35f, cost, "fire_charge");
        s.AddEffect({
            "Inflict 5 Fire",
            [](Character* caster, Character* target) {
                int damageFirePower = (int)(5 * (1 + caster->GetFirePower() / 100.0f));
                target->SetBurned(true, damageFirePower, caster);
            }
            });
        return s;
        });

    Register("charge_arrow", [](int cost) {
        Skill s("Charge Arrow", DamageType::None, 0, 0.0f, cost, "charge_arrow");
        s.AddEffect({
            "Gain 20 power and 30 initiative",
            [](Character* caster, Character* target) {
                caster->AddInitiative(30);
                caster->ModifyPower(20);
            }
            });
        return s;
        });

    Register("green_arrow", [](int cost) {
        Skill s("Green Arrow", DamageType::Physical, 20, 0.1f, cost, "green_arrow");
        s.AddEffect({
            "Inflict 10 poison",
            [](Character* caster, Character* target) {
                 int damagePoisonPower = (int)(8 * (1 + caster->GetPoisonPower() / 100.0f));
                target->SetPoisoned(true, damagePoisonPower, caster);
            }
            });
        return s;
        });

    Register("charged_arrow", [](int cost) {
        Skill s("Charged Arrow", DamageType::None, 0, 0.0f, cost, "charged_arrow");
        s.AddEffect({
            "Waste all Initiative and gain 50% of it as Power",
            [](Character* caster, Character* target) {
                caster->ModifyPower((int)(caster->GetCurrentInitiative() * 0.5f));
                caster->ResetCurrentInitiative();
            }
            });
        return s;
        });

    Register("fire_arrow_2", [](int cost) {
        Skill s("Fire Arrow 2", DamageType::Physical, 15, 0.1f, cost, "fire_arrow_2");
        s.AddEffect({
            "Deal bonus damage per fire stack and reset fire to 0",
            [](Character* caster, Character* target) {
                int bonusDmg = (int)((1 + caster->GetPower() * 0.1f) * target->GetBurnDamage());
                target->ReceivePhysicalDamage(bonusDmg, caster);
                target->SetBurned(false, 0, caster);
            }
            });
        return s;
        });
#pragma endregion

#pragma endregion

#pragma region ENEMIES

#pragma region RAPTOR

    // Slam: Deal 30(+10%Power) physical damage and heal 10(+20%Power)
    Register("raptor_slam", [](int cost) {
        Skill s("Slam", DamageType::Physical, 30, 0.1f, cost, "raptor_slam");
        s.AddEffect({
            "Heal 10(+20%Power)",
            [](Character* caster, Character* target) {
                caster->Heal(10 + (int)(caster->GetPower() * 0.2f));
            }
            });
        return s;
        });

    // Charge: Gain 25 Durability and 20 Power
    Register("raptor_charge", [](int cost) {
        Skill s("Charge", DamageType::None, 0, 0.0f, cost, "raptor_charge");
        s.AddEffect({
            "Gain 25 Durability and 20 Power",
            [](Character* caster, Character* target) {
                caster->ModifyDurability(25);
                caster->ModifyPower(20);
            }
            });
        return s;
        });
#pragma endregion

#pragma region REX

    // Bite: Deal 10(+60%Power) Physical Damage to a foe in any lane
    Register("rex_bite", [](int cost) {
        Skill s("Bite", DamageType::Physical, 10, 0.6f, cost, "rex_bite");
        return s;
        });

    // Charge: Gain 25 Speed and 20 Power
    Register("rex_charge", [](int cost) {
        Skill s("Charge", DamageType::None, 0, 0.0f, cost, "rex_charge");
        s.AddEffect({
            "Gain 25 Speed and 20 Power",
            [](Character* caster, Character* target) {
                caster->ModifySpeed(25);
                caster->ModifyPower(20);
            }
            });
        return s;
        });
#pragma endregion

#pragma region CHAMAN
    // Bonk Fire: Deal 25(+20%Power) Magic Damage and inflict 15 fire and 15 Poison
    Register("chaman_bonk_fire", [](int cost) {
        Skill s("Bonk Fire", DamageType::Magical, 25, 0.2f, cost, "chaman_bonk_fire");
        s.AddEffect({
            "Inflict 15 Fire and 15 Poison",
            [](Character* caster, Character* target) {
                int damageFirePower = (int)(15 * (1 + caster->GetFirePower() / 100.0f));
                int damagePoisonPower = (int)(15 * (1 + caster->GetPoisonPower() / 100.0f));

                target->SetBurned(true, damageFirePower, caster);
                target->SetPoisoned(true, damagePoisonPower, caster);
            }
            });
        return s;
        });

    Register("chaman_charge", [](int cost) {
        Skill s("Charge", DamageType::Magical, 0, 0.0f, cost, "chaman_charge");
        s.AddEffect({
            "Gain 10 fire mod and 20 power",
            [](Character* caster, Character* target) {
                caster->ModifyPower(20);
                caster->ModifyPoisonPower(20.0f);
                caster->ModifyFirePower(20.0f);
            }
            });
        return s;
        });
#pragma endregion

#pragma endregion



}
