#pragma once
#include <string>
#include "Island.h"

class SceneUtils
{
public:
	static std::string GetFactionString(IslandFaction faction);
	static std::string GetBuildingTexturePath(IslandFaction faction, const std::string& building);
	static std::string GetIslandTexturePath(IslandFaction faction);
	static std::string GetPortraitPath(const std::string& dialogueId);
};
