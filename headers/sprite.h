#ifndef SPRITE_H
#define SPRITE_H

#include "graphics.h"

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
    float x = 0.0f, y = 0.0f, w = 0.0f, h = 0.0f;
    unsigned int spriteW, spriteH;
    unsigned int framesX, framesY;
    unsigned int textureX, textureY, textureWidth, textureHeight;
    // std::string path;
    // sf::Sprite rect;
    unsigned int sprite_texture;
    bool empty = true;
    // object_type obj_type;
    object *sprite_object;

    sprite(object *_obj);

    // sprite(const char *_path, float _x, float _y, unsigned int _fx, unsigned int _fy);
    void setTexture(const char *path, unsigned int &textureID);

    void Put(float _x, float _y);
    void Move(float _xdist, float _ydist);

    void Draw(shader &program, unsigned int VAO, unsigned int EBO);
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
        _sprite->textureX = frame % _sprite->framesX * _sprite->spriteW;
        _sprite->textureY = frame / _sprite->framesX * _sprite->spriteH;
        _sprite->textureWidth = _sprite->spriteW * xScale;
        _sprite->textureHeight = _sprite->spriteH * yScale;
        // _sprite->rect.setTextureRect(sf::IntRect(frame % _sprite->framesX * _sprite->spriteW, frame / _sprite->framesX * _sprite->spriteH,
        //                                          _sprite->spriteW * xScale, _sprite->spriteH * yScale));
    }
};

// void clearAllTextures();

#endif