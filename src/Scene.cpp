#include "Engine.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Log.h"
#include "Entity.h"
#include "Player.h"
#include "Map.h"
#include "Item.h"
#include "UIManager.h"
#include "MainMenuScene.h"
#include "SplashScene.h"
#include "QuestManager.h"

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

	// fade black logic
	if (isTransitioning)
	{
		if (!Engine::GetInstance().render->IsFading())
		{
			// 1. eliminar escena actual
			if (!sceneStack.empty())
			{
				sceneStack.top()->Unload();
				delete sceneStack.top();
				sceneStack.pop();
			}

			// 2. cargar nueva escena
			pendingScene->Load();
			sceneStack.push(pendingScene);

			pendingScene = nullptr;

			// 3. fade in
			Engine::GetInstance().render->StartFadeIn(1.5f);

			isTransitioning = false;
		}
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

	//Notifications quests
	QuestManager::GetInstance().UpdateNotification(Engine::GetInstance().GetDt()); // ~16ms en ms
	DrawQuestNotification();

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
	//// Iniciar fade out
	//Engine::GetInstance().render->StartFadeOut(fadeSpeed);

	//pendingScene = scene;
	//waitingFadeOut = true;

	//ignoreInputThisFrame = true;

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

void Scene::DrawQuestNotification()
{
	const QuestManager::QuestNotification* notif =
		QuestManager::GetInstance().GetActiveNotification();

	if (notif == nullptr)
	{
		return;
	}

	int screenW = 0;
	int screenH = 0;
	Engine::GetInstance().window->GetWindowSize(screenW, screenH);

	// Fade out en el ultimo tercio
	float progress = notif->timer / QuestManager::GetInstance().GetNotificationDuration();
	Uint8 alpha = 255;
	if (progress > 0.66f)
	{
		float fadeProgress = (progress - 0.66f) / 0.34f;
		alpha = (Uint8)(255.0f * (1.0f - fadeProgress));
	}

	const int CHAR_W = 8;
	const int LINE_H = 18;
	const int PADDING = 10;

	// 4 lineas: "Quest completada!" + nombre + descripcion + recompensa
	const int PANEL_H = PADDING + LINE_H * 4 + PADDING;
	const int PANEL_W = 340;
	const int PANEL_X = screenW / 2 - PANEL_W / 2;
	const int PANEL_Y = 24;

	SDL_Renderer* ren = Engine::GetInstance().render->renderer;
	SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);

	// Fondo
	SDL_SetRenderDrawColor(ren, 15, 15, 25, (Uint8)(200.0f * alpha / 255.0f));
	SDL_FRect bg = { (float)PANEL_X, (float)PANEL_Y, (float)PANEL_W, (float)PANEL_H };
	SDL_RenderFillRect(ren, &bg);

	// Borde dorado
	SDL_SetRenderDrawColor(ren, 255, 200, 50, alpha);
	SDL_RenderRect(ren, &bg);

	// Linea de acento superior
	SDL_FRect topLine = { (float)PANEL_X + 1, (float)PANEL_Y + 1, (float)PANEL_W - 2, 3.0f };
	SDL_RenderFillRect(ren, &topLine);

	SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_NONE);

	int textY = PANEL_Y + PADDING;

	// Linea 1: "Quest completada!" — dorado, centrado
	const char* header = "Quest completed!";
	int headerW = (int)strlen(header) * CHAR_W;
	Engine::GetInstance().render->DrawText(
		header,
		PANEL_X + PANEL_W / 2 - headerW / 2,
		textY, headerW, LINE_H,
		{ 255, 200, 50, alpha }
	);
	textY += LINE_H;

	// Linea 2: nombre de la quest — blanco, centrado
	int nameW = (int)notif->questName.size() * CHAR_W;
	Engine::GetInstance().render->DrawText(
		notif->questName.c_str(),
		PANEL_X + PANEL_W / 2 - nameW / 2,
		textY, nameW, LINE_H,
		{ 255, 255, 255, alpha }
	);
	textY += LINE_H;

	// Linea 3: descripcion — gris claro, centrado
	int descW = (int)notif->description.size() * CHAR_W;
	Engine::GetInstance().render->DrawText(
		notif->description.c_str(),
		PANEL_X + PANEL_W / 2 - descW / 2,
		textY, descW, LINE_H,
		{ 180, 180, 180, alpha }
	);
	textY += LINE_H;

	// Linea 4: recompensa — amarillo, centrado
	std::string rewardStr = "Reward: " + std::to_string(notif->rewardGold) + " gold";
	int rewardW = (int)rewardStr.size() * CHAR_W;
	Engine::GetInstance().render->DrawText(
		rewardStr.c_str(),
		PANEL_X + PANEL_W / 2 - rewardW / 2,
		textY, rewardW, LINE_H,
		{ 255, 200, 50, alpha }
	);
}

#pragma region FADE SYSTEM
void Scene::ChangeSceneWithFade(BaseScene* scene)
{
	if (isTransitioning) return;

	isTransitioning = true;
	pendingScene = scene;

	Engine::GetInstance().render->StartFadeOut(1.5f);
}
#pragma endregion
