#ifndef SPRITE_H
#define SPRITE_H

#include "graphics.h"
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/freetype.h>

const unsigned int texture_limit = 32;

// struct texturePile
// {
//     sf::Texture list[texture_limit];
//     const char *paths[texture_limit];

//     unsigned int count = 0;
// };

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
    float x = 0.0f, y = 0.0f, z = 0.0f, w = 1.0f, h = 1.0f, d = 1.0f, rx = 0.0f, ry = 0.0f, rz = 0.0f;
    float colr = 1.0f, colg = 1.0f, colb = 1.0f, cola = 1.0f;
    unsigned int spriteW = 0, spriteH = 0;
    unsigned int framesX = 1, framesY = 1;
    unsigned int textureX = 0, textureY = 0;
    float textureWidth = 0, textureHeight = 0;
    // std::string path;
    // sf::Sprite rect;
    unsigned int sprite_texture;
    bool empty = true;
    // object_type obj_type;
    object *sprite_object = nullptr;
    const char *texture_path;

    sprite();
    sprite(object *_obj, const char *path, unsigned int _fx = 1, unsigned int _fy = 1);

    // sprite(const char *_path, float _x, float _y, unsigned int _fx, unsigned int _fy);
    void textureInit();

    void Put(float _x, float _y, float _z);
    void Move(float _xdist, float _ydist, float _zdist);
    void Scale(float _w, float _h, float _d);
    void Rotate(float _rx, float _ry, float _rz);
    void SetColor(float _r, float _g, float _b, float _a);

    void Draw(shader &program);
};

struct animation
{
    unsigned int start = 0, end = 0;

    float speed = 1.0f;
    float timer = 10.0f;
    unsigned int frame = 0;
    bool finished = false;
    float xScale = 1.0f, yScale = 1.0f;

    sprite *_sprite;

    animation()
    {
        _sprite = nullptr;
    }

    animation(sprite *sp, unsigned int s, unsigned int e = 1, float spd = 1.0f) : _sprite(sp)
    {
        start = s;
        end = e;
        speed = spd;
    }
    void setScale(float x, float y)
    {
        xScale = x;
        yScale = y;
    }

    void run(float delta_time, bool loop)
    {
        finished = false;
        if (timer <= 0.0f)
        {
            if (frame >= end)
                finished = true;

            timer = 10.0f;

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
void renderText(object &spriteObject, shader &shaderProgram, std::string text, float x, float y, float scale, glm::vec4 color);

// void clearAllTextures();

#endif