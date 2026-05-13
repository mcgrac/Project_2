#include "SceneUtils.h"
#include "Engine.h"
#include "Textures.h"
#include "Render.h"
#include "Log.h"
#include <cstdio>
#include <direct.h>

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
	SetTextureCoin(_path);
	LoadAnimation();

	position.setX(_positionX);
	position.setY(_positionY);
}


void GoldCounter::Update(int currentGoldAmount, float dt) {

	LOG("UPDATE GOLD COUNTER");

	UpdateAmount(currentGoldAmount);
	Draw(dt);
}

void GoldCounter::Draw(float dt)
{
	anims.Update(dt);

	const SDL_Rect& animFrame = anims.GetCurrentFrame();

	int drawX = (int)position.getX() - animFrame.w / 2;
	int drawY = (int)position.getY() - animFrame.h / 2;

	LOG("FRAME: ", animFrame.x, animFrame.y, animFrame.w, animFrame.h);

	Engine::GetInstance().render->DrawTexture(
		coinIcon,
		drawX,
		drawY,
		&animFrame,
		0.0f
	);

	RenderCounter(drawX + animFrame.w + 5, drawY);
}

void GoldCounter::LoadAnimation()
{

	// load
	std::unordered_map<int, std::string> aliases = { {0,"idle"} };
	bool loaded = anims.LoadFromTSX("Assets/Textures/Animations/coin.tsx", aliases);
	LOG("ANIMS LOADED, success: ", loaded);
	LOG("Has idle: ", anims.Has("idle"));
	anims.SetCurrent("idle");
	const SDL_Rect& testFrame = anims.GetCurrentFrame();
	LOG("FRAME AFTER LOAD: ", testFrame.x, testFrame.y, testFrame.w, testFrame.h);
}

void GoldCounter::RenderCounter(int x, int y)
{
	//render text
	SDL_Color col = { 255,255,255,255 };
	std::string textAmount = std::to_string(amount);
	Engine::GetInstance().render->DrawText(textAmount.c_str(), x, y, 30, 30, col);
}
void GoldCounter::UpdateAmount(int i)
{
	//update ampunt
	amount = i;
}
void GoldCounter::SetTextureCoin(std::string path)
{
	coinIcon = Engine::GetInstance().textures->Load(path.c_str());
}
#pragma endregion


