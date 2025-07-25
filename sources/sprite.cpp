#include "../headers/sprite.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"
#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

const unsigned int transparent_sprite_limit = 256;

// double quad_vertices[] = {
//     -0.5f, -0.5f, 0.0, 0.0, 1.0,
//     0.5f, -0.5f, 0.0, 1.0, 1.0,
//     0.5f, 0.5f, 0.0, 1.0, 0.0,
//     -0.5f, 0.5f, 0.0, 0.0, 0.0};
double quad_vertices[] = {
    0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
    1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0,
    1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0};
unsigned int quad_indices[] = {
    0, 1, 2,
    0, 3, 2};
double text_quad_vertices[] = {
    0.0, 1.0,
    0.0, 0.0,
    1.0, 1.0,
    1.0, 0.0};
double cube_vertices[] = {
    -0.5f, -0.5f, -0.5f, 0.0, 0.0,
    0.5f, -0.5f, -0.5f, 1.0, 0.0,
    0.5f, 0.5f, -0.5f, 1.0, 1.0,
    0.5f, 0.5f, -0.5f, 1.0, 1.0,
    -0.5f, 0.5f, -0.5f, 0.0, 1.0,
    -0.5f, -0.5f, -0.5f, 0.0, 0.0,

    -0.5f, -0.5f, 0.5f, 0.0, 0.0,
    0.5f, -0.5f, 0.5f, 1.0, 0.0,
    0.5f, 0.5f, 0.5f, 1.0, 1.0,
    0.5f, 0.5f, 0.5f, 1.0, 1.0,
    -0.5f, 0.5f, 0.5f, 0.0, 1.0,
    -0.5f, -0.5f, 0.5f, 0.0, 0.0,

    -0.5f, 0.5f, 0.5f, 1.0, 0.0,
    -0.5f, 0.5f, -0.5f, 1.0, 1.0,
    -0.5f, -0.5f, -0.5f, 0.0, 1.0,
    -0.5f, -0.5f, -0.5f, 0.0, 1.0,
    -0.5f, -0.5f, 0.5f, 0.0, 0.0,
    -0.5f, 0.5f, 0.5f, 1.0, 0.0,

    0.5f, 0.5f, 0.5f, 1.0, 0.0,
    0.5f, 0.5f, -0.5f, 1.0, 1.0,
    0.5f, -0.5f, -0.5f, 0.0, 1.0,
    0.5f, -0.5f, -0.5f, 0.0, 1.0,
    0.5f, -0.5f, 0.5f, 0.0, 0.0,
    0.5f, 0.5f, 0.5f, 1.0, 0.0,

    -0.5f, -0.5f, -0.5f, 0.0, 1.0,
    0.5f, -0.5f, -0.5f, 1.0, 1.0,
    0.5f, -0.5f, 0.5f, 1.0, 0.0,
    0.5f, -0.5f, 0.5f, 1.0, 0.0,
    -0.5f, -0.5f, 0.5f, 0.0, 0.0,
    -0.5f, -0.5f, -0.5f, 0.0, 1.0,

    -0.5f, 0.5f, -0.5f, 0.0, 1.0,
    0.5f, 0.5f, -0.5f, 1.0, 1.0,
    0.5f, 0.5f, 0.5f, 1.0, 0.0,
    0.5f, 0.5f, 0.5f, 1.0, 0.0,
    -0.5f, 0.5f, 0.5f, 0.0, 0.0,
    -0.5f, 0.5f, -0.5f, 0.0, 1.0};

