#include "SaveLoad.h"
#include "Party.h"
#include "Character.h"
#include "pugixml.hpp"
#include "Log.h"
#include <fstream>


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
    root.append_child("party");

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

    for (Character* c : party->GetMembers())
    {
        pugi::xml_node charNode = partyNode.append_child("character");
        charNode.append_attribute("name").set_value(c->GetName().c_str());
        charNode.append_attribute("health").set_value(c->GetCurrentHP());
        charNode.append_attribute("isAlive").set_value(c->GetIsAlive());
    }

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

    // Characters
    for (pugi::xml_node charNode : root.child("party").children("character"))
    {
        SaveData::CharacterSave charSave;
        charSave.name    = charNode.attribute("name").as_string();
        charSave.health  = charNode.attribute("health").as_int();
        charSave.isAlive = charNode.attribute("isAlive").as_bool();

        data.characters.push_back(charSave);
    }

    data.exists = true;

    LOG("SaveLoad: partida cargada — isla %d, %d personajes.",
        data.currentIslandId, (int)data.characters.size());

    return data;
}
