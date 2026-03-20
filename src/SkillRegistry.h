#pragma once
#include "Skill.h"
#include <unordered_map>
#include <functional>
#include <string>

// SkillRegistry — mapa de id -> función que construye la Skill completa.
// Para añadir una skill nueva: registrarla en SkillRegistry.cpp con su id y sus efectos.
// El id debe coincidir exactamente con el atributo id del XML.
class SkillRegistry
{
public:
    static SkillRegistry& GetInstance();

    // Devuelve una Skill construida a partir de su id y el coste leído del XML.
    // Devuelve una Skill vacía si el id no está registrado.
    Skill Create(const std::string& id, int initiativeCost) const;

    // Registra una skill: id -> función constructora
    void Register(const std::string& id, std::function<Skill(int initiativeCost)> builder);

private:
    SkillRegistry();
    std::unordered_map<std::string, std::function<Skill(int initiativeCost)>> registry;
};
