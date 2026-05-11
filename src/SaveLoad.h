#pragma once
#include <string>
#include <vector>

class Party;

// Data
struct SaveData
{
    bool exists = false;    // false si no hay fichero de guardado
    int currentIslandId = 0;

    //specific characters saving
    struct CharacterSave
    {
        std::string name;
        int health;
        bool isAlive;
    };

    std::vector<CharacterSave> characters;
};

class SaveLoad
{
public:
    static constexpr const char* SAVE_PATH = "Assets/Save/SaveData.xml";

    // Guarda el estado actual de la party y la isla en la que está el jugador
    static void Save(Party* party, int currentIslandId);

    // Carga el fichero de guardado y devuelve los datos.
    // Si no existe el fichero, devuelve SaveData con exists=false.
    static SaveData Load();

    // Devuelve true si existe un fichero de guardado
    static bool HasSaveFile();

    //clean data saving/loading file
    static void ClearSave();

private:
    SaveLoad() = delete;    // no se puede instanciar
};
