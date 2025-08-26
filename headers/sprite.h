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
    OBJ_NULL
};

struct object
{
    unsigned int VBO, VAO, EBO;
    unsigned int instanceCount = 0, instanceVBO, instanceTextureVBO;
    glm::mat4 *instanceArray;
    glm::vec2 *instanceTextureOffsetArray;
    glm::vec4 *instanceColorArray;
    int *instanceTextureArray;
    object_type obj_type;
    object();
    object(object_type _obj);
    void setInstances(unsigned int _instanceCount = 0, glm::mat4 *instanceMap = nullptr, glm::vec2 *textureOffsetInstanceMap = nullptr,
                      glm::vec4 *colorInstanceMap = nullptr, int *textureInstanceMap = nullptr);
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
    bool inTransparencyList = false, textureIncludesTransparency = false;

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
    sprite(shader *program, object *sprite_object, std::string path, unsigned int _fx = 1, unsigned int _fy = 1, bool text = false, bool transparentTexture = false);

    void textureInit();

    void Put(double _x, double _y, double _z);
    void Move(double _xdist, double _ydist, double _zdist);
    void Scale(double _w, double _h, double _d);
    void Rotate(double _rx, double _ry, double _rz);
    void SetColor(double _r, double _g, double _b, double _a);

    void Draw(bool wireframe = false);
    void trueDraw(bool wireframe);

    constexpr bool operator==(sprite &o)
    {
        // probably could make this behemoth more efficient; but I need positional comparisions so
        if (x != o.x || y != o.y || z != o.z || w != o.w || h != o.h || d != o.d || rx != o.rx || ry != o.ry || rz != o.rz || xOffset != o.xOffset ||
            yOffset != o.yOffset || zOffset != o.zOffset || colr != o.colr || colg != o.colg || colb != o.colb || cola != o.cola || spriteW != o.spriteW ||
            spriteH != o.spriteH || framesX != o.framesX || framesY != o.framesY || textureX != o.textureX || textureY != o.textureY || textureWidth != o.textureWidth ||
            textureHeight != o.textureHeight || textureIncludesTransparency != o.textureIncludesTransparency || sprite_texture != o.sprite_texture ||
            texture_path != o.texture_path || shaderP != o.shaderP || objectP != o.objectP)
            return false;
        return true;
    }
};

struct animation
{
    unsigned int start = 0, end = 0;

    double speed = 1.0;
    double timer = 10.0;
    unsigned int frame = 0;
    bool finished = true; // objective: I want to activate the strike collider when the correct frame hits and deactivate it when the next frame happens
    // can this be simplified from a function? Can I hardcode it maybe? Probably would be best if only things inside the character/sprite object could be accessed for animation metadata purposes

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
            std::cout << "error: animation not attached to a sprite!\n"; // needs an anim_metadata or something to add functions when a frame hits
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
void drawTransparentSprites(camera &cam);
void resetTransparentSprites();

#endif