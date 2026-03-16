#pragma once

#include "Module.h"
#include "Player.h"
#include "UIButton.h"
#include <stack>
#include "BaseScene.h"

struct SDL_Texture;

//L17 TODO 1: Define SceneID enum class with INTRO_SCREEN, MAIN_MENU, LEVEL1, LEVEL2
enum class SceneID
{
	INTRO_SCREEN,
	MAIN_MENU,
	LEVEL1,
	LEVEL2
};


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

	//Return the player position
	Vector2D GetPlayerPosition();

	//Get tilePosDebug value
	std::string GetTilePosDebug() {
		return tilePosDebug;
	}

	// Handles multiple Gui Event methods
	bool OnUIMouseClickEvent(UIElement* uiElement);

	// L17 TODO 2: Define functions to handle scene changes
	void ChangeScene(SceneID newScene);
	void UnloadCurrentScene();
	void LoadScene(SceneID newScene);

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

	// L17 TODO 3: Define specific function for main menu scene: Load, Unload, Handle UI events
	void LoadMainMenu();
	void UnloadMainMenu();
	void UpdateMainMenu(float dt);
	void HandleMainMenuUIEvents(UIElement* uiElement);

	// L17 TODO 4: Define specific functions for level1 scene: Load, Unload, Update, PostUpdate
	void LoadLevel1();
	void UnloadLevel1();
	void UpdateLevel1(float dt);
	void PostUpdateLevel1();

	// L17 TODO 5: Define specific functions for level2 scene: Load, Unload, Update
	void LoadLevel2();
	void UpdateLevel2(float dt);
	void UnloadLevel2();


	//L03: TODO 3b: Declare a Player attribute
	std::shared_ptr<Player> player;
	SDL_Texture* mouseTileTex = nullptr;
	std::string tilePosDebug = "[0,0]";
	bool once = false;

	// L16: TODO 2: Declare a UIButton 
	std::shared_ptr<UIButton> uiBt;
	float volume = 1.0;

	// L17 TODO 1: Current scene attribute with initial value
	SceneID currentScene = SceneID::MAIN_MENU;

#pragma region SCENE MANAGER (STACK)
	//----STACK SCENE MANAGER----//
	std::stack<BaseScene*> sceneStack;

	//Destroy and unload all scenes
	void ClearStack();
#pragma endregion
};