#include "SceneUtils.h"

std::string SceneUtils::GetFactionString(IslandFaction faction)
{
	switch (faction)
	{
	case IslandFaction::HUMANS:
		return "human";
	case IslandFaction::BIRD:
		return "bird";
	case IslandFaction::SIRENS:
		return "siren";
	case IslandFaction::REPTILES:
		return "reptile";
	default:
		return "human";
	}
}

std::string SceneUtils::GetBuildingTexturePath(IslandFaction faction, const std::string& building)
{
	return "Assets/Textures/" + building + "/" + SceneUtils::GetFactionString(faction) + ".png";
}

std::string SceneUtils::GetIslandTexturePath(IslandFaction faction)
{
	return "Assets/Textures/IslandInteriorBackground/" + SceneUtils::GetFactionString(faction) + ".png";
}
