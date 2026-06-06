#include "SaveLoad.h"
#include "Party.h"
#include "Character.h"
#include "pugixml.hpp"
#include "Log.h"
#include <fstream>
#include "Item.h"
#include "EquippableItem.h"
#include "QuestManager.h"

bool SaveLoad::HasSaveFile()
{

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(SAVE_PATH);

    if (!result) return false;

    pugi::xml_node root = doc.child("savedata");
    if (!root) return false;

    // check if there is at leat one character saved
    pugi::xml_node party = root.child("party");
    if (!party) return false;

    return party.child("character") != nullptr;
}

void SaveLoad::ClearSave()
{
    pugi::xml_document doc;
    pugi::xml_node root = doc.append_child("savedata");
    root.append_child("world").append_attribute("currentIslandId").set_value(0);
    pugi::xml_node partyNode = root.append_child("party");
    partyNode.append_attribute("gold").set_value(0);
    partyNode.append_attribute("consumables").set_value(0);

    bool cleared = doc.save_file(SAVE_PATH);
    if (cleared)
    {
        LOG("SaveLoad: archivo de guardado limpiado correctamente.");
    }
    else
    {
        LOG("SaveLoad: ERROR al limpiar el archivo de guardado '%s'.", SAVE_PATH);
    }
}

//Save
void SaveLoad::Save(Party* party, int currentIslandId)
{
    if (party == nullptr)
    {
        LOG("SaveLoad::Save — party es nullptr, no se guarda.");
        return;
    }

    pugi::xml_document doc;
    pugi::xml_node root = doc.append_child("savedata");

    pugi::xml_node worldNode = root.append_child("world");
    worldNode.append_attribute("currentIslandId").set_value(currentIslandId);

    //each character's state
    pugi::xml_node partyNode = root.append_child("party");
    partyNode.append_attribute("gold").set_value(party->GetGold());
    partyNode.append_attribute("consumables").set_value(party->GetInventory().GetItemCount("consumable"));

    for (Character* c : party->GetMembers())
    {
        pugi::xml_node charNode = partyNode.append_child("character");
        charNode.append_attribute("name").set_value(c->GetName().c_str());
        charNode.append_attribute("health").set_value(c->GetCurrentHP());
        charNode.append_attribute("isAlive").set_value(c->GetIsAlive());
        charNode.append_attribute("experience").set_value(c->GetExperience());
        charNode.append_attribute("level").set_value(c->GetLevel());

        //Upgrades
        pugi::xml_node upgradesNode = charNode.append_child("upgrades");
        const std::vector<UpgradeTier>& tiers = c->GetUpgradeTree()->GetTiers();
        for (int i = 0; i < (int)tiers.size(); i++)
        {
            const Upgrade* chosen = tiers[i].GetChosen();
            pugi::xml_node tierNode = upgradesNode.append_child("tier");
            tierNode.append_attribute("index").set_value(i);
            if (chosen != nullptr)
            {
                tierNode.append_attribute("chosen").set_value(chosen->name.c_str());
            }
            else
            {
                tierNode.append_attribute("chosen").set_value("");
            }
        }

        // Items
        pugi::xml_node itemsNode = charNode.append_child("equippedItems");
        std::vector<EquippableItem*>& equipped = party->GetInventory().GetEquipped(c->GetName());
        for (EquippableItem* item : equipped)
        {
            pugi::xml_node itemNode = itemsNode.append_child("item");
            itemNode.append_attribute("name").set_value(item->GetName().c_str());
        }
    }

    //save damage done
    QuestManager::GetInstance().SaveProgress(root);

    // save file
    bool saved = doc.save_file(SAVE_PATH);
    if (saved)
    {
        LOG("SaveLoad: partida guardada — isla %d, %d personajes.",
            currentIslandId, party->GetMemberCount());
    }
    else
    {
        LOG("SaveLoad: ERROR al guardar en '%s'.", SAVE_PATH);
    }
}

//Load
SaveData SaveLoad::Load()
{
    SaveData data;
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(SAVE_PATH);

    if (!result)
    {
        LOG("SaveLoad: no se pudo cargar '%s' — %s", SAVE_PATH, result.description());
        data.exists = false;
        return data;
    }

    pugi::xml_node root = doc.child("savedata");
    data.currentIslandId = root.child("world").attribute("currentIslandId").as_int();

    pugi::xml_node partyNode = root.child("party");
    data.partyGold = partyNode.attribute("gold").as_int();
    data.consumables = partyNode.attribute("consumables").as_int();


    // Characters
    for (pugi::xml_node charNode : root.child("party").children("character"))
    {
        SaveData::CharacterSave charSave;
        charSave.name = charNode.attribute("name").as_string();
        charSave.health = charNode.attribute("health").as_int();
        charSave.isAlive = charNode.attribute("isAlive").as_bool();
        charSave.experience = charNode.attribute("experience").as_int();
        charSave.level = charNode.attribute("level").as_int();

        // Upgrades
        for (pugi::xml_node tierNode : charNode.child("upgrades").children("tier"))
        {
            std::string chosen = tierNode.attribute("chosen").as_string();
            charSave.chosenUpgrades.push_back(chosen);
        }

        // Items equipados
        for (pugi::xml_node itemNode : charNode.child("equippedItems").children("item"))
        {
            std::string itemName = itemNode.attribute("name").as_string();
            charSave.equippedItems.push_back(itemName);
        }

        data.characters.push_back(charSave);
    }

    data.exists = true;

    LOG("SaveLoad: partida cargada — isla %d, %d personajes.",
        data.currentIslandId, (int)data.characters.size());

    return data;
}