sprite::sprite()
{
    texture_path = "";
    x = 0.0;
    y = 0.0;
}
sprite::sprite(shader *program, object *sprite_object, std::string path, unsigned int _fx, unsigned int _fy, bool text, bool transparentTexture)
{
    shaderP = program;
    objectP = sprite_object;

    textureWidth = 1.0;
    textureHeight = 1.0;

    texture_path = path;
    textureIncludesTransparency = transparentTexture;

    framesX = 1;
    framesY = 1;
    if (_fx > 1)
        framesX = _fx;
    if (_fy > 1)
        framesY = _fy;

    if (!text)
        textureInit();
    else
    {
        spriteW = 126.0;
        spriteH = 126.0;

        textureWidth = 1.0;
        textureHeight = 1.0;
    }
}
void sprite::textureInit()
{
    glGenTextures(1, &sprite_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sprite_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    float borderColor[] = {0.0, 0.0, 0.0, 1.0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(texture_path.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "\n\tError: " << texture_path << " failed to load\n";
    }

    stbi_image_free(data);

    spriteW = width / framesX;
    spriteH = height / framesY;

    textureWidth = static_cast<double>(spriteW) / width;
    textureHeight = static_cast<double>(spriteH) / height;

    empty = false;
}
void sprite::Put(double _x, double _y, double _z)
{
    x = _x;
    y = _y;
    z = _z;
}
void sprite::Move(double _xdist, double _ydist, double _zdist)
{
    x += _xdist;
    y += _ydist;
    z += _zdist;
}
void sprite::Scale(double _w, double _h, double _d)
{
    w = _w;
    h = _h;
    d = _d;
}
void sprite::Rotate(double _rx, double _ry, double _rz)
{
    rx = _rx;
    ry = _ry;
    rz = _rz;
}

void sprite::SetColor(double _r, double _g, double _b, double _a)
{
    colr = _r;
    colg = _g;
    colb = _b;
    cola = _a;
}

sprite *transparentSpriteList[transparent_sprite_limit] = {nullptr};
unsigned int transparentSpriteCount = 0;

void resetTransparentSprites()
{
    for (int i = 0; i < transparent_sprite_limit; ++i)
    {
        if (transparentSpriteList[i] == nullptr)
            continue;

        transparentSpriteList[i]->inTransparencyList = false;
        transparentSpriteList[i] = nullptr;
    }
    transparentSpriteCount = 0;
}

void drawTransparentSprites(camera &cam)
{
    if (transparentSpriteCount == 0)
        return;

    std::map<double, sprite *> drawOrderMap;
    for (int i = 0; i < transparentSpriteCount; ++i)
    {
        if (transparentSpriteList[i] == nullptr)
        {
            std::cout << "\n\twarning: sprite doesn't exist??\n";
            continue;
        }

        double length = glm::length2(cam.cameraPosition - glm::dvec3(transparentSpriteList[i]->x, transparentSpriteList[i]->y, transparentSpriteList[i]->z));
        drawOrderMap[length] = transparentSpriteList[i];
    }

    for (std::map<double, sprite *>::iterator iter = drawOrderMap.begin(); iter != drawOrderMap.end(); ++iter)
    {
        if (iter->second->shaderP == nullptr)
        {
            std::cout << "shader for sprite " << iter->second->texture_path << " does not exist or is not assigned, skipping draw and removing from transparency list.\n";
            iter->second->inTransparencyList = false;
            continue;
        }
        iter->second->trueDraw(false);
    }
}

void sprite::Draw(bool wireframe)
{
    if (cola == 0.0)
        return;

    bool thisSpriteTransparent = false;

    if (cola < 1.0 || textureIncludesTransparency)
    {
        thisSpriteTransparent = true;
    }
    if (objectP->instanceCount > 1 && objectP->instanceColorArray != nullptr)
    {
        for (int i = 0; i < objectP->instanceCount; ++i)
        {
            if (objectP->instanceColorArray[i].w < 1.0 && objectP->instanceColorArray[i].w > 0.0)
            {
                thisSpriteTransparent = true;
            }
        }
    }

    if (!inTransparencyList && thisSpriteTransparent && !wireframe)
    {
        if (transparentSpriteCount >= transparent_sprite_limit)
        {
            std::cout << "\n\tToo many transparent sprites! Try increasing the limit or taking out some transparent objects\n";
        }
        transparentSpriteList[transparentSpriteCount] = this;
        inTransparencyList = true;
        ++transparentSpriteCount;
        return;
    }
    if (inTransparencyList && !thisSpriteTransparent)
    {
        int index = -1;
        for (int i = 0; i < transparentSpriteCount; ++i)
        {
            if (this == transparentSpriteList[i])
            {
                index = i;
                break;
            }
        }
        for (int i = index; i < transparentSpriteCount - 1; ++i)
        {
            transparentSpriteList[i] = transparentSpriteList[i + 1];
        }
        inTransparencyList = false;
        --transparentSpriteCount;
    }
    if (thisSpriteTransparent)
        return;

    trueDraw(wireframe);
}
void sprite::trueDraw(bool wireframe)
{
    shaderP->use();
    if (objectP->obj_type != OBJ_TEXT)
    {
        // pixel perfect snapping
        glm::vec3 new_pivot(x + xOffset, y + yOffset, z + zOffset);
        glm::vec3 pixel_position;

        double inverted_pixel_scale = 1.0 / pixel_scale;
        if (pixel_scale < 1.0)
        {
            pixel_position.x = std::round(new_pivot.x * inverted_pixel_scale) / inverted_pixel_scale;
            pixel_position.y = std::round(new_pivot.y * inverted_pixel_scale) / inverted_pixel_scale;
            pixel_position.z = std::round(new_pivot.z * inverted_pixel_scale) / inverted_pixel_scale;
        }

        double anti_jitter_epsilon = 0.01;
        if (std::abs(pixel_position.x - (x + xOffset)) < anti_jitter_epsilon)
        {
            pixel_position.x = (x + xOffset);
        }
        if (std::abs(pixel_position.y - (y + yOffset)) < anti_jitter_epsilon)
        {
            pixel_position.y = (y + yOffset);
        }
        if (std::abs(pixel_position.z - (z + zOffset)) < anti_jitter_epsilon)
        {
            pixel_position.z = (z + zOffset);
        }

        glm::mat4 model = glm::mat4(1.0);
        // why is there a glm::dvec3 but no option to translate using it??? searching web shows up nothing relevant
        model = glm::translate(model, pixel_position);
        model = glm::rotate(model, glm::radians(static_cast<float>(rx)), glm::vec3(1.0, 0.0, 0.0));
        model = glm::rotate(model, glm::radians(static_cast<float>(ry)), glm::vec3(0.0, 1.0, 0.0));
        model = glm::rotate(model, glm::radians(static_cast<float>(rz)), glm::vec3(0.0, 0.0, 1.0));
        model = glm::scale(model, glm::vec3(w, h, d));
        shaderP->setUniformInt("tex", 0);
        shaderP->setUniformMat4("model", model);
        glm::mat3 normal_model = glm::transpose(glm::inverse(model));
        shaderP->setUniformMat3("normal_model", normal_model);
        shaderP->setUniformVec2("tex_offset", textureX, textureY);
        shaderP->setUniformVec2("tex_scale", textureWidth, textureHeight);
        shaderP->setUniformVec4("color", colr, colg, colb, cola);

        shaderP->setUniformBool("instanced", (objectP->instanceCount >= 1));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sprite_texture);
    }

    if (wireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    switch (objectP->obj_type)
    {
    case OBJ_QUAD:
        glBindVertexArray(objectP->VAO);
        glBindBuffer(1, objectP->EBO);
        if (objectP->instanceCount == 0)
        {
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        else
        {
            glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, objectP->instanceCount);
        }
        break;
    case OBJ_CUBE:
        glBindVertexArray(objectP->VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        break;
    case OBJ_NULL:
        break;
    case OBJ_TEXT:
        std::cout << sprite_texture << ", " << texture_path << " lol\n";
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, sprite_texture);
        glBindVertexArray(objectP->VAO);

        glBindBuffer(GL_ARRAY_BUFFER, objectP->VBO);
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, objectP->instanceCount);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        break;
    default:
        break;
    }
    if (wireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

object::object() {}
object::object(object_type _obj)
{
    obj_type = _obj;

    switch (obj_type)
    {
    case OBJ_QUAD:
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 8 * sizeof(double), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_DOUBLE, GL_FALSE, 8 * sizeof(double), (void *)(3 * sizeof(double)));
        glEnableVertexAttribArray(8);
        glVertexAttribPointer(8, 3, GL_DOUBLE, GL_FALSE, 8 * sizeof(double), (void *)(5 * sizeof(double)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        break;
    case OBJ_CUBE:
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 5 * sizeof(double), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_DOUBLE, GL_FALSE, 5 * sizeof(double), (void *)(3 * sizeof(double)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        break;
    case OBJ_TEXT:
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(text_quad_vertices), text_quad_vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        break;
    default:
        break;
    }
}
void object::setInstances(unsigned int _instanceCount, glm::mat4 *instanceMap, glm::vec2 *textureOffsetInstanceMap,
                          glm::vec4 *colorInstanceMap, int *textureInstanceMap)
{
    instanceCount = _instanceCount;

    if (instanceCount > 1 && instanceMap != nullptr)
    {
        instanceArray = instanceMap;

        glGenBuffers(1, &instanceVBO);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * instanceCount, &instanceArray[0], GL_STATIC_DRAW);

        glBindVertexArray(VAO);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)(3 * sizeof(glm::vec4)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
        glBindVertexArray(0);
    }
    if (instanceCount > 1 && textureOffsetInstanceMap != nullptr)
    {
        instanceTextureOffsetArray = textureOffsetInstanceMap;

        glGenBuffers(1, &instanceVBO);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * instanceCount, &instanceTextureOffsetArray[0], GL_STATIC_DRAW);

        glBindVertexArray(VAO);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void *)0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glVertexAttribDivisor(2, 1);
        glBindVertexArray(0);
    }
    if (instanceCount > 1 && colorInstanceMap != nullptr)
    {
        instanceColorArray = colorInstanceMap;

        glGenBuffers(1, &instanceVBO);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * instanceCount, &instanceColorArray[0], GL_STATIC_DRAW);

        glBindVertexArray(VAO);
        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void *)0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glVertexAttribDivisor(7, 1);
        glBindVertexArray(0);
    }
    if (instanceCount > 1 && textureInstanceMap != nullptr)
    {
        instanceTextureArray = textureInstanceMap;

        glGenBuffers(1, &instanceVBO);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int) * instanceCount, &instanceTextureArray[0], GL_STATIC_DRAW);

        glBindVertexArray(VAO);
        glEnableVertexAttribArray(9);
        glVertexAttribPointer(9, 1, GL_FLOAT, GL_FALSE, sizeof(unsigned int), (void *)0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glVertexAttribDivisor(9, 1);
        glBindVertexArray(0);
    }
}
void object::objectKill()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}
// Text globals maybe can be made not global? But who cares? Also get rid of the absurd amount of globals in main.cpp? You terrible coder?

