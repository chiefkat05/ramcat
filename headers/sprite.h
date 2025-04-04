#ifndef SPRITE_H
#define SPRITE_H

#include "graphics.h"
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/freetype.h>

enum object_type
{
    OBJ_QUAD,
    OBJ_CUBE,
    OBJ_MODEL,
    OBJ_TEXT,
    OBJ_INSTANCED,
    OBJ_NULL
};

struct object
{
    unsigned int VBO, VAO, EBO;
    object_type obj_type;
    object(object_type _obj);
    void objectKill();
};

struct sprite
{
    double x = 0.0, y = 0.0, z = 0.0, w = 1.0, h = 1.0, d = 1.0, rx = 0.0, ry = 0.0, rz = 0.0;
    double colr = 1.0, colg = 1.0, colb = 1.0, cola = 1.0;
    unsigned int spriteW = 0, spriteH = 0;
    unsigned int framesX = 1, framesY = 1;
    unsigned int textureX = 0, textureY = 0;
    double textureWidth = 0, textureHeight = 0;
    // std::string path;
    // sf::Sprite rect;
    unsigned int sprite_texture;
    bool empty = true;
    // object_type obj_type;
    const char *texture_path;

    sprite();
    sprite(const char *path, unsigned int _fx = 1, unsigned int _fy = 1, bool text = false);

    // sprite(const char *_path, double _x, double _y, unsigned int _fx, unsigned int _fy);
    void textureInit();

    void Put(double _x, double _y, double _z);
    void Move(double _xdist, double _ydist, double _zdist);
    void Scale(double _w, double _h, double _d);
    void Rotate(double _rx, double _ry, double _rz);
    void SetColor(double _r, double _g, double _b, double _a);

    void Draw(shader &program, object &sprite_object);
};

struct animation
{
    unsigned int start = 0, end = 0;

    double speed = 1.0;
    double timer = 10.0;
    unsigned int frame = 0;
    bool finished = true;
    double xScale = 1.0, yScale = 1.0;

    sprite *_sprite;

    animation()
    {
        _sprite = nullptr;
    }

    animation(sprite *sp, unsigned int s, unsigned int e = 1, double spd = 1.0) : _sprite(sp)
    {
        start = s;
        end = e;
        speed = spd;
    }
    void setScale(double x, double y)
    {
        xScale = x;
        yScale = y;
    }

    void run(double delta_time, bool loop)
    {
        finished = false;
        if (timer <= 0.0)
        {
            if (frame >= end)
                finished = true;

            timer = 10.0;

            if (loop)
            {
                frame >= end ? frame = 0 : ++frame;
            }
            if (!loop)
            {
                frame >= end ? frame = end : ++frame;
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
        _sprite->textureX = frame % _sprite->framesX;
        _sprite->textureY = frame / _sprite->framesX;
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