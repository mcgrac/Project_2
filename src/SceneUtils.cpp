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

std::string SceneUtils::GetPortraitPath(const std::string& dialogueId)
{
	// npc_hostel_human -> Assets/Textures/Portraits/hostel/human.png
	// npc_dockyard_siren -> Assets/Textures/Portraits/dockyard/siren.png
	// formato esperado: npc_{location}_{faction}
	std::string stripped = dialogueId.substr(4); // quita "npc_"
	size_t underscore = stripped.find('_');
	if (underscore == std::string::npos)
	{
		return "";
	}
	std::string location = stripped.substr(0, underscore);
	std::string faction = stripped.substr(underscore + 1);
	return "Assets/Textures/PortraitsDialogues/" + location + "/" + faction + ".png";
}