// temp
// glm::vec4 renderText(object &spriteObject, shader &shaderProgram, std::string text, double x, double y, double scale, glm::vec4 color, unsigned int texture_id)
// {
//     scale = scale * 48.0 / 256.0;
//     double newlineX = x;
//     shaderProgram.use();
//     shaderProgram.setUniformVec4("textColor", color.x, color.y, color.z, color.w);

//     glActiveTexture(GL_TEXTURE0);
//     glBindTexture(GL_TEXTURE_2D_ARRAY, texture_id);
//     glBindVertexArray(spriteObject.VAO);

//     glBindBuffer(GL_ARRAY_BUFFER, spriteObject.VBO);

//     glm::vec4 returnVec = glm::vec4(x, y, 0, 0);

//     unsigned int working_index = 0;

//     std::string::const_iterator c;
//     for (c = text.begin(); c != text.end(); ++c)
//     {
//         if (*c == '\n' || *c == ' ')
//             continue;
//         working_index++;
//     }

//     glm::mat4 textTransformArray[working_index];
//     int textLetterIndexArray[working_index];
//     working_index = 0;

//     for (c = text.begin(); c != text.end(); c++)
//     {
//         shaderProgram.use();
//         textCharacter ch = textCharacters[*c];

//         if (*c == '\n')
//         {
//             y -= ((ch.Size.y)) * 1.3 * scale;
//             x = newlineX;
//             continue;
//         }
//         if (*c == ' ')
//         {
//             x += (ch.Advance >> 6) * scale;
//             continue;
//         }

