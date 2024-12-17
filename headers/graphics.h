#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <glad/glad.h>

#include <iostream>
#include <fstream>

std::string readFileToString(std::string path);

class shader
{
public:
    unsigned int ID;

    void use();
    shader(const char *vPath, const char *fPath);

    void setUniformVec4(const char *name, float x, float y, float z, float w);
    void setUniformVec3(const char *name, float x, float y, float z);
    void setUniformVec2(const char *name, float x, float y);
    void setUniformFloat(const char *name, float x);
    void setUniformInt(const char *name, int x);
    void setUniformBool(const char *name, bool x);
};

#endif