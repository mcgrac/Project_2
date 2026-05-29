#include "QuestManager.h"
#include "Quest.h"
#include "Party.h"
#include "Character.h"
#include <iostream>


QuestManager& QuestManager::GetInstance()
{
    static QuestManager instance;
    return instance;
}


void QuestManager::Init(Party* _party)
{
    party = _party;
    combatDamage = 0;
}


// XML loading
void QuestManager::LoadQuestsFromXML(const std::string& path)
{
    _quests.clear();

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(path.c_str());
    if (!result)
    {
        std::cerr << "[QuestManager] Could not load " << path << "\n";
        return;
    }

    pugi::xml_node root = doc.child("quests");
    for (pugi::xml_node node : root.children("quest"))
    {
        Quest quest;
        quest.id = node.attribute("id").as_int();
        quest.name = node.attribute("name").as_string();
        quest.rewardGold = node.attribute("reward_gold").as_int();
        quest.description = node.child_value("description");

        pugi::xml_node cond = node.child("condition");
        std::string condType = cond.attribute("type").as_string();

        if (condType == "REACH_LEVEL")
        {
            quest.condition.type = QuestConditionType::REACH_LEVEL;
            quest.condition.targetLevel = cond.attribute("target_level").as_int();
        }
        else if (condType == "VISIT_FACTION_ISLAND")
        {
            quest.condition.type = QuestConditionType::VISIT_FACTION_ISLAND;
            quest.condition.faction = cond.attribute("faction").as_string();
        }
        else if (condType == "BUY_FACTION_ITEM")
        {
            quest.condition.type = QuestConditionType::BUY_FACTION_ITEM;
            quest.condition.faction = cond.attribute("faction").as_string();
            quest.condition.itemName = cond.attribute("item_name").as_string();
        }
        else if (condType == "DAMAGE_IN_COMBAT")
        {
            quest.condition.type = QuestConditionType::DAMAGE_IN_COMBAT;
            quest.condition.amount = cond.attribute("amount").as_int();
        }
        else if (condType == "TOTAL_DAMAGE")
        {
            quest.condition.type = QuestConditionType::TOTAL_DAMAGE;
            quest.condition.amount = cond.attribute("amount").as_int();
        }
        else if (condType == "STAT_THRESHOLD")
        {
            quest.condition.type = QuestConditionType::STAT_THRESHOLD;
            quest.condition.stat = cond.attribute("stat").as_string();
            quest.condition.amount = cond.attribute("amount").as_int();
        }

        _quests.push_back(quest);
    }
}


// Persistencia

void QuestManager::SaveProgress(pugi::xml_node& rootNode)
{
    pugi::xml_node oldQuests = rootNode.child("quests");
    if (oldQuests)
    {
        rootNode.remove_child(oldQuests);
    }

    pugi::xml_node questsNode = rootNode.append_child("quests");

    for (const Quest& quest : _quests)
    {
        pugi::xml_node node = questsNode.append_child("quest");
        node.append_attribute("id") = quest.id;
        node.append_attribute("progress") = quest.progress;

        if (quest.status == QuestStatus::COMPLETED)
        {
            node.append_attribute("status") = "completed";
        }
        else if (quest.status == QuestStatus::LOCKED)
        {
            node.append_attribute("status") = "locked";
        }
        else
        {
            node.append_attribute("status") = "active";
        }
    }
}

void QuestManager::LoadProgress(const std::string& savePath)
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(savePath.c_str());
    if (!result)
    {
        return;
    }

    pugi::xml_node root = doc.child("savedata");
    pugi::xml_node questsNode = root.child("quests");
    if (!questsNode)
    {
        return;
    }

    for (pugi::xml_node node : questsNode.children("quest"))
    {
        int id = node.attribute("id").as_int();
        int progress = node.attribute("progress").as_int();
        std::string status = node.attribute("status").as_string();

        for (Quest& quest : _quests)
        {
            if (quest.id == id)
            {
                quest.progress = progress;

                if (status == "completed")
                {
                    quest.status = QuestStatus::COMPLETED;
                }
                else if (status == "locked")
                {
                    quest.status = QuestStatus::LOCKED;
                }
                else
                {
                    quest.status = QuestStatus::ACTIVE;
                }
                break;
            }
        }
    }
}

