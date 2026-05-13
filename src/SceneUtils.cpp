#include "SceneUtils.h"
#include "Engine.h"
#include "Textures.h"
#include "Render.h"


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

#pragma region GOLD COUNTER
GoldCounter::GoldCounter(int _amount, std::string _path, int _positionX, int _positionY) : amount(_amount)
{
	position.setX(_positionX);
	position.setY(_positionY);

	SetTextureCoin(_path);
}

void GoldCounter::Update(int currentGoldAmount) {

	//update amount of the counter if the party's gold amount change
	int currentAmount = amount;
	if (currentAmount != currentGoldAmount) {
		UpdateAmount(currentGoldAmount);
	}

	RenderCounter();
}

void GoldCounter::RenderCounter()
{
	//render texture
	Engine::GetInstance().render->DrawTexture(coinIcon, position.getX(), position.getY());

	//render text
	SDL_Color col = { 255,255,255,255 };
	std::string textAmount = std::to_string(amount);
	Engine::GetInstance().render->DrawText(textAmount.c_str(), position.getX() + 50, position.getY(), 30, 30, col);
}
void GoldCounter::UpdateAmount(int i)
{
	//update ampunt
	amount = i;
}
void GoldCounter::SetTextureCoin(std::string path)
{
	Engine::GetInstance().textures->Load(path.c_str());
}
#pragma endregion


