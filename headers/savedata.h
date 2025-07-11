#ifndef SAVE_DATA_H
#define SAVE_DATA_H

#include "system.h"

const unsigned int save_data_type_limit = 64;
// const unsigned int save_data_reference_limit = 64;

template <typename T>
struct savevalue
{
    // void loadData(std::string save_path, size_t dataLocation, T &dataV, size_t dataSize)
    // {
    //     std::ifstream save_file;
    //     save_file.open(save_path, std::ios::in | std::ios::binary);
    //     if (!save_file.is_open())
    //     {
    //         std::cout << "\n\tWarning: failed to open " << save_path << "\n";
    //         return;
    //     }
    //     save_file.seekg(dataLocation);

    //     save_file.read((char *)&dataV, dataSize);
    //     std::cout << "loading " << dataV << "\n";

    //     save_file.close();
    // }
    void loadData(std::string save_path, size_t dataLocation, T &dataV, size_t dataSize)
    {
        std::ifstream save_file;
        save_file.open(save_path, std::ios::in | std::ios::binary);
        if (!save_file.is_open())
        {
            std::cout << "\n\tWarning: failed to open " << save_path << "\n";
            return;
        }
        save_file.seekg(dataLocation);

        save_file.read((char *)&dataV, dataSize);
        std::cout << "loading " << *reinterpret_cast<T *>(&dataV) << "\n";

        save_file.close();
    }
    void saveData(std::string save_path, T &dataV, size_t dataSize)
    {
        std::ofstream save_file;
        save_file.open(save_path, std::ios::out | std::ios::binary | std::ios::app);
        save_file.write(reinterpret_cast<char *>(&dataV), dataSize); // this was (char *)&dataV before so if major problems arise try changing this back

        std::cout << "saving " << *reinterpret_cast<T *>(&dataV) << " at pointer " << &dataV << "\n";
        save_file.close();
    }
};

enum save_data_type
{
    SDT_INT,
    SDT_UINT,
    SDT_DOUBLE,
    SDT_BOOL,
    SDT_CHAR,
    sdt_limit
};
struct save_data
{
    save_data_type type;
    void *data_reference;
};

struct save_file_manager // do it again but keep in mind that editing the text file produces char* values so you should save the file from this function before loading it
{
    std::string save_path = "", save_extension = "";
    unsigned int save_location = 0;

    std::vector<save_data> smap;

    void setPath(std::string path, std::string exe)
    {
        save_path = path;
        save_extension = exe;
    }
    void setCurrentSaveLoc(unsigned int loc)
    {
        save_location = loc;
    }
    void addDataToMap(save_data_type t, void *ref)
    {
        smap.push_back({t, ref});
    }
    // void removeData // what would be the point of this

    std::string fullPath()
    {
        return save_path + std::to_string(save_location) + save_extension;
    }
    void saveGame()
    {
        std::string fp = fullPath();

        std::ofstream saveFile(fp, std::ios::trunc);

        for (int i = 0; i < smap.size(); ++i)
        {
            switch (smap[i].type)
            {
            case SDT_INT:
            {
                savevalue<int> temp;
                temp.saveData(fp, *(int *)smap[i].data_reference, sizeof(int)); // fix these all to be reinterpret_casts
            }
            break;
            case SDT_UINT:
            {
                savevalue<unsigned int> temp;
                temp.saveData(fp, *(unsigned int *)smap[i].data_reference, sizeof(unsigned int));
            }
            break;
            case SDT_DOUBLE:
            {
                savevalue<double> temp;
                temp.saveData(fp, *(double *)smap[i].data_reference, sizeof(double));
            }
            break;
            case SDT_BOOL:
            {
                savevalue<bool> temp;
                temp.saveData(fp, *(bool *)smap[i].data_reference, sizeof(bool));
            }
            break;
            case SDT_CHAR:
            {
                savevalue<char> temp;
                temp.saveData(fp, *(char *)smap[i].data_reference, sizeof(char));
            }
            break;
            default:
                break;
            }
        }
    }
    void loadGame()
    {
        std::string fp = fullPath();

        size_t loc = 0;
        for (int i = 0; i < smap.size(); ++i)
        {
            switch (smap[i].type)
            {
            case SDT_INT:
            {
                savevalue<int> temp;
                temp.loadData(fp, loc, *(int *)smap[i].data_reference, sizeof(int)); // fix these to also be reinterpret_casts
                loc += sizeof(int);
            }
            break;
            case SDT_UINT:
            {
                savevalue<unsigned int> temp;
                temp.loadData(fp, loc, *(unsigned int *)smap[i].data_reference, sizeof(unsigned int));
                loc += sizeof(unsigned int);
            }
            break;
            case SDT_DOUBLE:
            {
                savevalue<double> temp;
                temp.loadData(fp, loc, *(double *)smap[i].data_reference, sizeof(double));
                loc += sizeof(double);
            }
            break;
            case SDT_BOOL:
            {
                savevalue<bool> temp;
                temp.loadData(fp, loc, *(bool *)smap[i].data_reference, sizeof(bool));
                loc += sizeof(bool);
            }
            break;
            case SDT_CHAR:
            {
                savevalue<char> temp;
                temp.loadData(fp, loc, *(char *)smap[i].data_reference, sizeof(char));
                loc += sizeof(char);
            }
            break;
            default:
                break;
            }
        }
    }
};

#endif