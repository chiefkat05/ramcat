#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
    void setUniformMat4(const char *name, glm::mat4 matrix);
};

#endif