#ifndef SAVE_DATA_H
#define SAVE_DATA_H

#include "system.h"

template <typename T>
struct savevalue
{
    std::string save_path;

    savevalue(std::string sp) : save_path(sp) {}

    void loadData(size_t dataLocation, T &dataV);
    void saveData(T dataV);
};

template <typename T>
void savevalue<T>::loadData(size_t dataLocation, T &dataV)
{
    std::ifstream save_file;
    save_file.open(save_path, std::ios::in | std::ios::binary);
    if (!save_file.is_open())
    {
        return;
    }
    save_file.seekg(dataLocation);

    save_file.read((char *)&dataV, sizeof(T));
    std::cout << "loading " << dataV << "\n";

    save_file.close();
}

template <typename T>
void savevalue<T>::saveData(T dataV)
{
    std::cout << "saving " << dataV << "\n";
    std::ofstream save_file;
    save_file.open(save_path, std::ios::out | std::ios::binary);
    // save_file.write(reinterpret_cast<char *>(&dataV), sizeof(T));
    save_file.write((char *)&dataV, sizeof(T));
    save_file.close();
}

#endif