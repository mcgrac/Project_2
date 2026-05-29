#pragma once
#include "Quest.h"
#include <vector>
#include <string>
#include "pugixml.hpp"

class Party;
class Character;

class QuestManager
{
public:
    static QuestManager& GetInstance();

    // Inicializacion: llamar una vez al arrancar el juego
    void Init(Party* _party);

    // Carga las quests desde XML.
    // Llamar tras Init() en un juego nuevo, o tras LoadProgress() al cargar partida.
    void LoadQuestsFromXML(const std::string& path);

    // Persistencia: guarda/carga solo status y progress en el XML de guardado
    void SaveProgress(pugi::xml_node& rootNode);
    void LoadProgress(const std::string& savePath);

    // Eventos
    void OnCharacterLevelUp(Character* character);
    void OnIslandVisited(const std::string& faction);
    void OnItemPurchased(const std::string& itemName, const std::string& faction);
    void OnCombatDamageDealt(int amount);
    void OnCombatEnd();
    void OnStatChanged(Character* character);

    // Desbloquea una quest LOCKED (para uso futuro de NPCs)
    void UnlockQuest(int questId);

    const std::vector<Quest>& GetQuests() const;

private:
    QuestManager()  = default;
    ~QuestManager() = default;
    QuestManager(const QuestManager&) = delete;
    QuestManager& operator=(const QuestManager&) = delete;

    void CheckAndComplete(Quest& quest);
    void CompleteQuest(Quest& quest);

    std::vector<Quest> _quests;
    Party* party = nullptr;
    int combatDamage = 0;   // acumulado del combate actual
};
