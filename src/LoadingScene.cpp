#include "LoadingScene.h"
#include "InGameScene.h"
#include "CharacterFactory.h"
#include "Scene.h"
#include "Engine.h"
#include "Render.h"
#include "Textures.h"
#include "Log.h"
#include "DialogueManager.h"

LoadingScene::LoadingScene(std::vector<std::string> _characterNames, bool _isContinue)
    : characterNames(_characterNames)
    , isContinue(_isContinue)
    , loadingDone(false)
    , background(nullptr)
    , dotTimer(0.0f)
    , dotCount(0)
    , framesRendered(0)
    , elapsedTime(0.0f)
{
    sceneName = "LoadingScene";
}

void LoadingScene::Load()
{
    LoadTextures();

}

void LoadingScene::DoBackgroundLoad()
{
    //characters allies
    std::vector<std::string> namesToLoad;
    if (isContinue)
    {
        SaveData data = SaveLoad::Load();
        if (data.exists)
        {
            for (const auto& charSave : data.characters)
            {
                namesToLoad.push_back(charSave.name);
            }
        }
    }
    else
    {
        namesToLoad = characterNames;
    }

    for (const std::string& name : namesToLoad)
    {
        LOG("LoadingScene: creando personaje '%s' en background.", name.c_str());
        Character* c = CharacterFactory::CreateDataOnly(name);
        if (c != nullptr)
        {
            loadedCharacters.push_back(c);
        }
        else
        {
            LOG("LoadingScene: no se pudo crear '%s'.", name.c_str());
        }

        c->SetIsAllied(true);
    }

    // WorldMap — solo datos, sin texturas
    loadedWorldMap = new WorldMap();
    loadedWorldMap->LoadWorldData("Assets/Maps/world.xml");

    //dialogues
    DialogueManager::LoadDialogues("dialogues.xml");

    loadingDone = true;
}

void LoadingScene::Update(float dt)
{
    //create thread in the first frame
    if (framesRendered == 0 && !loadThread.joinable())
    {
        loadThread = std::thread(&LoadingScene::DoBackgroundLoad, this);
    }

    //Engine::GetInstance().render->DrawTexture(background, 0, 0);
    Engine::GetInstance().render->DrawRectangle({ 0, 0, 1280, 720 }, 0, 0, 0, 255, true);

    // Mientras carga, mostrar feedback
    dotTimer += dt;
    elapsedTime += dt;

    if (dotTimer >= 200.0f)
    {
        dotTimer = 0.0f;
        dotCount = (dotCount + 1) % 4; // 0, 1, 2, 3 → vuelve a 0
    }

    // Construir el string de loading
    std::string loadingText = "Loading";
    for (int i = 0; i < dotCount; i++)
    {
        loadingText += ".";
    }

    Engine::GetInstance().render->DrawText(loadingText.c_str(), 400, 300, 200, 40, { 255, 255, 255, 255 });

    framesRendered++;

    if (!loadingDone || elapsedTime < 3000.0f)
    {
        return;
    }

    // Carga terminada — esperar a que el thread haya acabado limpiamente
    if (loadThread.joinable())
    {
        loadThread.join();
    }

    // Cargar texturas en el hilo principal
    //characters
    for (Character* c : loadedCharacters)
    {
        CharacterFactory::LoadVisualsFor(c, c->GetName());
    }

    //worldMap
    SDL_Texture * humanTex = Engine::GetInstance().textures->Load("Assets/Textures/Islands/island_human.png");
    SDL_Texture* reptileTex = Engine::GetInstance().textures->Load("Assets/Textures/Islands/island_reptile.png");
    SDL_Texture* skullTex = Engine::GetInstance().textures->Load("Assets/Textures/MainMap/EnemySymbol.png");
    loadedWorldMap->ConnectVisuals(humanTex, reptileTex, skullTex);

    LOG("LoadingScene: carga completada, lanzando InGameScene.");
    Engine::GetInstance().scene->ReplaceScene(new InGameScene(loadedCharacters, loadedWorldMap, isContinue));
}

void LoadingScene::PostUpdate(float dt)
{
}

void LoadingScene::Unload()
{
    // Seguridad: si por alguna razón se hace Unload antes de que el thread termine
    if (loadThread.joinable())
    {
        loadThread.join();
    }

    UnloadTextures();
}

void LoadingScene::LoadTextures()
{
    //background = Engine::GetInstance().textures->Load("Assets/Textures/LoadingScene/LoadingBackground.png");
}

void LoadingScene::UnloadTextures()
{
    //Engine::GetInstance().textures->UnLoad(background);
}

bool LoadingScene::OnUIMouseClickEvent(UIElement* uiElement)
{
    return true;
}