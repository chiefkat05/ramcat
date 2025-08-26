#ifndef SAVE_DATA_H
#define SAVE_DATA_H

#include <iostream>
#include <fstream>
#include <vector>

const unsigned int save_data_type_limit = 64;
// const unsigned int save_data_reference_limit = 64;

template <typename T>
struct savevalue
{
    bool loadData(std::string save_path, size_t dataLocation, T &dataV, size_t dataSize)
    {
        std::ifstream save_file;
        save_file.open(save_path, std::ios::in | std::ios::binary);
        if (!save_file.is_open())
        {
            std::cout << "\n\tWarning: failed to open save path \"" << save_path << "\"\n";
            return false;
        }
        save_file.seekg(dataLocation);

        save_file.read((char *)&dataV, dataSize);

        save_file.close();
        return true;
    }
    void saveData(std::string save_path, T &dataV, size_t dataSize)
    {
        std::ofstream save_file;
        save_file.open(save_path, std::ios::out | std::ios::binary | std::ios::app);
        save_file.write(reinterpret_cast<char *>(&dataV), dataSize); // this was (char *)&dataV before so if major problems arise try changing this back

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

struct save_file_manager
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
    size_t getDataLocation(unsigned int index)
    {
        if (index > smap.size())
        {
            std::cout << "\n\tWarning: Requested save data index greater than total data objects in save file! This function will return the beginning of the file.\n";
            return 0;
        }
        size_t loc = 0;
        for (int i = 0; i < smap.size(); ++i)
        {
            if (i == index)
                break;

            switch (smap[i].type)
            {
            case SDT_INT:
            {
                loc += sizeof(int);
            }
            break;
            case SDT_UINT:
            {
                loc += sizeof(unsigned int);
            }
            break;
            case SDT_DOUBLE:
            {
                loc += sizeof(double);
            }
            break;
            case SDT_BOOL:
            {
                loc += sizeof(bool);
            }
            break;
            case SDT_CHAR:
            {
                loc += sizeof(char);
            }
            break;
            default:
                break;
            }
        }
        return loc;
    }
    void referenceData(unsigned int loc, save_data_type t, void *output) // also a version that you select the data using a location number corresponding to the data's sequential position not byte location
    {
        std::string fp = fullPath();

        switch (t)
        {
        case SDT_INT:
        {
            savevalue<int> temp;
            if (!temp.loadData(fp, loc, *reinterpret_cast<int *>(output), sizeof(int)))
            {
                *reinterpret_cast<int *>(output) = 0;
            }
            break;
        case SDT_UINT:
        {
            savevalue<unsigned int> temp;
            if (!temp.loadData(fp, loc, *reinterpret_cast<unsigned int *>(output), sizeof(unsigned int)))
            {
                *reinterpret_cast<int *>(output) = 0;
            }
        }
        break;
        case SDT_DOUBLE:
        {
            savevalue<double> temp;
            if (!temp.loadData(fp, loc, *reinterpret_cast<double *>(output), sizeof(double)))
            {
                *reinterpret_cast<double *>(output) = 0.0;
            }
        }
        break;
        case SDT_BOOL:
        {
            savevalue<bool> temp;
            if (!temp.loadData(fp, loc, *reinterpret_cast<bool *>(output), sizeof(bool)))
            {
                *reinterpret_cast<bool *>(output) = false;
            }
        }
        break;
        case SDT_CHAR:
        {
            savevalue<char> temp;
            if (!temp.loadData(fp, loc, *(char *)output, sizeof(char)))
            {
                *reinterpret_cast<char *>(output) = ' ';
            }
        }
        break;
        default:
            break;
        }
        }
    }

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
                temp.saveData(fp, *reinterpret_cast<int *>(smap[i].data_reference), sizeof(int));
            }
            break;
            case SDT_UINT:
            {
                savevalue<unsigned int> temp;
                temp.saveData(fp, *reinterpret_cast<unsigned int *>(smap[i].data_reference), sizeof(unsigned int));
            }
            break;
            case SDT_DOUBLE:
            {
                savevalue<double> temp;
                temp.saveData(fp, *reinterpret_cast<double *>(smap[i].data_reference), sizeof(double));
            }
            break;
            case SDT_BOOL:
            {
                savevalue<bool> temp;
                temp.saveData(fp, *reinterpret_cast<bool *>(smap[i].data_reference), sizeof(bool));
            }
            break;
            case SDT_CHAR:
            {
                savevalue<char> temp;
                temp.saveData(fp, *reinterpret_cast<char *>(smap[i].data_reference), sizeof(char));
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
                if (!temp.loadData(fp, loc, *reinterpret_cast<int *>(smap[i].data_reference), sizeof(int)))
                {
                    *reinterpret_cast<int *>(smap[i].data_reference) = 0;
                }
                loc += sizeof(int);
            }
            break;
            case SDT_UINT:
            {
                savevalue<unsigned int> temp;
                if (!temp.loadData(fp, loc, *reinterpret_cast<unsigned int *>(smap[i].data_reference), sizeof(unsigned int)))
                {
                    *reinterpret_cast<unsigned int *>(smap[i].data_reference) = 0;
                }
                loc += sizeof(unsigned int);
            }
            break;
            case SDT_DOUBLE:
            {
                savevalue<double> temp;
                if (temp.loadData(fp, loc, *reinterpret_cast<double *>(smap[i].data_reference), sizeof(double)))
                {
                    *reinterpret_cast<double *>(smap[i].data_reference) = 0.0;
                }
                loc += sizeof(double);
            }
            break;
            case SDT_BOOL:
            {
                savevalue<bool> temp;
                if (!temp.loadData(fp, loc, *reinterpret_cast<bool *>(smap[i].data_reference), sizeof(bool)))
                {
                    *reinterpret_cast<bool *>(smap[i].data_reference) = false;
                }
                loc += sizeof(bool);
            }
            break;
            case SDT_CHAR:
            {
                savevalue<char> temp;
                if (temp.loadData(fp, loc, *reinterpret_cast<char *>(smap[i].data_reference), sizeof(char)))
                {
                    *reinterpret_cast<char *>(smap[i].data_reference) = ' ';
                }
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