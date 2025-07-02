#include "../headers/savedata.h"
#include <ios>

// template <typename T>
// void savevalue<T>::loadData(size_t dataLocation, T &dataV)
// {
//     std::ifstream save_file(save_path);
//     if (!save_file.is_open())
//     {
//         return;
//     }
//     char *buffer[sizeof(T)];
//     save_file.open(save_path, std::ios::in | std::ios::binary);
//     save_file.seekg(dataLocation);
//     save_file.readsome(buffer, sizeof(T));
//     save_file.close();
// }

// template <typename T>
// void savevalue<T>::saveData(T dataV)
// {
//     std::ofstream save_file(save_path);
//     save_file.open(save_path, std::ios::out | std::ios::binary);
//     save_file.write((char *)dataV, sizeof(T));
//     save_file.close();
// }