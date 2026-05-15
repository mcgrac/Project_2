#include "WorldMap.h"
#include "Engine.h"
#include "Input.h"
#include "Textures.h"
#include "Render.h"
#include "LOG.h"
#include "pugixml.hpp"
#include <SDL3/SDL.h>

#include "Window.h" //temporal
#include <queue> // AÑADE ESTO para el BFS temporal

const std::vector<int> WorldMap::EMPTY = {};

WorldMap::WorldMap() : pendingIslandId(-1)
{}

bool WorldMap::LoadWorldData(const std::string& xmlPath)
{
    pugi::xml_document doc;
    if (!doc.load_file(xmlPath.c_str()))
    {
        LOG("WorldMap: failed to load %s", xmlPath.c_str());
        return false;
    }

    pugi::xml_node root = doc.child("world");

    int j = 0;

    for(int i = 0; i<7; i++){

        j++;
    
    }

    // 1. Load all islands
    for (pugi::xml_node node = root.child("island"); node; node = node.next_sibling("island"))
    {
        int id = node.attribute("id").as_int();
        std::string name = node.attribute("name").as_string();

        std::string faction = node.attribute("faction").as_string();
        IslandFaction islandFaction;
        if (faction == "human") {

            islandFaction = IslandFaction::HUMANS;
        }
        else if (faction == "reptile") {

            islandFaction = IslandFaction::REPTILES;
        }
        else if (faction == "bird") {

            islandFaction = IslandFaction::BIRD;
        }
        else if (faction == "siren") {

            islandFaction = IslandFaction::SIRENS;
        }
        else if (faction == "tribal") {

            islandFaction = IslandFaction::TRIBAL;
        }
        else if (faction == "jellyfish") {

            islandFaction = IslandFaction::JELLYFISH;
        }
        else if (faction == "fish") {

            islandFaction = IslandFaction::FISH;
        }
        else if (faction == "boss") {

            islandFaction = IslandFaction::BOSS;
        }
        else {

            islandFaction = IslandFaction::UNDEFINED;
        }

        std::string typeStr = node.attribute("type").as_string();
        IslandType type;
        if (typeStr == "hostile")
        {
            type = IslandType::HOSTILE;
        }
        else if (typeStr == "friendly")
        {
            type = IslandType::FRIENDLY;
        }
        else
        {
            type = IslandType::UNDEFINED;
        }

        islands[id] = new Island(id, name, type, islandFaction); //ISLAND CREATION
        tree[id] = {};

        LOG("WorldMap: loaded island — id=%d name='%s' type='%s' faction='%s'",
            id, name.c_str(), typeStr.c_str(), faction.c_str());
    }

    LOG("WorldMap: total loaded islands: %d", (int)islands.size());

    // 2. Load navigation tree
    for (pugi::xml_node node = root.child("node"); node; node = node.next_sibling("node"))
    {
        int parentId = node.attribute("islandId").as_int();

        for (pugi::xml_node next = node.child("next"); next; next = next.next_sibling("next"))
        {
            int childId = next.text().as_int();
            tree[parentId].push_back(childId);

            LOG("WorldMap: conection — parent island %d -> child island %d", parentId, childId);
        }
    }

    LOG("WorldMap: navigation tree loaded.");

    // 3. Initial island is the one with id = 0
    currentIslandId = 0;

    LOG("WorldMap: initial island established: id=%d name='%s'",
        currentIslandId, islands.at(currentIslandId)->GetName().c_str());

    doc.reset();

    return true;
}

void WorldMap::ConnectVisuals(SDL_Texture* humanTex, SDL_Texture* reptileTex, SDL_Texture* skullTex)
{
    for (auto& pair : islands)
    {
        Island* island = pair.second;

        if (island->GetIslandFaction() == IslandFaction::HUMANS)
        {
            island->SetSprite(humanTex);
        }
        else if (island->GetIslandFaction() == IslandFaction::REPTILES)
        {
            island->SetSprite(reptileTex);
        }

        island->SetSkullSprite(skullTex);
    }
}