// Eventos
void QuestManager::OnCharacterLevelUp(Character* character)
{
    if (!character)
    {
        return;
    }

    for (Quest& quest : _quests)
    {
        if (quest.status != QuestStatus::ACTIVE)
        {
            continue;
        }

        if (quest.condition.type == QuestConditionType::REACH_LEVEL)
        {
            if (character->GetLevel() >= quest.condition.targetLevel)
            {
                CompleteQuest(quest);
            }
        }

        if (quest.condition.type == QuestConditionType::STAT_THRESHOLD)
        {
            CheckAndComplete(quest);
        }
    }
}

void QuestManager::OnIslandVisited(const std::string& faction)
{
    for (Quest& quest : _quests)
    {
        if (quest.status != QuestStatus::ACTIVE)
        {
            continue;
        }

        if (quest.condition.type == QuestConditionType::VISIT_FACTION_ISLAND)
        {
            if (quest.condition.faction == faction)
            {
                CompleteQuest(quest);
            }
        }
    }
}

void QuestManager::OnItemPurchased(const std::string& itemName, const std::string& faction)
{
    for (Quest& quest : _quests)
    {
        if (quest.status != QuestStatus::ACTIVE)
        {
            continue;
        }

        if (quest.condition.type == QuestConditionType::BUY_FACTION_ITEM)
        {
            if (quest.condition.faction == faction && quest.condition.itemName == itemName)
            {
                CompleteQuest(quest);
            }
        }
    }
}

void QuestManager::OnCombatDamageDealt(int amount)
{
    if (amount <= 0)
    {
        return;
    }

    combatDamage += amount;

    for (Quest& quest : _quests)
    {
        if (quest.status != QuestStatus::ACTIVE)
        {
            continue;
        }

        if (quest.condition.type == QuestConditionType::DAMAGE_IN_COMBAT)
        {
            if (combatDamage >= quest.condition.amount)
            {
                CompleteQuest(quest);
            }
        }

        if (quest.condition.type == QuestConditionType::TOTAL_DAMAGE)
        {
            quest.progress += amount;
            if (quest.progress >= quest.condition.amount)
            {
                CompleteQuest(quest);
            }
        }
    }
}

void QuestManager::OnCombatEnd()
{
    combatDamage = 0;
}

void QuestManager::OnStatChanged(Character* character)
{
    if (!character)
    {
        return;
    }

    for (Quest& quest : _quests)
    {
        if (quest.status != QuestStatus::ACTIVE)
        {
            continue;
        }

        if (quest.condition.type == QuestConditionType::STAT_THRESHOLD)
        {
            CheckAndComplete(quest);
        }
    }
}

void QuestManager::UnlockQuest(int questId)
{
    for (Quest& quest : _quests)
    {
        if (quest.id == questId && quest.status == QuestStatus::LOCKED)
        {
            quest.status = QuestStatus::ACTIVE;
            break;
        }
    }
}

const std::vector<Quest>& QuestManager::GetQuests() const
{
    return _quests;
}

// Complete quest

void QuestManager::CheckAndComplete(Quest& quest)
{
    if (quest.status != QuestStatus::ACTIVE)
    {
        return;
    }

    if (quest.condition.type != QuestConditionType::STAT_THRESHOLD)
    {
        return;
    }

    if (!party)
    {
        return;
    }

    // Comprueba si algun miembro de la party supera el umbral del stat
    for (int i = 0; i < party->GetMemberCount(); i++)
    {
        Character* member = party->GetMembers()[i];
        if (!member)
        {
            continue;
        }

        int statValue = 0;

        if (quest.condition.stat == "power")
        {
            statValue = member->GetBasePower();
        }
        else if (quest.condition.stat == "speed")
        {
            statValue = member->GetBaseSpeed();
        }
        else if (quest.condition.stat == "defense")
        {
            statValue = member->GetTotalDurability();
        }
        else if (quest.condition.stat == "hp")
        {
            statValue = member->GetMaxHP();
        }

        if (statValue >= quest.condition.amount)
        {
            CompleteQuest(quest);
            return;
        }
    }
}

void QuestManager::CompleteQuest(Quest& quest)
{
    quest.status = QuestStatus::COMPLETED;

    if (party)
    {
        party->AddGold(quest.rewardGold);
    }

    std::cout << "[QuestManager] Quest completed: " << quest.name
              << " | Reward: " << quest.rewardGold << " gold\n";
}