//         double xpos = x + ch.Bearing.x * scale;
//         double ypos = y - (256.0 - ch.Bearing.y) * scale;
//         double w = 256.0 * scale;
//         double h = 256.0 * scale;

//         textTransformArray[working_index] = glm::translate(glm::mat4(1.0), glm::vec3(xpos, ypos, 0.0)) * glm::scale(glm::mat4(1.0), glm::vec3(w, h, 0.0)); // make z pos equal to gui zpos
//         textLetterIndexArray[working_index] = ch.letterID;

//         unsigned int testAdvance = ch.Advance;

//         x += (ch.Advance >> 6) * scale;

//         if (h > returnVec.w)
//             returnVec.w = h;

//         ++working_index;
//     }
//     spriteObject.setInstances(working_index, textTransformArray, nullptr, nullptr, textLetterIndexArray);
//     glBindVertexArray(spriteObject.VAO);
//     glBindBuffer(GL_ARRAY_BUFFER, spriteObject.VBO);

//     returnVec = glm::vec4(returnVec.x, returnVec.y, x, returnVec.y + returnVec.w);

//     // std::cout << working_index << ", " << spriteObject.instanceTextureArray[working_index - 1] << " well\n";
//     glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, working_index);

//     glBindBuffer(GL_ARRAY_BUFFER, 0);
//     glBindVertexArray(0);
//     glBindTexture(GL_TEXTURE_2D, 0);

//     return returnVec;
// }