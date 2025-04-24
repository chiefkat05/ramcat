#ifndef SAVE_DATA_H
#define SAVE_DATA_H

#include "system.h"

struct savedata
{
    character *saveChar[character_limit];
    int level;

    void save(std::string path, game_system &game)
    {
        std::ofstream saveFile(path);

        if (!saveFile.is_open())
        {
            std::cout << "\n\tError: Save file could not be opened.\n";
            return;
        }

        for (int i = 0; i < game.characterCount; ++i)
        {
            // double chX = game.characters[i]->visual.x;
            // std::string writeString(i + "x" + chX);
            saveFile.write(std::string(i + " hello\n").c_str(), 7);
        }
    }
    void load(std::string path)
    {
    }
};

#endif