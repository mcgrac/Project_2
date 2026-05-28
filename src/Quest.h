#pragma once
#include <string>

enum class QuestConditionType
{
    REACH_LEVEL,
    VISIT_FACTION_ISLAND,
    BUY_FACTION_ITEM,
    DAMAGE_IN_COMBAT,
    TOTAL_DAMAGE,
    STAT_THRESHOLD
};

enum class QuestStatus
{
    ACTIVE,
    COMPLETED,
    LOCKED
};

struct QuestCondition
{
    QuestConditionType type;
    int amount      = 0;
    int targetLevel = 0;
    std::string faction;
    std::string itemName;
    std::string stat;
};

struct Quest
{
    int id          = 0;
    std::string name;
    std::string description;
    int rewardGold  = 0;
    QuestCondition condition;
    QuestStatus status   = QuestStatus::ACTIVE;
    int progress    = 0;   // usado por DAMAGE_IN_COMBAT y TOTAL_DAMAGE
};
