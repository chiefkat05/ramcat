#include "../headers/world.h"

// #define DEBUG_COLLISIONS

world::world()
{
    worldInitialized = false;
}
world::world(const char *_tileSetPath, unsigned int _fx, unsigned int _fy, object_type obj)
{
    tileSetPath = _tileSetPath;
    worldSprite = sprite(tileSetPath, _fx, _fy);
    worldInitialized = true;
    for (unsigned int x = 0; x < width_limit; ++x)
    {
        for (unsigned int y = 0; y < height_limit; ++y)
        {
            tiles[x][y].id = -1;
        }
    }
    // the pointer to the ui_element's visual object is incorrect inside the element's update function
    // what does this mean

    worldObject = object(obj);
}

void world::draw(GLFWwindow *win, shader &program)
{
    // worldSprite.Put(1.0, 1.0, 0.0);
    worldSprite.Draw(program, worldObject);
}

void world::readRoomFile(const char *path)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        std::cout << "Failed to open " << path << "\n";
        return;
    }

    std::string line;
    roomHeight = 0;

    for (int i = 0; i < width_limit; ++i)
    {
        for (int j = 0; j < height_limit; ++j)
        {
            tiles[i][j].id = -1;
        }
    }

    int uTileIDIncrement = 0;
    while (std::getline(file, line))
    {
        if (line.size() > roomWidth)
            roomWidth = line.size();

        unsigned int widthLim = std::min(static_cast<unsigned int>(line.size()), width_limit);
        for (unsigned int i = 0; i < widthLim; ++i)
        {
            switch (line[i])
            {
            case '0':
                tiles[i][roomHeight].id = -1;
                tiles[i][roomHeight].collisionID = -1;
                break;
            case '1':
                tiles[i][roomHeight].id = 0;
                tiles[i][roomHeight].collisionID = 0;
                break;
            case 's':
                tiles[i][roomHeight].id = 2;

                spawnLocationX = i * 0.16f;
                spawnLocationY = roomHeight;
                tiles[i][roomHeight].collisionID = -1;
                break;
            case 'S':
                tiles[i][roomHeight].id = 21;
                tiles[i][roomHeight].collisionID = 9;
                tiles[i][roomHeight].specialTileID = ++uTileIDIncrement;
                break;
            case 'c':
                tiles[i][roomHeight].id = 25;
                tiles[i][roomHeight].collisionID = 10;
                tiles[i][roomHeight].specialTileID = ++uTileIDIncrement;
                break;
            case 'e':
                tiles[i][roomHeight].id = 3;
                tiles[i][roomHeight].collisionID = 2;
                break;
            case 'f':
                tiles[i][roomHeight].id = 27;
                tiles[i][roomHeight].collisionID = 8;
                tiles[i][roomHeight].specialTileID = ++uTileIDIncrement;
                break;
            case '2':
                tiles[i][roomHeight].id = 1;
                tiles[i][roomHeight].collisionID = 1;
                break;
            case '3':
                tiles[i][roomHeight].id = 12;
                tiles[i][roomHeight].collisionID = 3;
                break;
            case '4':
                tiles[i][roomHeight].id = 13;
                tiles[i][roomHeight].collisionID = 4;
                break;
            case '5':
                tiles[i][roomHeight].id = 14;
                tiles[i][roomHeight].collisionID = 5;
                break;
            case '6':
                tiles[i][roomHeight].id = 15;
                tiles[i][roomHeight].collisionID = 6;
                break;
            case 'r':
                tiles[i][roomHeight].id = 8;
                tiles[i][roomHeight].collisionID = 7;
                tiles[i][roomHeight].specialTileID = ++uTileIDIncrement;
                break;
            case '!':
                tiles[i][roomHeight].id = 4;
                tiles[i][roomHeight].collisionID = 11;
                tiles[i][roomHeight].specialTileID = ++uTileIDIncrement;
                break;
            case 'C':
                tiles[i][roomHeight].id = 5;
                tiles[i][roomHeight].collisionID = 12;
                tiles[i][roomHeight].specialTileID = ++uTileIDIncrement;
                break;
            default:
                tiles[i][roomHeight].id = -1;
                tiles[i][roomHeight].collisionID = -1;
                break;
            }
        }

        ++roomHeight;
    }
    // add boolean to check if spawn exists pls
    spawnLocationY = -0.2f + (-static_cast<double>(roomHeight) + spawnLocationY) * 0.16f;
    // spawnLocationY = (-static_cast<double>(roomHeight) + spawnLocationY) * 0.16f;

    file.close();

    // it's not as good as it could be, but I love this thing
    int collisionstartx = -1, collisionstarty = -1, collisionendx = -1, collisionendy = -1;
    int lookXLimit = roomWidth;
    int lookXMin = 0;
    int lookYMin = 0;
    for (int c = 0; c < collision_id_limit; ++c)
    {
        for (int y = lookYMin; y < roomHeight; ++y)
        {
            for (int x = lookXMin; x < lookXLimit; ++x)
            {
                if (tiles[x][y].collisionID == c && collisionstartx == -1 && tiles[x][y].specialTileID > -1 && !tiles[x][y].collisionTaken)
                {
                    collisionstartx = x;
                    collisionstarty = y;
                    collisionendx = x + 1;
                    collisionendy = y + 1;
                    tiles[x][y].collisionTaken = true;
                    goto collisionAddition; // I think this is correct usage
                }
                // if current spot is valid for collision and collision box not started yet, start collision box
                if (tiles[x][y].collisionID == c && !tiles[x][y].collisionTaken && collisionstartx == -1)
                {
                    collisionstartx = x;
                    collisionstarty = y;
                    lookXMin = collisionstartx;
                    lookYMin = collisionstarty;
#ifdef DEBUG_COLLISIONS
                    std::cout << x << ", " << y << " -- start\n";
#endif
                }

                // if current spot is valid for collision and collision x not ended yet or x position is within start and end x bounds (when y is different)
                if (tiles[x][y].collisionID == c && !tiles[x][y].collisionTaken && (collisionendx == -1 || x >= collisionstartx && x < collisionendx))
                {
                    tiles[x][y].collisionTaken = true;
                }

                // if collision x started and collision x not ended and x hits the end or the next tile lacks collision or the next tile is part of a
                // different box, set end x to next tile
                if (collisionstartx != -1 && collisionendx == -1 && (x + 1 > roomWidth || tiles[x + 1][y].collisionID != c || tiles[x + 1][y].collisionTaken))
                {
#ifdef DEBUG_COLLISIONS
                    std::cout << x << " found x\n";
#endif
                    collisionendx = x + 1;
                    lookXLimit = collisionendx;
                }

                // if collision x started and end x found and current y is not start y
                // and current x is between box bounds (or endx is not found) and either next y has no collision
                // or is part of another box, set end y to next y tile

#ifdef DEBUG_COLLISIONS
                if (collisionstartx != -1 && collisionendy == -1 && collisionendx == -1)
                {
                    std::cout << tiles[x][y + 1].collisionID << ", " << tiles[x][y + 1].collisionTaken << " test\n";
                }
#endif
                if (collisionstartx != -1 && collisionendy == -1 &&
                    (collisionendx != -1 && x >= collisionstartx && x < collisionendx &&
                         (tiles[x][y + 1].collisionID != c || tiles[x][y + 1].collisionTaken) ||
                     y + 1 == roomHeight || collisionendx == -1 && x >= collisionstartx && (tiles[x][y + 1].collisionID != c || tiles[x][y + 1].collisionTaken)))
                {
#ifdef DEBUG_COLLISIONS
                    std::cout << y << " found y\n";
#endif
                    collisionendy = y + 1;
                }

                // if collision is not being finished in both directions, skip to next tile
                if (collisionendx == -1 || collisionendy == -1)
                {
#ifdef DEBUG_COLLISIONS
                    if (collisionendx != -1 || collisionendy != -1)
                    {
                        std::cout << c << ", " << tiles[x][y + 1].collisionID << " loop\n";
                    }
#endif
                    continue;
                }

#ifdef DEBUG_COLLISIONS
                // std::cout << "xs = " << collisionstartx << ", ys = " << collisionstarty << ", xw = " << collisionendx << ", yw = " << collisionendy << "\n";
                std::cout << "x = " << collisionstartx << "-" << collisionendx << ", y = " << collisionstarty << "-" << collisionendy << ", id = " << c << "\n";
                std::cout << "<---------------------------------------------------------------------->\n";
#endif

            collisionAddition:
                int newystart = static_cast<int>(roomHeight) - collisionstarty + 1;
                int newyend = static_cast<int>(roomHeight) - collisionendy + 1;
                collision_boxes[collision_box_count] = aabb(collisionstartx * (worldSprite.spriteW * 0.02), newyend * (worldSprite.spriteH * 0.02),
                                                            collisionendx * (worldSprite.spriteW * 0.02), newystart * (worldSprite.spriteH * 0.02));

                collision_boxes[collision_box_count].specialTileID = tiles[x][y].specialTileID;
                collision_boxes[collision_box_count].collisionID = c;

                ++collision_box_count;
                if (collision_box_count >= collision_box_limit)
                {
                    std::cerr << "\n\tToo many collision boxes! Try increasing the collision box limit or lowering the number of boxes in your level.\n";
                    std::cerr << "\tfor information, there are currently " << collision_box_count << " collision boxes out of the limit of " << collision_box_limit << ".\n";
                    return;
                }

                x = collisionstartx; // bad practice, make efficient
                y = collisionstarty; // no, it's actually perfect so shut up

                collisionstartx = -1;
                collisionstarty = -1;
                collisionendx = -1;
                collisionendy = -1;

                lookXLimit = roomWidth;
                lookXMin = 0;
                lookYMin = 0;
            }
        }
    }

    // instancing work
    glm::mat4 translations[roomHeight * roomWidth];
    glm::vec2 textureTranslations[roomHeight * roomWidth];
    int index = 0;
    for (int x = 0; x < roomWidth; ++x)
    {
        for (int y = 0; y < roomHeight; ++y)
        {
            if (tiles[x][y].id == -1)
                continue;

            translations[index] = glm::mat4(1.0);
            translations[index] = glm::translate(translations[index], glm::vec3(0.16 * x, 0.16 * (roomHeight - y), 0.0));
            // rotations go here if you add them
            translations[index] = glm::scale(translations[index], glm::vec3(0.16, 0.16, 0.0)); // mke the 0.16s be dynamic and not static numbers

            textureTranslations[index].x = tiles[x][y].id % worldSprite.framesX;
            textureTranslations[index].y = tiles[x][y].id / worldSprite.framesX;

            ++index;
        }
    }

    worldObject.setInstances(index, translations, textureTranslations);
}

tile *world::getTile(unsigned int tileID)
{
    for (int x = 0; x < roomWidth; ++x)
    {
        for (int y = 0; y < roomHeight; ++y)
        {
            if (tiles[x][y].specialTileID == tileID)
            {
                return &tiles[x][y];
            }
        }
    }
    return nullptr;
}
tile *world::getTileFromCollisionSpecialID(unsigned int collisionIndex)
{
    for (int x = 0; x < roomWidth; ++x)
    {
        for (int y = 0; y < roomHeight; ++y)
        {
            if (tiles[x][y].specialTileID == collision_boxes[collisionIndex].specialTileID)
            {
                return &tiles[x][y];
            }
        }
    }
    return nullptr;
}