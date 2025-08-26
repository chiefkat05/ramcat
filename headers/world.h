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
const unsigned int tile_color_limit = 12;

struct tile
{
    int id = -1, colliderID = -1, specialTileID = -1, animationIndexID = -1, colorIndexID = -1;
    bool collisionTaken = false;

    void emptyTile()
    {
        id = -1;
        colliderID = -1;
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
    bool tileColorsNeedUpdate = false;
    tile tiles[width_limit][height_limit];

    animation tileAnimations[tile_animation_limit];
    unsigned int tileAnimationCount = 0;
    int animationToIDMap[tile_animation_limit];

    glm::vec4 tileColors[tile_color_limit] = {glm::vec4(1.0)};
    // sets first color to default value if unspecified variables
    void setColorList(int color_index = 0, double r = 1.0, double g = 1.0, double b = 1.0, double a = 1.0)
    {
        tileColors[color_index] = glm::vec4(r, g, b, a);
    }

    sprite worldSprite;

    unsigned int roomWidth = 0, roomHeight = 0;
    unsigned int tileSpriteX, tileSpriteY;
    double spawnLocationX = 0.0, spawnLocationY = 0.0;

    double viewBoundsX, viewBoundsY, viewBoundsWidth, viewBoundsHeight;

    const char *tileSetPath;

    aabb collision_boxes[collision_box_limit];
    unsigned int collision_box_count = 0;

    world();
    world(const char *_tileSetPath, unsigned int _fx, unsigned int _fy, object *worldObject, shader *program);

    // void changeScreenViewPosition(glview &view, double newX, double newY);

    void draw();

    void readRoomFile(const char *path, object &worldObject);
    tile *getTile(unsigned int tileID);
    // tile *getTileFromCollisionSpecialID(unsigned int collisionIndex);

    void setTileAnimation(unsigned int specialTileID, unsigned int s = 0, unsigned int e = 1, double spd = 1.0)
    {
        tileAnimations[tileAnimationCount] = animation(&worldSprite, s, e, spd);
        animationToIDMap[tileAnimationCount] = specialTileID;
        tiles[specialTileID]->animationIndexID = tileAnimationCount;
        ++tileAnimationCount;
    }
    void removeTileAnimation(unsigned int tileAnimationIndex)
    {
        for (int i = tileAnimationIndex; i < tileAnimationCount - 1; ++i)
        {
            animationToIDMap[i] = animationToIDMap[i + 1];
            tileAnimations[i] = tileAnimations[i + 1];
            --tileAnimationCount;
        }
    }

    void updateTileTextures(object &worldObject)
    {
        glm::vec2 textureTranslations[roomHeight * roomWidth];
        int index = 0;
        for (int x = 0; x < roomWidth; ++x)
        {
            for (int y = 0; y < roomHeight; ++y)
            {
                for (int w = 0; w < tileAnimationCount; ++w)
                {
                    if (tileAnimations[w]._sprite == nullptr)
                        continue;

                    if (animationToIDMap[w] == tiles[x][y].specialTileID)
                    {
                        tiles[x][y].id = tileAnimations[w].frame;
                    }
                }

                textureTranslations[index].x = tiles[x][y].id % worldSprite.framesX;
                textureTranslations[index].y = tiles[x][y].id / worldSprite.framesX;

                ++index;
            }
        }

        worldObject.setInstances(index, nullptr, textureTranslations);
    }
    void updateTileColors(object &worldObject)
    {
        glm::vec4 colors[roomHeight * roomWidth];
        int index = 0;
        for (int x = 0; x < roomWidth; ++x)
        {
            for (int y = 0; y < roomHeight; ++y)
            {
                if (tiles[x][y].colorIndexID == -1)
                {
                    colors[index] = glm::vec4(1.0);
                }
                if (tiles[x][y].colorIndexID > -1)
                {
                    colors[index] = tileColors[tiles[x][y].colorIndexID];
                }

                ++index;
            }
        }

        worldObject.setInstances(index, nullptr, nullptr, colors);
        tileColorsNeedUpdate = false;
    }
};

#endif