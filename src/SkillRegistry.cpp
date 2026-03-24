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
            Skill s("Red Dance", DamageType::Magical, 10, 0.25f, cost);
            s.AddEffect({
                "Inflict 10 Fire",
                [](Character* caster, Character* target) {
                    target->SetBurned(true, 10, caster);
                }
                });
            return s;
        });

    Register("divine_light", [](int cost) {
        Skill s("Divine Light", DamageType::Magical, 0, 0.5f, cost);
        s.AddEffect({
            "Heal ally or damage enemy",
            [](Character* caster, Character* target) {
                target->Heal((int)(caster->GetPower() * 0.5f));
            }
            });
        return s;
        });

    Register("laser_upward", [](int cost) {
        Skill s("Laser Upward", DamageType::Magical, 15, 0.2f, cost);
        return s;
        });

    Register("ascend", [](int cost) {
        Skill s("Ascend", DamageType::None, 0, 0.0f, cost);
        s.AddEffect({
            "Buff team PhysDmg",
            [](Character* caster, Character* target) {
                // aplicar cuando la logica de equipo este lista
            }
            });
        return s;
        });

    Register("healing_halo", [](int cost) {
        Skill s("Healing Halo", DamageType::None, 0, 0.0f, cost);
        s.AddEffect({
            "Heal all allies over time",
            [](Character* caster, Character* target) {
                target->Heal((int)(caster->GetPower() * 0.3f));
            }
            });
        return s;
        });
#pragma endregion

#pragma region THERESIA

    Register("encourage", [](int cost) {
        Skill s("Encourage", DamageType::None, 0, 0.0f, cost);
        s.AddEffect({
            "Grant your team 5 Durability and 10 Initiative",
            [](Character* caster, Character* target) {
                // aplicar cuando la logica de equipo este lista
            }
            });
        return s;
        });

    Register("battle_fury", [](int cost) {
        Skill s("Battle Fury", DamageType::Magical, 0, 0.5f, cost);
        s.AddEffect({
            "Grant your team 5% Power and Speed",
            [](Character* caster, Character* target) {
                // aplicar cuando la logica de equipo este lista
            }
            });
        return s;
        });

    Register("slash", [](int cost) {
        Skill s("Slash", DamageType::Physical, 10, 0.1f, cost);
        s.AddEffect({
            "Steal 5 durability",
            [](Character* caster, Character* target) {
                caster->ModifyDurability(5);
                target->ModifyDurability(-5);
            }
            });
        return s;
        });

    Register("shield_bash", [](int cost) {
        Skill s("Shield Bash", DamageType::Physical, 5, 0.0f, cost);
        s.AddEffect({
            "Reduce initiative by 15 + level",
            [](Character* caster, Character* target) {
                target->AddInitiative(-15 - caster->GetLevel());
            }
            });
        return s;
        });

    Register("double_blade", [](int cost) {
        Skill s("Double Blade", DamageType::Physical, 10, 0.25f, cost);
        s.AddEffect({
            "Inflict 0(+FireMod) Fire",
            [](Character* caster, Character* target) {
                if ((int)caster->GetFirePower() > 0)
                {
                    target->SetBurned(true, (int)caster->GetFirePower(), caster);
                }
            }
            });
        return s;
        });
#pragma endregion

#pragma region GERBERA

    Register("fire_charge", [](int cost) {
        Skill s("Fire Charge", DamageType::Physical, 10, 0.35f, cost);
        s.AddEffect({
            "Inflict 5 Fire",
            [](Character* caster, Character* target) {
                target->SetBurned(true, 5, caster);
            }
            });
        return s;
        });

    Register("charge_arrow", [](int cost) {
        Skill s("Charge Arrow", DamageType::None, 0, 0.0f, cost);
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
        Skill s("Green Arrow", DamageType::Physical, 20, 0.1f, cost);
        s.AddEffect({
            "Inflict 10 poison",
            [](Character* caster, Character* target) {
                target->SetPoisoned(true, 10, caster);
            }
            });
        return s;
        });

    Register("charged_arrow", [](int cost) {
        Skill s("Charged Arrow", DamageType::None, 0, 0.0f, cost);
        s.AddEffect({
            "Waste all Initiative and gain 50% of it as Power",
            [](Character* caster, Character* target) {
                caster->ModifyPower((int)(caster->GetCurrentInitiative() * 0.5f));
            }
            });
        return s;
        });

    Register("fire_arrow_2", [](int cost) {
        Skill s("Fire Arrow 2", DamageType::Physical, 15, 0.1f, cost);
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
        Skill s("Slam", DamageType::Physical, 30, 0.1f, cost);
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
        Skill s("Charge", DamageType::None, 0, 0.0f, cost);
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
        Skill s("Bite", DamageType::Physical, 10, 0.6f, cost);
        return s;
        });

    // Charge: Gain 25 Speed and 20 Power
    Register("rex_charge", [](int cost) {
        Skill s("Charge", DamageType::None, 0, 0.0f, cost);
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
        Skill s("Bonk Fire", DamageType::Magical, 25, 0.2f, cost);
        s.AddEffect({
            "Inflict 15 Fire and 15 Poison",
            [](Character* caster, Character* target) {
                target->SetBurned(true, 15, caster);
                target->SetPoisoned(true, 15, caster);
            }
            });
        return s;
        });
#pragma endregion

#pragma endregion



}
