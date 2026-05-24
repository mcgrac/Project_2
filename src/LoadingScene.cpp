#include "LoadingScene.h"
#include "InGameScene.h"
#include "CharacterFactory.h"
#include "Scene.h"
#include "Engine.h"
#include "Render.h"
#include "Textures.h"
#include "Log.h"
#include "DialogueManager.h"
#include "ItemManager.h"

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
        loadedSaveData = SaveLoad::Load();
        if (loadedSaveData.exists)
        {
            for (const auto& charSave : loadedSaveData.characters)
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
            c->SetIsAllied(true);
            loadedCharacters.push_back(c);
        }
        else
        {
            LOG("LoadingScene: no se pudo crear '%s'.", name.c_str());
        }
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

    // Restaurar estado del save si es Continue
    if (isContinue && loadedSaveData.exists)
    {
        ApplySaveDataToCharacters();
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

void LoadingScene::ApplySaveDataToCharacters()
{
    for (Character* c : loadedCharacters)
    {
        // Buscar el save de este personaje
        const SaveData::CharacterSave* charSave = nullptr;
        for (const auto& cs : loadedSaveData.characters)
        {
            if (cs.name == c->GetName())
            {
                charSave = &cs;
                break;
            }
        }

        if (charSave == nullptr) { continue; }

        // ----- Nivel -----
        // CreateDataOnly siempre crea en nivel 1, aplicar los level-ups acumulados
        int levelsToGain = charSave->level - c->GetLevel();
        for (int i = 0; i < levelsToGain; i++)
        {
            c->LevelUp();
        }

        // ----- XP -----
        c->AddXP(charSave->experience);

        // ----- Upgrades -----
        std::vector<UpgradeTier>& tiers = c->GetUpgradeTree()->GetTiers();
        for (int i = 0; i < (int)charSave->chosenUpgrades.size() && i < (int)tiers.size(); i++)
        {
            const std::string& chosenName = charSave->chosenUpgrades[i];
            if (chosenName.empty()) { continue; }

            // Determinar si es opción A (0) o B (1)
            int choice = -1;
            if (tiers[i].GetOptionA().name == chosenName)
            {
                choice = 0;
            }
            else if (tiers[i].GetOptionB().name == chosenName)
            {
                choice = 1;
            }

            if (choice != -1)
            {
                tiers[i].ChooseUpgrade(choice, *c);
                LOG("LoadingScene: %s — tier %d upgrade '%s' aplicado.",
                    c->GetName().c_str(), i, chosenName.c_str());
            }
            else
            {
                LOG("LoadingScene: %s — tier %d upgrade '%s' no encontrado.",
                    c->GetName().c_str(), i, chosenName.c_str());
            }
        }

        // ----- Items equipados -----
        for (const std::string& itemName : charSave->equippedItems)
        {
            Item* found = Engine::GetInstance().itemManager->GetItemByName(itemName);
            if (found == nullptr)
            {
                LOG("LoadingScene: item '%s' no encontrado en ItemManager.", itemName.c_str());
                continue;
            }

            EquippableItem* equippable = dynamic_cast<EquippableItem*>(found);
            if (equippable == nullptr)
            {
                LOG("LoadingScene: item '%s' no es equippable.", itemName.c_str());
                continue;
            }

            // Registrar en el inventario y aplicar el efecto al personaje
            // El inventory vive en la party, pero aquí aún no tenemos la party creada.
            // Aplicamos solo el efecto estadístico — el EquipItem al inventory
            // se hará en InGameScene al recibir los personajes
            equippable->Use(c);

            LOG("LoadingScene: %s — item '%s' aplicado.",
                c->GetName().c_str(), itemName.c_str());
        }

        // ----- HP -----
        // Se restaura al final, después de que los level-ups e items hayan
        // ajustado el maxHealth, para no recortar HP innecesariamente
        int hpDiff = charSave->health - c->GetCurrentHP();
        c->ModifyCurrentHealth(hpDiff);

        // ----- isAlive -----
        // Si murió y no ha revivido, el HP ya será 0 por el save;
        // LevelUp y FullyHeal no se habrán llamado así que isAlive
        // se gestiona solo via pendingToDie/health en combate.
        // Si quieres forzarlo:
        // if (!charSave->isAlive) { /* marcar muerto */ }

        LOG("LoadingScene: %s restaurado — nivel %d, HP %d/%d.",
            c->GetName().c_str(), c->GetLevel(), c->GetCurrentHP(), c->GetMaxHP());
    }

    // ----- Oro y consumibles de la party -----
    // La party se crea en InGameScene, así que pasamos los datos via loadedSaveData
    // que InGameScene ya recibe como isContinue=true y puede leer de nuevo,
}