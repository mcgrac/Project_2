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
	// Arranca en el menú principal
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
	scene->Load();
	sceneStack.push(scene);
}

void Scene::PopScene(){
	if (sceneStack.empty()) 
	{
		return;
	}

	sceneStack.top()->Unload();
	delete sceneStack.top();
	sceneStack.pop();
}

void Scene::ReplaceScene(BaseScene* scene){
	ClearStack();
	PushScene(scene);
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