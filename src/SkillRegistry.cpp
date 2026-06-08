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
            s.SetDescription("Deal 10(+15 power) Magical damage");
            s.AddEffect({
                "Inflict 10 Fire",
                [](Character* caster, Character* target) {
                    int damageFirePower = (int)(10 * (1 + caster->GetFirePower() / 100.0f));
                    LOG("RedDance: Damage done - %d", damageFirePower);
                    target->SetBurned(true, damageFirePower, caster);
                }
                });
            return s;
        });

    //incomplete
    Register("laser_upward", [](int cost) {
        Skill s("Laser Upward", DamageType::None, 0, 0.0f, cost, "laser_upward");
        s.SetDescription("");
        s.SetHasAreaEffect(true);
        s.SetAreaEffectTargetAllies(true);
        s.AddEffect({
            "Heal 10(+10%Power) your party",
            [](Character* caster, Character* target) {
                target->Heal((int)(10 + caster->GetTotalPower() * 0.10f));
            }
            });
        return s;
        });

    Register("blue_dance", [](int cost) {
        Skill s("Blue Dance", DamageType::Magical, 0, 0.0f, cost, "blue_dance");
        s.SetDescription("");
        s.SetHasAreaEffect(true);
        s.SetAreaEffectTargetAllies(true);
        s.AddEffect({
            "Increate base team power by 20",
            [](Character* caster, Character* target) {
                target->ModifyBonusPower(20);
            }
            });
        return s;
        });

    Register("flame", [](int cost) {
        Skill s("Flame", DamageType::None, 0, 0.0f, cost, "flame");
        s.SetDescription("");
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

    Register("healing_halo", [](int cost) {
        Skill s("Healing Halo", DamageType::None, 0, 0.0f, cost, "healing_halo");
        s.SetDescription("");
        s.SetHasAreaEffect(true);
        s.SetAreaEffectTargetAllies(true);
        s.AddEffect({
            "Heal for 5(+5%Power) and clean Poison and Burn effects to your party",
            [](Character* caster, Character* target) {
                target->Heal((int)( 5 + caster->GetTotalPower() * 0.05f));
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
        s.SetDescription("");
        s.SetHasAreaEffect(true);
        s.SetAreaEffectTargetAllies(true);
        s.AddEffect({
            "Grant your team 5 Durability and 10 Initiative",
            [](Character* caster, Character* target) {
                target->ModifyBonusDurability(5);
                target->AddInitiative(10);
            }
            });
        return s;
        });

    Register("battle_fury", [](int cost) {
        Skill s("Battle Fury", DamageType::Magical, 0, 0.0f, cost, "battle_fury");
        s.SetDescription("");
        s.SetHasAreaEffect(true);
        s.SetAreaEffectTargetAllies(true);
        s.AddEffect({
            "Grant your team 5% more Power and Speed",
            [](Character* caster, Character* target) {
                target->ModifyBonusPower(target->GetTotalPower() * 0.05f);
                target->ModifyBonusSpeed(target->GetTotalPower() * 0.05f);
            }
            });
        return s;
        });

    Register("slash", [](int cost) {
        Skill s("Slash", DamageType::Physical, 10, 0.1f, cost, "slash");
        s.SetDescription("Deal 10 (+10% power) Physical damage");
        s.AddEffect({
            "Steal 5 durability",
            [](Character* caster, Character* target) {
                caster->ModifyBonusDurability(5);
                target->ModifyDurability(-5);
            }
            });
        return s;
        });

    Register("shield_bash", [](int cost) {
        Skill s("Shield Bash", DamageType::Physical, 5, 1.0f, cost, "shield_bash", MultiplierStat::DURABILITY);
        s.SetDescription("Deal 5 (+durability) Physical damage");
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
        s.SetDescription("Deal 10 (+25% power) Physical damage");
        s.AddEffect({
            "Inflict 6 fire",
            [](Character* caster, Character* target) {

                int damageFirePower = (int)(6 * (1 + caster->GetFirePower() / 100.0f));
                target->SetBurned(true, damageFirePower, caster);
            }
            });
        return s;
        });
#pragma endregion

#pragma region GERBERA

    Register("fire_charge", [](int cost) {
        Skill s("Fire Charge", DamageType::Physical, 10, 0.20f, cost, "fire_charge");
        s.SetDescription("Deal 10 (+20% power) Physical damage");
        s.AddEffect({
            "Inflict 3 Fire",
            [](Character* caster, Character* target) {
                int damageFirePower = (int)(3 * (1 + caster->GetFirePower() / 100.0f));
                target->SetBurned(true, damageFirePower, caster);
            }
            });
        return s;
        });

    Register("charge_arrow", [](int cost) {
        Skill s("Charge Arrow", DamageType::None, 0, 0.0f, cost, "charge_arrow");
        s.SetDescription("");
        s.AddEffect({
            "Gain 20 power and 130 initiative",
            [](Character* caster, Character* target) {
                caster->AddInitiative(130);
                caster->ModifyBonusPower(20);
            }
            });
        return s;
        });

    Register("green_arrow", [](int cost) {
        Skill s("Green Arrow", DamageType::Physical, 20, 0.1f, cost, "green_arrow");
        s.SetDescription("Deal 20 (+10% power) Physical damage");
        s.AddEffect({
            "Inflict 7 poison",
            [](Character* caster, Character* target) {
                int damagePoisonPower = (int)(7 * (1 + caster->GetPoisonPower() / 100.0f));
                target->SetPoisoned(true, damagePoisonPower, caster);
            }
            });
        return s;
        });

    Register("charged_arrow", [](int cost) {
        Skill s("Charged Arrow", DamageType::None, 0, 0.0f, cost, "charged_arrow");
        s.SetDescription("");
        s.AddEffect({
            "Waste all Initiative and gain 50% of it as Power",
            [](Character* caster, Character* target) {
                caster->ModifyBonusPower((int)(caster->GetCurrentInitiative() * 0.5f));
                caster->ResetCurrentInitiative();
            }
            });
        return s;
        });

    Register("fire_arrow_2", [](int cost) {
        Skill s("Fire Arrow 2", DamageType::Physical, 15, 0.1f, cost, "fire_arrow_2");
        s.SetDescription("Deal 15 (+10% power) Physical damage");
        s.AddEffect({
            "Deal 1(+3% power) bonus damage per fire stack and reset fire to 0",
            [](Character* caster, Character* target) {
                int fireStacks = target->GetBurnDamage();
                float dmgPerStack = 1.0f + (caster->GetTotalPower() * 0.03f);
                int bonusDmg = (int)(dmgPerStack * fireStacks);
                target->ReceivePhysicalDamage(bonusDmg, caster);
                target->SetBurned(false, 0, nullptr);
            }
            });
        return s;
        });
#pragma endregion

#pragma region IGNIS
    Register("slash_fire", [](int cost) {
        Skill s("Slash Fire", DamageType::Physical, 15, 0.2f, cost, "slash_fire");
        s.SetDescription("Deal 15(+20%Power) Physical Damage ");
        s.AddEffect({
            "Apply 5(+1% Power) Fire",
            [](Character* caster, Character* target) {
                int damageFire = (int)(5 + (0.01 * caster->GetTotalPower()));
                int damageFirePower = (int)(damageFire * (1 + caster->GetFirePower() / 100.0f));
                target->SetBurned(true, damageFirePower, caster);
            }
            });
        return s;
        });

    Register("slash_poison", [](int cost) {
        Skill s("Slash Poison", DamageType::Physical, 15, 2.0f, cost, "slash_poison", MultiplierStat::LEVEL);
        s.SetDescription("Deal 15(+200% CurrentLevel) Physical Damage");
        s.AddEffect({
            "Apply 5 Poison",
            [](Character* caster, Character* target) {
                int damagePoisonPower = (int)(5 * (1 + caster->GetPoisonPower() / 100.0f));
                target->SetPoisoned(true, damagePoisonPower, caster);
            }
            });
        return s;
        });

    Register("taunt", [](int cost) {
        Skill s("Taunt", DamageType::Physical, 0, 0.0f, cost, "taunt");
        s.SetDescription("Gain 5(+100/(MaxHealth/CurrentHealth) Power, 50 Initiative and 20 Lifesteal");
        s.AddEffect({
            "",
            [](Character* caster, Character* target) {
                caster->ModifyBonusPower((int)(5 + (100 / (caster->GetMaxHP() / caster->GetCurrentHP()))));
                caster->AddInitiative(50);
                caster->ModifyLifesteal(20);
            }
            });
        return s;
        });

    Register("self_heal", [](int cost) {
        Skill s("Self Heal", DamageType::None, 0, 0.0f, cost, "self_heal");
        s.SetDescription("Heal 10(+15% Power)% Health");
        s.AddEffect({
            "",
            [](Character* caster, Character* target) {
                float healPercent = 10.0f + caster->GetTotalPower() * 0.15f;
                caster->Heal((int)(caster->GetMaxHP()* healPercent / 100.0f));
            }
            });
        return s;
        });

    Register("double_hit", [](int cost) {
        Skill s("Double Hit", DamageType::Physical, 0, 0.0f, cost, "double_hit");
        s.SetDescription("Deal 10(+50%Power+10%MaxHealth) Physical Damage");
        s.AddEffect({
            "",
            [](Character* caster, Character* target) {
                int damage = (int)(10 + ((caster->GetTotalPower() * 0.5f) + (caster->GetMaxHP() * 0.1f)));
                target->ReceivePhysicalDamage(damage, caster);
            }
            });
        return s;
        });
#pragma endregion

#pragma region JOCHI
    Register("yellow_projectile", [](int cost) {
        Skill s("Yellow Projectile", DamageType::Magical, 3, 0.35f, cost, "yellow_projectile");
        s.SetDescription("Deal 3 (+35% Power) magic damage");
        s.AddEffect({
            "Reduce his Power by 5(+10% Power)",
            [](Character* caster, Character* target) {
                int modifier = (int)(5 + (target->GetTotalPower() * 0.10f));
                target->ModifyBasePower(-modifier);
            }
            });
        return s;
        });

    //revisar
    Register("green_fire", [](int cost) {
        Skill s("Green Fire", DamageType::None, 0, 0.0f, cost, "green_fire");
        s.SetDescription("Apply me 5 Fire and Posion and double that amount to the enemy party");
        s.SetHasAreaEffect(true);
        s.SetAreaEffectTargetAllies(false);
        s.AddEffect({
            "",
            [](Character* caster, Character* target) {
                int damagePoisonPowerAlly = (int)(5 * (1 + caster->GetPoisonPower() / 100.0f));
                int damageFirePowerAlly = (int)(5 * (1 + caster->GetFirePower() / 100.0f));
                caster->SetBurned(true, damagePoisonPowerAlly, caster);
                caster->SetPoisoned(true, damageFirePowerAlly, caster);

                int damagePoisonPowerEnemy = (int)(10 * (1 + caster->GetPoisonPower() / 100.0f));
                int damageFirePowerEnemy = (int)(10 * (1 + caster->GetFirePower() / 100.0f));
                target->SetBurned(true, damagePoisonPowerEnemy, caster);
                target->SetPoisoned(true, damageFirePowerEnemy, caster);
            }
            });
        return s;
        });

    Register("green_orbs", [](int cost) {
        Skill s("Green Orbs", DamageType::Physical, 0, 0.0f, cost, "green_orbs");
        s.SetDescription("Heal 10(+30%Power)% of the missing health of all the party members");
        s.SetHasAreaEffect(true);
        s.SetAreaEffectTargetAllies(true);
        s.AddEffect({
            "",
            [](Character* caster, Character* target) {
                int missingHealth = target->GetMaxHP() - target->GetCurrentHP();
                int healing = (int)(missingHealth * (0.10f + (target->GetTotalPower() * 0.30f / 100.0f)));
                target->Heal(healing);
            }
            });
        return s;
        });

    Register("yellow_tornado", [](int cost) {
        Skill s("Yellow Tornado", DamageType::Magical, 5, 0.1f, cost, "yellow_tornado");
        s.SetDescription("Deal 5(+10% Power) Magic Damage");
        s.SetHasAreaEffect(true);
        s.SetAreaEffectTargetAllies(true);
        s.AddEffect({
            "Reduce the Initiative by 20",
            [](Character* caster, Character* target) {
                target->AddInitiative(-20);
            }
            });
        return s;
        });

    Register("yellow_explosion", [](int cost) {
        Skill s("Yellow Explosion", DamageType::Physical, 0, 0.0f, cost, "yellow_explosion");
        s.SetDescription("");
        s.AddEffect({
            "Burn for 4 and deal 150% of the current fire on the enemy (+50% Power) as magic damage",
            [](Character* caster, Character* target) {
                int damageFirePower = (int)(4 * (1 + caster->GetFirePower() / 100.0f));
                target->SetBurned(true, damageFirePower, caster);

                int damage = (int)(target->GetBurnDamage() * 1.5f + (caster->GetTotalPower() * 0.5f));
                target->ReceiveMagicalDamage(damage, caster);
            }
            });
        return s;
        });
#pragma endregion

#pragma region FATUUS
    Register("tail_slap", [](int cost) {
        Skill s("Tail Slap", DamageType::Physical, 3, 0.35f, cost, "tail_slap");
        s.SetDescription("Deal 3(+35% Power) Physical Damage");
        s.AddEffect({
            "Deal 20(+20% Power) magic damage if the target is below 30% Health",
            [](Character* caster, Character* target) {
                int remainingHealth = target->GetMaxHP() - target->GetCurrentHP();
                if(remainingHealth <= (target->GetMaxHP() * 0.3f))
                {
                    int damage = (int)(20 + (caster->GetTotalPower() * 0.2));
                    caster->ReceiveMagicalDamage(damage, caster);
                }
                else {
#if _DEBUG
                    LOG("Doing TailSlap Fatuus: not enough remaining health to deal magic damage");
#endif // _DEBUG
                }
            }
            });
        return s;
        });

    Register("poison_orb", [](int cost) {
        Skill s("Poison Orb", DamageType::None, 0, 0.0f, cost, "poison_orb");
        s.SetDescription("");
        s.AddEffect({
            "Apply 6 Poison and 3 Fire and reduce the Durability of the target by 5(+15% Fire Power)",
            [](Character* caster, Character* target) {
                int damagePoisonPower = (int)(6 * (1 + caster->GetPoisonPower() / 100.0f));
                int damageFirePower = (int)(3 * (1 + caster->GetFirePower() / 100.0f));

                target->SetBurned(true, damageFirePower, caster);
                target->SetPoisoned(true, damagePoisonPower, caster);

                int reducedDurability = (int)(5 + (caster->GetFirePower() * 0.15f));
                target->ModifyDurability(-reducedDurability);
            }
            });
        return s;
        });

    Register("explosion", [](int cost) {
        Skill s("Explosion", DamageType::Physical, 0, 0.0f, cost, "explosion");
        s.SetDescription("Deal 15(+25% Power) Physical Damage");
        s.AddEffect({
            "If the target is full live, Fatuus wins",
            [](Character* caster, Character* target) {

                int currentHpTarget = target->GetCurrentHP();
                int maxHptarget = target->GetMaxHP();
                if (currentHpTarget = maxHptarget) { caster->AddInitiative(30); }

                int baseDamage = 20;
                float multiplierPower = 0.35f;
                int totalDamage = baseDamage + (caster->GetTotalPower() * multiplierPower);
                target->ReceivePhysicalDamage(totalDamage, caster);
            }
            });
        return s;
        });

    Register("projectile", [](int cost) {
        Skill s("Projectile", DamageType::None, 0, 0.0f, cost, "projectile");
        s.SetDescription("");
        s.AddEffect({
            "Deal 5(+40% Power -50% of the Enemy's Durability) Physical Damage",
            [](Character* caster, Character* target) {
                int damage = (int)(5 + ((caster->GetTotalPower() * 0.4f) - (target->GetTotalDurability() * 0.5f)));
                target->ReceivePhysicalDamage(damage, caster);
            }
            });
        s.AddEffect({
            "Reduce the Healing Power by 5(+100% Poison Power)",
             [](Character* caster, Character* target) {
                int modifier = (int)(5 + (caster->GetPoisonPower() * 1.0f));
                target->ModifyHealingPower(modifier);
             }
             });
        return s;
        });

    Register("laser", [](int cost) {
        Skill s("Laser", DamageType::Physical, 3, 0.2f, cost, "laser");
        s.SetDescription("Deal 3(+20% Power) Physical Damage to all enemies");
        s.SetHasAreaEffect(true);
        s.SetAreaEffectTargetAllies(false);
        s.AddEffect({
            "Remove all their Posion and Fire and deal 3(+1% Power) magic damage for each point removed",
            [](Character* caster, Character* target) {
                int poisonPointsRemoved = (int)target->GetPoisonDamage();
                int burnPointsRemoved = (int)target->GetBurnDamage();
                target->ClearStatusEffects();
                int totalPointsRemoved = poisonPointsRemoved + burnPointsRemoved;
                int damage = (int)(totalPointsRemoved * (3 + (caster->GetTotalPower() * 0.01f)));
                target->ReceiveMagicalDamage(damage, caster);
            }
            });
        return s;
        });
#pragma endregion


#pragma endregion

#pragma region NEUTRALS

#pragma region RAPTOR

    // Slam: Deal 30(+10%Power) physical damage and heal 10(+20%Power)
    Register("raptor_slam", [](int cost) {
        Skill s("Slam", DamageType::Physical, 8, 0.5f, cost, "raptor_slam");
        s.AddEffect({
            "Deal 8(50%Power) physical damage and heal 5(+35%Power) health",
            [](Character* caster, Character* target) {
                caster->Heal(5 + (int)(caster->GetTotalPower() * 0.35f));
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
                caster->ModifyBonusDurability(20);
                caster->ModifyBonusPower(30);
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
                caster->ModifyBonusSpeed(25);
                caster->ModifyBonusPower(30);
            }
            });
        return s;
        });
#pragma endregion

#pragma region CHAMAN
    // Bonk Fire: Deal 25(+20%Power) Magic Damage and inflict 15 fire and 15 Poison
    Register("chaman_bonk_fire", [](int cost) {
        Skill s("Bonk Fire", DamageType::Magical, 8, 0.4f, cost, "chaman_bonk_fire");
        s.AddEffect({
            "Inflict 15 Fire and 15 Poison",
            [](Character* caster, Character* target) {
                int damageFirePower = (int)(4 * (1 + caster->GetFirePower() / 100.0f));
                int damagePoisonPower = (int)(4 * (1 + caster->GetPoisonPower() / 100.0f));

                target->SetBurned(true, damageFirePower, caster);
                target->SetPoisoned(true, damagePoisonPower, caster);
            }
            });
        return s;
        });

    Register("chaman_charge", [](int cost) {
        Skill s("Charge", DamageType::Magical, 0, 0.0f, cost, "chaman_charge");
        s.AddEffect({
            "Gain 50 FirePower and 50 PoisonPower and 20 power",
            [](Character* caster, Character* target) {
                caster->ModifyBonusPower(20);
                caster->ModifyPoisonPower(50.0f);
                caster->ModifyFirePower(50.0f);
            }
            });
        return s;
        });
#pragma endregion

#pragma region FIRE BRUTE
    Register("hit_fire", [](int cost) {
        Skill s("Hit Fire", DamageType::None, 0, 0.0f, cost, "hit_fire");
        s.AddEffect({
            "Deal 25(+150% Enemy's Fire + 20% Power) Physical Damage",
            [](Character* caster, Character* target) {
                int enemyFireDamage = target->GetBurnDamage();
                int damage = (int)(25 + ((enemyFireDamage * 1.5f) + (caster->GetTotalPower() * 0.2f)));
                target->ReceivePhysicalDamage(damage, caster);
            }
            });
        return s;
        });
#pragma endregion

#pragma region BF_SWORD MAN
    Register("slash_sword", [](int cost) {
        Skill s("Slash Sword", DamageType::None, 0, 0.0f, cost, "slash_sword");
        s.AddEffect({
            "Deal 15(+115% Power -50% of the Enemy's Durability) Physical Damage",
            [](Character* caster, Character* target) {
                int enemyDurability = target->GetTotalDurability();
                int damage = (int)(15 + ((caster->GetTotalPower() * 1.15f) - (enemyDurability * 0.5f)));
                target->ReceivePhysicalDamage(damage, caster);
            }
            });
        return s;
        });
#pragma endregion

#pragma region BIG BIRD
    Register("birds_flying", [](int cost) {
        Skill s("Birds Flying", DamageType::None, 0, 0.0f, cost, "birds_flying");
        s.AddEffect({
             "Grant yourself 10(+15% Power)% Speed, 10 Lifesteal and 10 Durability",
             [](Character* caster, Character* target) {
                caster->ModifyBaseSpeed((int)(15 + (caster->GetTotalPower() * 0.15f)));
                caster->ModifyLifesteal(10);
                caster->ModifyBonusDurability(10);
            }
            });
        return s;
        });


    Register("paper_bird_projectile", [](int cost) {
        Skill s("Paper Bird Projectile", DamageType::Physical, 5, 0.3f, cost, "paper_bird_projectile", MultiplierStat::SPEED);
        s.SetHasAreaEffect(true);
        s.SetAreaEffectTargetAllies(true);
        s.AddEffect({
            "Deal 5(+30% Speed) Physical Damage to the whole team",
            [](Character* caster, Character* target) {
            }
            });
        return s;
        });
#pragma endregion

#pragma region UBO
    Register("immolate", [](int cost) {
        Skill s("Immolate", DamageType::None, 0, 0.0f, cost, "immolate");
        s.AddEffect({
            "Deal yourself and an enemy 5(+60% Power) Physical Damage and reduce the Durability by 10(+20% Power)",
            [](Character* caster, Character* target) {
                caster->ReceivePhysicalDamage((int)(5 + (caster->GetTotalPower() * 0.6f)), caster);
                target->ReceivePhysicalDamage((int)(5 + (caster->GetTotalPower() * 0.6f)), caster);

                caster->ModifyBonusDurability((int)(10 + (caster->GetTotalPower() * 0.2f)));
                target->ModifyBonusDurability((int)(10 + (caster->GetTotalPower() * 0.2f)));
            }
            });
        return s;
        });
#pragma endregion

#pragma region CORAL

    Register("bubbles", [](int cost) {
        Skill s("Bubbles", DamageType::None, 0, 0.0f, cost, "bubbles");
        s.SetHasAreaEffect(true);
        s.SetAreaEffectTargetAllies(false);
        s.AddEffect({
            "Remove all status effects on your team (Fire and Poison) and Heal for 15(+Fire and Posion removed)",
            [](Character* caster, Character* target) {
                int fireDamage = target->GetBurnDamage();
                int poisonDamage = target->GetPoisonDamage();
                int totalMod = fireDamage + poisonDamage;

                target->ClearStatusEffects();
                target->Heal(15 + totalMod);
            }
            });
        return s;
        });
#pragma endregion

#pragma region MAXINE

    Register("heartbreak", [](int cost) {
        Skill s("Heartbreak", DamageType::Magical, 12, 0.15f, cost, "heartbreak");
        s.AddEffect({
            "Deal 12(+15% Power) Magic Damage and reduce the Power by 10%",
            [](Character* caster, Character* target) {
                target->ModifyBasePower((int)(target->GetTotalPower() * 0.10f));
            }
            });
        return s;
        });

#pragma endregion

#pragma region PEARL

    Register("water_surge", [](int cost) {
        Skill s("Water Surge", DamageType::Magical, 15, 1.0f, cost, "water_surge", MultiplierStat::DURABILITY);
        s.AddEffect({
            "Deal 15(+Enemy's Durability) Magic Damage and reduce the Speed by 20%",
            [](Character* caster, Character* target) {
                target->ModifyBaseSpeed((int)(target->GetTotalSpeed() * 0.20f));
            }
            });
        return s;
        });
#pragma endregion

#pragma endregion

#pragma region ENEMIES
#pragma region BUCK-T

    Register("buck_t_bite", [](int cost) {
        Skill s("Bite", DamageType::Physical, 5, 0.75f, cost, "buck_t_bite");
        return s;
        });


    Register("splash", [](int cost) {
        Skill s("Splash", DamageType::None, 0, 0.0f, cost, "splash");
        s.AddEffect({
            "Gain 10(+15% Power) Power, 10(+15% Speed) Speed and 20 Lifesteal",
            [](Character* caster, Character* target) {
                int currentTotalPower = caster->GetTotalPower();
                int modifierPower = (int)(10 + (0.15 * currentTotalPower));
                caster->ModifyBasePower(modifierPower);

                int currentTotalSpeed = caster->GetTotalSpeed();
                int modifierSpeed = (int)(10 + (0.15 * currentTotalSpeed));
                caster->ModifyBaseSpeed(modifierSpeed);

                caster->ModifyLifesteal(20);
            }
            });
        return s;
        });
#pragma endregion

#pragma region CHOMPERS
    Register("chompers_bite", [](int cost) {
        Skill s("Bite", DamageType::Physical, 5, 0.75f, cost, "chompers_bite");
        return s;
        });
#pragma endregion

#pragma region PECK AND BUBBLES
    Register("water_stream", [](int cost) {
        Skill s("Water Stream", DamageType::Physical, 1, 0.15f, cost, "water_stream");
        s.AddEffect({
            "Deal 15 Magic Damage",
            [](Character* caster, Character* target) {
                target->ReceiveMagicalDamage(15, caster);
            }
            });
        return s;
        });

#pragma endregion

#pragma region TOXIC JELLY

    Register("toxic_bubbles", [](int cost) {
        Skill s("Toxic Bubbles", DamageType::None, 0, 0.0f, cost, "toxic_bubbles");
        s.SetHasAreaEffect(true);
        s.SetAreaEffectTargetAllies(true);
        s.AddEffect({
            "Inflict 1 (+2% Power) Poison to all enemies, increase their total posion by 40(+40%Power)% and reduce the healing Power by 10",
            [](Character* caster, Character* target) {
                int poisonDamage = (int)(1 + (caster->GetTotalPower() * 0.02f));
                target->SetPoisoned(true, poisonDamage, caster);

                //increase poison by 40%
                int poisonDamageDebug = target->GetPoisonDamage();
                float poisonDamageIncrease = 0.4f + (caster->GetTotalPower() * 0.4f);
                int damage = (int)target->GetPoisonDamage() * poisonDamageIncrease;
                target->SetPoisoned(true, damage, caster);
#if _DEBUG
                LOG("ABILITY TOXIC BUBBLES BY  TOXIC JELLY| Poison stat mod of %s(target) before was: %d, and now is: %d, and Poison Debug %d", target->GetName().c_str(), poisonDamage, target->GetPoisonDamage(), poisonDamageDebug);
#endif // _DEBUG

                //reduce healing power
                target->ModifyHealingPower(-10);
            }
            });
        return s;
        });
#pragma endregion

#pragma region PINK JELLY

    Register("electric_shock", [](int cost) {
        Skill s("Electric Shock", DamageType::Magical, 3, 0.1f, cost, "electric_shock");
        s.SetDescription("Deal 3(+10%Power) magic damage");
        s.AddEffect({
            "Reduce initiative by 10(+10%Power)%",
            [](Character* caster, Character* target) {
                target->ReceiveMagicalDamage(15, caster);
                float initiativePercentage = 0.10f + ((caster->GetTotalPower() * 0.10f) / 100);
                target->AddInitiative(-1 * (target->GetCurrentInitiative() * initiativePercentage));
            }
            });
        return s;
        });

    Register("shield", [](int cost) {
        Skill s("Shield", DamageType::None, 0, 0.0f, cost, "shield");
        s.SetHasAreaEffect(true);
        s.SetAreaEffectTargetAllies(false);
        s.AddEffect({
            "Grant all allies 5(+50%Durability) Durability and heal them for 5(+10% Power) Health",
            [](Character* caster, Character* target) {
                int grantedDurability = (int)(5 + (caster->GetTotalDurability() * 0.5f));
                target->ModifyDurability(grantedDurability);

                int healing = (int)(5 + (caster->GetTotalPower() * 0.1f));
                target->Heal(healing);
            }
            });
        return s;
        });
#pragma endregion

#pragma region BLUE JELLY

    Register("waves", [](int cost) {
        Skill s("Waves", DamageType::Magical, 0, 0.0f, cost, "waves");
        s.SetHasAreaEffect(true);
        s.SetAreaEffectTargetAllies(false);
        s.AddEffect({
            "Grant your whole team 10(+25% Power) Power and 10 Speed",
            [](Character* caster, Character* target) {
                int powerGranted = (int)(10 + (target->GetTotalPower() * 0.25f));
                target->ModifyBasePower(powerGranted);
                target->ModifyBaseSpeed(10);
            }
            });
        return s;
        });

    Register("electric_ball", [](int cost) {
        Skill s("Electric Ball", DamageType::Magical, 3, 0.15f, cost, "electric_ball");
        s.SetHasAreaEffect(true);
        s.SetAreaEffectTargetAllies(false);
        s.SetDescription("Deal 3(+15% Power) magic damage");
        s.AddEffect({
            "Reduce the enemy's Power by 10(+10% Power)%",
            [](Character* caster, Character* target) {
                float reducedPower = 0.1f + (caster->GetTotalPower() * 0.1f);
                target->ModifyBasePower(-1 * (target->GetTotalPower() * reducedPower));
            }
            });
        return s;
        });

#pragma endregion

#pragma region GIRL BOSS

    Register("cast", [](int cost) {
        Skill s("Cast", DamageType::None, 0, 0.0f, cost, "cast");
        s.SetHasAreaEffect(true);
        s.SetAreaEffectTargetAllies(false);
        s.AddEffect({
            "Apply 10 Fire and deal 1 Magic Damage for every point of Fire to all enemies",
            [](Character* caster, Character* target) {
                target->SetBurned(true, 10, caster);

                int fireDamage = target->GetBurnDamage();
                target->ReceiveMagicalDamage(fireDamage, caster);
            }
            });
        return s;
        });

    //in-acabada
    Register("idles", [](int cost) {
        Skill s("Idles", DamageType::None, 0, 0.0f, cost, "idles");
        s.AddEffect({
            "Remove all stat and status changes",
            [](Character* caster, Character* target) {
                caster->SetPoisoned(false, 0, caster);
                caster->SetBurned(false, 0, caster);
            }
            });
        return s;
        });

    Register("strike", [](int cost) {
        Skill s("Strike", DamageType::Physical, 30, 0.1f, cost, "strike");
        s.AddEffect({
            "Deal 30(+10% Power) Physical Damage and 0(+20% Power) Magic Damage",
            [](Character* caster, Character* target) {
                int damage = (int)(0 + (caster->GetTotalPower() * 0.2f));
                target->ReceiveMagicalDamage(damage, caster);
            }
            });
        return s;
        });

    Register("power_up", [](int cost) {
        Skill s("Power Up", DamageType::None, 0, 0.0f, cost, "power_up");
        s.AddEffect({
            "Gain 50 Speed, 30 Power, 30 Fire Power",
            [](Character* caster, Character* target) {
                caster->ModifyBaseSpeed(50);
                caster->ModifyBasePower(30);
                caster->ModifyFirePower(30.0f);
            }
            });
        return s;
        });

#pragma endregion

#pragma region LIL GUY 1 

    Register("spin", [](int cost) {
        Skill s("Spin", DamageType::None, 0, 0.0f, cost, "spin");
        s.AddEffect({
            "Gain 10 Durability and deal 20(+100% Durability) Physical Damage",
            [](Character* caster, Character* target) {
                caster->ModifyDurability(10);

                int damage = 20 + caster->GetTotalDurability();
                target->ReceivePhysicalDamage(damage, caster);
            }
            });
        return s;
        });
#pragma endregion

#pragma region LIL GUY 2

    Register("fire_attack", [](int cost) {
        Skill s("Fire Attack", DamageType::Physical, 5, 1.0f, cost, "fire_attack");
        s.SetDescription("Deal 5(+100% Power) Physical Damage");
        s.AddEffect({
            "Deal 5(+100% Power) Physical Damage and inflict 10 Fire",
            [](Character* caster, Character* target) {
                target->SetBurned(true, 10, caster);
            }
            });
        return s;
        });

    Register("lil_guy_charge", [](int cost) {
        Skill s("Lil Guy Charge", DamageType::Physical, 5, 1.0f, cost, "lil_guy_charge");
        s.AddEffect({
            "Gain 25% Speed",
            [](Character* caster, Character* target) {
                caster->ModifyBasePower(caster->GetTotalSpeed() * 0.25f);
            }
            });
        return s;
        });
#pragma endregion

#pragma endregion
}
