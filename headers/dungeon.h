#ifndef DUNGEON_H
#define DUNGEON_H

#include <random>
#include <fstream>
#include "sprite.h"
#include "collision.h"

const unsigned int width_limit = 128;
const unsigned int height_limit = 128;
const unsigned int collision_box_limit = 128;
const unsigned int collision_id_limit = 3;

struct tile
{
    unsigned int id = 0;
    int collisionID = -1;
    bool collisiontaken;
};

struct dungeon
{
    bool start = true, end = false, dungeonInitialized = false;
    tile tiles[width_limit][height_limit];
    sprite dungeonSprite;

    unsigned int roomWidth = 0, roomHeight = 0;
    unsigned int tileSpriteX, tileSpriteY;
    float spawnLocationX = 0.0f, spawnLocationY = 0.0f;

    float viewBoundsX, viewBoundsY, viewBoundsWidth, viewBoundsHeight;

    const char *tileSetPath;

    aabb collision_boxes[collision_box_limit];
    unsigned int collision_box_count = 0;

    dungeon();
    dungeon(const char *_tileSetPath, object *_spriteObject, unsigned int _fx, unsigned int _fy);

    // void changeScreenViewPosition(glview &view, float newX, float newY);

    void draw(GLFWwindow *win, shader &program);

    void readRoomFile(const char *path);
};

#endif