Vector2D WorldMap::GetIslandShipPosition(int islandId) const
{
    auto it = islandShipPositions.find(islandId);
    if (it != islandShipPositions.end()) { return it->second; }
    return Vector2D(0, 0);
}

const std::vector<int>& WorldMap::GetNextIds(int islandId) const
{
    auto it = tree.find(islandId);
    if (it != tree.end())
    {
        return it->second;
    }
    return EMPTY;
}

bool WorldMap::IsReachable(int islandId) const
{
    const std::vector<int>& nexts = GetNextIds(currentIslandId);
    for (int id : nexts)
    {
        if (id == islandId)
        {
            return true;
        }
    }
    return false;
}

void WorldMap::ConfirmTravel()
{
    if (pendingIslandId == -1)
    {
        LOG("WorldMap::ConfirmTravel — no hay isla pendiente.");
        return;
    }

    currentIslandId = pendingIslandId;
    pendingIslandId = -1;

    LOG("WorldMap: viaje confirmado a isla id=%d (%s)", currentIslandId, islands.at(currentIslandId)->GetName().c_str());
}

void WorldMap::RenderWorld(float dt)
{
#pragma region FINAL
    // 1. Dibujar las conexiones (Líneas) PRIMERO para que queden debajo
    //for (auto const& pair : islands)
    //{
    //    Island* island = pair.second;
    //    const std::vector<int>& nextIds = GetNextIds(pair.first);

    //    for (int nextId : nextIds)
    //    {
    //        if (islands.find(nextId) != islands.end())
    //        {
    //            Island* nextIsland = islands.at(nextId);

    //            // Calculamos centros dinámicamente
    //            int startX = (int)(island->GetX() + (island->GetWidth() / 2.0f));
    //            int startY = (int)(island->GetY() + (island->GetHeight() / 2.0f));

    //            int endX = (int)(nextIsland->GetX() + (nextIsland->GetWidth() / 2.0f));
    //            int endY = (int)(nextIsland->GetY() + (nextIsland->GetHeight() / 2.0f));

    //            // Dibujamos la línea desde el centro del botón actual al siguiente
    //            Engine::GetInstance().render->DrawLine(startX, startY, endX, endY, 200, 200, 200, 255);
    //        }
    //    }
    //}

    //-------UNCOMENT for dynamic islands------------
    // 2. Dibujar sprites de islas (debajo del botón)
    //for (auto const& pair : islands)
    //{
    //    Island* island = pair.second;

    //    int x = (int)island->GetX();
    //    int y = (int)island->GetY();

    //    int offsetY = 40; // ajusta según tu arte

    //    if (island->GetSprite() != nullptr)
    //    {
    //        Engine::GetInstance().render->DrawTexture(
    //            island->GetSprite(),
    //            x,
    //            y + offsetY
    //        );
    //    }
    //}
    //-------------------------------------------------
    
    // 3. Dibujar icono de hostil (calavera)
    for (auto const& pair : islands)
    {
        Island* island = pair.second;

        if (island->GetType() == IslandType::HOSTILE)
        {
            int x = (int)island->GetX();
            int y = (int)island->GetY();

            int offsetX = -10;
            int offsetY = -10;

            if (island->GetSkullSprite() != nullptr)
            {
                Engine::GetInstance().render->DrawTexture(
                    island->GetSkullSprite(),
                    x + offsetX,
                    y + offsetY
                );
            }
        }
    }

    // 4. Dibujar un indicador de "Jugador Actual" (Opcional)
    // Ya que los botones ya se dibujan por el UIManager, aquí solo dibujamos 
    // efectos visuales extra, como un recuadro de selección.
    if (islands.count(currentIslandId))
    {
        Island* current = islands.at(currentIslandId);
        SDL_Rect highlight = {
            current->GetX() - 5,
            current->GetY() - 5,
            current->GetWidth() + 10,
            current->GetHeight() + 10
        };
        // Un recuadro amarillo brillante para saber dónde estamos
        Engine::GetInstance().render->DrawRectangle(highlight, 255, 255, 0, 100);
    }
#pragma endregion

#pragma region RENDER
    //// --- CONSTANTES TEMPORALES DE DISEÑO ---
    //// (Ajusta estos valores para ver cómo cambia el mapa)
    //static const int COL_SPACING = 200; // Distancia horizontal entre columnas
    //static const int ISLAND_W = 100;     // Ancho del rectángulo de la isla
    //static const int ISLAND_H = 60;      // Alto del rectángulo de la isla

    //// Obtenemos el tamaño de la ventana para centrar verticalmente
    //int screenW = 0;
    //int screenH = 0;
    //Engine::GetInstance().window->GetWindowSize(screenW, screenH);

    //// --- 1. Mapas para guardar las posiciones calculadas (solo para este frame) ---
    //std::unordered_map<int, SDL_Rect> islandPositions; // islandId -> Rectángulo visual

    //// Mapas auxiliares para el BFS (copia de InGameScene)
    //std::unordered_map<int, int> islandColumn; // islandId -> columna
    //std::unordered_map<int, int> islandRow;    // islandId -> fila en columna
    //std::unordered_map<int, int> colCount;     // column -> número total de islas

    //// --- 2. BFS para asignar columna y fila (Copia exacta de tu lógica) ---
    //std::queue<int> bfsQueue;

    //// Asumimos que la isla 0 es la raíz. Si no tienes isla 0, esto fallará.
    //if (islands.count(0)) {
    //    bfsQueue.push(0);
    //    islandColumn[0] = 0;
    //}
    //else {
    //    // Fallback: si no hay isla 0, coge la primera que encuentres
    //    if (!islands.empty()) {
    //        int firstId = islands.begin()->first;
    //        bfsQueue.push(firstId);
    //        islandColumn[firstId] = 0;
    //    }
    //    else {
    //        return; // No hay islas, nada que dibujar
    //    }
    //}

    //while (!bfsQueue.empty())
    //{
    //    int currentId = bfsQueue.front();
    //    bfsQueue.pop();

    //    int col = islandColumn[currentId];

    //    // Asignar fila y actualizar contador
    //    islandRow[currentId] = colCount[col];
    //    colCount[col]++;

    //    // Procesar hijos
    //    auto it = tree.find(currentId);
    //    if (it != tree.end())
    //    {
    //        for (int childId : it->second)
    //        {
    //            if (islandColumn.find(childId) == islandColumn.end())
    //            {
    //                islandColumn[childId] = col + 1;
    //                bfsQueue.push(childId);
    //            }
    //        }
    //    }
    //}

    //// --- 3. Calcular rectángulos finales ---
    //for (auto& pair : islands)
    //{
    //    int islandId = pair.first;

    //    // Si la isla no fue alcanzada por el BFS (está desconectada), no la dibujamos
    //    if (islandColumn.find(islandId) == islandColumn.end()) continue;

    //    int col = islandColumn[islandId];
    //    int row = islandRow[islandId];
    //    int islandsInCol = colCount[col];

    //    // Cálculo de posición (Copia exacta de tu lógica)
    //    int centerX = col * COL_SPACING + COL_SPACING / 2;
    //    int slotH = screenH / (islandsInCol + 1);
    //    int centerY = (row + 1) * slotH;

    //    int rectX = centerX - ISLAND_W / 2;
    //    int rectY = centerY - ISLAND_H / 2;

    //    islandPositions[islandId] = { rectX, rectY, ISLAND_W, ISLAND_H };
    //}

    //// --- 4. DIBUJAR ---

    //// A. Dibujar Líneas PRIMERO (usando el mapa temporal)
    //for (auto const& pair : islands)
    //{
    //    int islandId = pair.first;

    //    // Asegurarnos que tenemos posición para esta isla
    //    if (islandPositions.find(islandId) == islandPositions.end()) continue;

    //    const SDL_Rect& startRect = islandPositions.at(islandId);
    //    const std::vector<int>& nextIds = GetNextIds(islandId);

    //    for (int nextId : nextIds)
    //    {
    //        if (islandPositions.count(nextId))
    //        {
    //            const SDL_Rect& endRect = islandPositions.at(nextId);

    //            // Centro de inicio y fin
    //            int startX = startRect.x + startRect.w / 2;
    //            int startY = startRect.y + startRect.h / 2;
    //            int endX = endRect.x + endRect.w / 2;
    //            int endY = endRect.y + endRect.h / 2;

    //            // Línea gris claro
    //            Engine::GetInstance().render->DrawLine(startX, startY, endX, endY, 180, 180, 180, 255);
    //        }
    //    }
    //}

    //// B. Dibujar Rectángulos de Isla
    //for (auto& pair : islands)
    //{
    //    int islandId = pair.first;
    //    Island* island = pair.second;

    //    // Asegurarnos que tenemos posición para esta isla
    //    if (islandPositions.find(islandId) == islandPositions.end()) continue;

    //    const SDL_Rect& rect = islandPositions.at(islandId);

    //    // Color según tipo
    //    Uint8 r = 0, g = 0, b = 0;
    //    if (island->GetType() == IslandType::HOSTILE)
    //    {
    //        r = 220; g = 50; b = 50; // Rojo
    //    }
    //    else if (island->GetType() == IslandType::FRIENDLY)
    //    {
    //        r = 50; g = 220; b = 50; // Verde
    //    }
    //    else
    //    {
    //        r = 100; g = 100; b = 100; // Gris
    //    }

    //    // Rellenar el rectángulo
    //    Engine::GetInstance().render->DrawRectangle(rect, r, g, b, 255);

    //    // Dibujar un borde negro para que se vea mejor
    //    Engine::GetInstance().render->DrawRectangle({ rect.x, rect.y, rect.w, rect.h }, 0, 0, 0, 255, false); // false = solo borde (si DrawRectangle lo soporta)
    //    // Si DrawRectangle no soporta borde, simplemente dibuja el rectángulo y ya está.

    //    // C. Marcador para la isla actual (Amarillo)
    //    if (islandId == currentIslandId)
    //    {
    //        SDL_Rect marker = { rect.x - 5, rect.y - 5, rect.w + 10, rect.h + 10 };
    //        Engine::GetInstance().render->DrawRectangle(marker, 255, 255, 0, 150);
    //    }
    //}
#pragma endregion
}

