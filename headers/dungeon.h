#ifndef DUNGEON_H
#define DUNGEON_H

#include <random>
#include <fstream>
#include "sprite.h"
#include "collision.h"

const unsigned int width_limit = 128;
const unsigned int height_limit = 128;
const unsigned int collision_box_limit = 128;
// const unsigned int collision_box_limit = 2;
const unsigned int collision_id_limit = 8;

struct tile
{
    unsigned int id = 0;
    int collisionID = -1, specialTileID = -1;
    bool collisionTaken = false;
};

enum specialTiles
{
    TILE_BUTTON_0,
    TILE_BUTTON_1,
    TILE_BUTTON_2,
    TILE_BUTTON_3,
    TILE_BUTTON_4,
    TILE_SPRING_0,
    TILE_SPRING_1,
    TILE_SPRING_2,
    TILE_SPRING_3,
    TILE_SPRING_4,
    TILE_SPRING_5,
    TILE_SPRING_6,
    TILE_SPRING_7,
    TILE_SPRING_8,
    TILE_SPRING_9,
    TILE_WALL_BUTTON_0,
    TILE_WALL_BUTTON_1,
    TILE_WALL_BUTTON_2,
    TILE_WALL_BUTTON_3,
    TILE_WALL_BUTTON_4
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
    dungeon(const char *_tileSetPath, unsigned int _fx, unsigned int _fy);

    // void changeScreenViewPosition(glview &view, float newX, float newY);

    void draw(GLFWwindow *win, shader &program, object &sprite_object);

    void readRoomFile(const char *path);
};

#endif