#ifndef SPRITE_H
#define SPRITE_H

#include "graphics.h"
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/freetype.h>

const unsigned int instance_limit = 256;

enum object_type
{
    OBJ_QUAD,
    OBJ_CUBE,
    OBJ_MODEL,
    OBJ_TEXT,
    OBJ_NULL
};

struct object
{
    unsigned int VBO, VAO, EBO;
    unsigned int instanceCount = 0, instanceVBO, instanceTextureVBO;
    glm::mat4 *instanceArray;
    glm::vec3 *instanceTextureArray;
    object_type obj_type;
    object();
    object(object_type _obj);
    void setInstances(unsigned int _instanceCount = 0, glm::mat4 *instanceMap = nullptr, glm::vec3 *textureInstanceMap = nullptr);
    void objectKill();
};

struct sprite
{
    double x = 0.0, y = 0.0, z = 0.0, w = 1.0, h = 1.0, d = 1.0, rx = 0.0, ry = 0.0, rz = 0.0;
    double xOffset = 0.0, yOffset = 0.0, zOffset = 0.0; // for special cases like player sprite flipping
    double colr = 1.0, colg = 1.0, colb = 1.0, cola = 1.0;
    unsigned int spriteW = 0, spriteH = 0;
    unsigned int framesX = 1, framesY = 1;
    unsigned int textureX = 0, textureY = 0;
    double textureWidth = 0, textureHeight = 0;
    bool inTransparencyList = false;

    double trueW() { return spriteW * pixel_scale; }
    double trueH() { return spriteH * pixel_scale; }
    // std::string path;
    // sf::Sprite rect;
    unsigned int sprite_texture;
    bool empty = true;
    // object_type obj_type;
    std::string texture_path;

    shader *shaderP = nullptr;
    object *objectP = nullptr;

    sprite();
    sprite(shader *program, object *sprite_object, std::string path, unsigned int _fx = 1, unsigned int _fy = 1, bool text = false);

    void textureInit();

    void Put(double _x, double _y, double _z);
    void Move(double _xdist, double _ydist, double _zdist);
    void Scale(double _w, double _h, double _d);
    void Rotate(double _rx, double _ry, double _rz);
    void SetColor(double _r, double _g, double _b, double _a);

    void Draw(bool wireframe = false);
};

struct animation
{
    unsigned int start = 0, end = 0;

    double speed = 1.0;
    double timer = 10.0;
    unsigned int frame = 0;
    bool finished = true;

    sprite *_sprite;

    animation()
    {
        _sprite = nullptr;
    }

    animation(sprite *sp, unsigned int s = 0, unsigned int e = 1, double spd = 1.0) : _sprite(sp)
    {
        start = s;
        end = e;
        speed = spd;
        frame = s;
    }

    void run(double delta_time, bool loop, bool changeSprite = true, bool *updated = nullptr)
    {
        finished = false;
        if (timer <= 0.0)
        {
            if (frame >= end)
                finished = true;

            timer = 10.0;

            if (loop)
            {
                frame >= end ? frame = start : ++frame;
            }
            if (!loop)
            {
                frame >= end ? frame = end : ++frame;
            }

            if (updated != nullptr)
            {
                *updated = true;
            }
        }

        timer -= speed * delta_time;

        if (_sprite == nullptr)
        {
            std::cout << "error: animation not attached to a sprite!\n";
            finished = true;
            return;
        }
        // _sprite->textureX = frame % _sprite->framesX * _sprite->spriteW;
        // _sprite->textureY = frame / _sprite->framesX * _sprite->spriteH;
        if (changeSprite)
        {
            _sprite->textureX = frame % _sprite->framesX;
            _sprite->textureY = frame / _sprite->framesX;
        }
        // _sprite->textureWidth = _sprite->spriteW * xScale;
        // _sprite->textureHeight = _sprite->spriteH * yScale;
        // _sprite->rect.setTextureRect(sf::IntRect(frame % _sprite->framesX * _sprite->spriteW, frame / _sprite->framesX * _sprite->spriteH,
        //                                          _sprite->spriteW * xScale, _sprite->spriteH * yScale));
    }
};

struct textCharacter
{
    unsigned int TextureID;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    unsigned int Advance;
};
int loadFont(const char *path);
glm::vec4 renderText(object &spriteObject, shader &shaderProgram, std::string text, double x, double y, double scale, glm::vec4 color);

// void clearAllTextures();

#endif