void WorldMap::UnloadWorld()
{
    for (auto& pair : islands)
    {
        delete pair.second;
        pair.second = nullptr;
    }

    islands.clear();
    tree.clear();
    currentIslandId = -1;
}

void WorldMap::MakeAllIslandsHostile(IslandFaction faction)
{

    for (auto& pair : islands) {
        Island* island = pair.second;
        if (island->GetIslandFaction() == faction && island->GetType() == IslandType::FRIENDLY) {
            island->SetType(IslandType::HOSTILE);
            LOG("WorldMap: island '%s' is now Hostile", island->GetName().c_str());
        }
    }
}

void WorldMap::TravelTo(int islandId)
{
    if (!IsReachable(islandId))
    {
        LOG("WorldMap::TravelTo — island %d is not reachable from current island %d", islandId, currentIslandId);
        return;
    }

    //currentIslandId = islandId;
    pendingIslandId = islandId;

    LOG("WorldMap: navegando hacia isla id=%d (%s) — pendiente de confirmar",pendingIslandId, islands.at(pendingIslandId)->GetName().c_str());

    if (arrivalIsland)
    {
        //arrivalIsland(islands.at(currentIslandId));
        arrivalIsland(islands.at(pendingIslandId));
    }
}

Island* WorldMap::GetIslandById(int id) const
{
    auto it = islands.find(id);
    if (it != islands.end()) { return it->second; }
    return nullptr;
}

bool WorldMap::Update(float dt)
{
    return true;
}

bool WorldMap::PostUpdate(float dt)
{
    RenderWorld(dt);
    return true;
}