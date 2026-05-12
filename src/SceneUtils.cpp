#include "SceneUtils.h"

std::string SceneUtils::GetFactionString(IslandFaction faction)
{
	switch (faction)
	{
	case IslandFaction::HUMANS:
		return "human";
		break;
	case IslandFaction::BIRD:
		return "bird";
		break;
	case IslandFaction::SIRENS:
		return "siren";
		break;
	case IslandFaction::REPTILES:
		return "reptile";
		break;
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
