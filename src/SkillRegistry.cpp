#include "SkillRegistry.h"
#include "Character.h"
#include "Log.h"

// ─────────────────────────────────────────────
//  Singleton
// ─────────────────────────────────────────────
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

// ─────────────────────────────────────────────
//  Constructor — registro de todas las skills
//  Para añadir una skill nueva: añade un bloque Register()
//  con su id (debe coincidir con el XML) y su lógica.
// ─────────────────────────────────────────────
SkillRegistry::SkillRegistry()
{
    // ── MARKUS ───────────────────────────────────────────────────────────

    Register("red_dance", [](int cost) {
        Skill s("Red Dance", DamageType::Magical, 10, 0.25f, cost);
        s.AddEffect({
            "Inflict 10 Fire",
            [](Character* caster, Character* target) {
                target->SetBurned(true, 10);
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

    // ── THERESIA ─────────────────────────────────────────────────────────

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
                    target->SetBurned(true, (int)caster->GetFirePower());
                }
            }
        });
        return s;
    });

    // ── GERBERA ──────────────────────────────────────────────────────────

    Register("fire_charge", [](int cost) {
        Skill s("Fire Charge", DamageType::Physical, 10, 0.35f, cost);
        s.AddEffect({
            "Inflict 5 Fire",
            [](Character* caster, Character* target) {
                target->SetBurned(true, 5);
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
                target->SetPoisoned(true, 10);
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
                target->SetBurned(false, 0);
            }
        });
        return s;
    });
}
