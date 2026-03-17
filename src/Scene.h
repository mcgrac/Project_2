#pragma once

#include "Module.h"
#include "Player.h"
#include "UIButton.h"
#include <stack>
#include "BaseScene.h"

struct SDL_Texture;

class Scene : public Module
{
public:

	Scene();
	virtual ~Scene();

	bool Awake() override;
	bool Start() override;
	bool PreUpdate() override;
	bool Update(float dt) override;
	bool PostUpdate() override;
	bool CleanUp() override;


	//Get tilePosDebug value
	std::string GetTilePosDebug() {
		return tilePosDebug;
	}

	// Delega el evento UI al top de la pila
	bool OnUIMouseClickEvent(UIElement* uiElement) override;

#pragma region SCENE MANAGER
	// Push that scene to the top of the stack. The other one remains frozen
	// Use it for PauseScene (Pause will be loaded but the combar or ingame scene will remain visible at the back and frozen)
	void PushScene(BaseScene* scene);

	// Eliminates the scene at the top of the stack(calls unload of the scene)
	// The previous scene becomes the active one
	void PopScene();

	// Empy all the stack and loads a single scene as the only one
	// Use it for changing scenes totally (e.g.: InGame -> MainMenu)
	void ReplaceScene(BaseScene* scene);
#pragma endregion

private:

	SDL_Texture* mouseTileTex = nullptr;
	std::string tilePosDebug = "[0,0]";
	bool once = false;

#pragma region SCENE MANAGER (STACK)
	//----STACK SCENE MANAGER----//
	std::stack<BaseScene*> sceneStack;

	//Destroy and unload all scenes
	void ClearStack();
#pragma endregion
};