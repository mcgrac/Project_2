#include "Engine.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Log.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Player.h"
#include "Map.h"
#include "Item.h"
#include "UIManager.h"
#include "MainMenuScene.h"
#include "SplashScene.h"

Scene::Scene() : Module()
{
	name = "scene";
}

// Destructor
Scene::~Scene(){
	ClearStack();
}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");

	bool ret = true;

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	PushScene(new SplashScene());
	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	//reset input flag
	if (ignoreInputThisFrame)
	{
		ignoreInputThisFrame = false;
	}

	if (!sceneStack.empty())
	{
		sceneStack.top()->Update(dt);
	}

	return true;
}

bool Scene::PostUpdate()
{
	bool ret = true;

	if(Engine::GetInstance().input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		ret = false;
	}

	if (!sceneStack.empty())
	{
		sceneStack.top()->PostUpdate(0.f);
	}

	return ret;
}

bool Scene::OnUIMouseClickEvent(UIElement* uiElement)
{
	if (ignoreInputThisFrame) {return false;}

	if (!sceneStack.empty())
	{
		return sceneStack.top()->OnUIMouseClickEvent(uiElement);
	}

	return true;
}

bool Scene::CleanUp()
{

	LOG("Freeing scene");
	ClearStack();
	return true;
}

#pragma region SCENE MANAGER
void Scene::PushScene(BaseScene* scene){

	if (!sceneStack.empty())
	{
		sceneStack.top()->OnPause(); // pause current scene
	}

	scene->Load();
	sceneStack.push(scene);

	ignoreInputThisFrame = true;

	PrintStack();
}

void Scene::PopScene(){
	if (sceneStack.empty()) 
	{
		return;
	}

	sceneStack.top()->Unload();
	delete sceneStack.top();
	sceneStack.pop();

	if (!sceneStack.empty())
	{
		sceneStack.top()->OnResume(); // Unpause scene
	}

	ignoreInputThisFrame = true; // bloquear input este frame

	PrintStack();
}

void Scene::ReplaceScene(BaseScene* scene){
	ClearStack();
	scene->Load();
	sceneStack.push(scene);
	PrintStack();
}

void Scene::PrintStack()
{
	std::stack<BaseScene*> temp = sceneStack; // copy stack
	std::vector<BaseScene*> ordered;

	while (!temp.empty())
	{
		ordered.push_back(temp.top());
		temp.pop();
	}

	std::cout << "\n==== SCENE STACK ====\n";

	// Print bottom → top
	for (int i = ordered.size() - 1; i >= 0; --i)
	{
		std::cout << "[" << (ordered.size() - 1 - i) << "] "
			<< ordered[i]->sceneName;

		if (i == 0)
		{
			std::cout << "  <-- ACTIVE";
		}

		std::cout << "\n";
	}

	std::cout << "=====================\n";
}

void Scene::ClearStack() {
	while (!sceneStack.empty())
	{
		sceneStack.top()->Unload();
		delete sceneStack.top();
		sceneStack.pop();
	}
}
#pragma endregion