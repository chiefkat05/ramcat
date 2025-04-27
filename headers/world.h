#ifndef WORLD_H
#define WORLD_H

#include <random>
#include <fstream>
#include "sprite.h"
#include "collision.h"

const unsigned int width_limit = 128;
const unsigned int height_limit = 128;
const unsigned int collision_box_limit = 128;
const unsigned int collision_id_limit = 24;
const unsigned int tile_animation_limit = 24;

struct tile
{
    unsigned int id = 0;
    int collisionID = -1, specialTileID = -1;
    bool collisionTaken = false;

    void emptyTile()
    {
        id = -1;
        collisionID = -1;
        specialTileID = -1;
        collisionTaken = false;
    }
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

struct world
{
    bool start = true, end = false, worldInitialized = false;
    tile tiles[width_limit][height_limit];

    animation tileAnimations[tile_animation_limit]; // unimplemented, do later pls
    int animationToIDMap[tile_animation_limit];

    object worldObject;
    sprite worldSprite;

    unsigned int roomWidth = 0, roomHeight = 0;
    unsigned int tileSpriteX, tileSpriteY;
    double spawnLocationX = 0.0, spawnLocationY = 0.0;

    double viewBoundsX, viewBoundsY, viewBoundsWidth, viewBoundsHeight;

    const char *tileSetPath;

    aabb collision_boxes[collision_box_limit];
    unsigned int collision_box_count = 0;

    world();
    world(const char *_tileSetPath, unsigned int _fx, unsigned int _fy, object_type obj);

    // void changeScreenViewPosition(glview &view, double newX, double newY);

    void draw(GLFWwindow *win, shader &program);

    void readRoomFile(const char *path);
    tile *getTile(unsigned int tileID);
    tile *getTileFromCollisionSpecialID(unsigned int collisionIndex);
};

#endif