#ifndef SYSTEM_H
#define SYSTEM_H

#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include "collision.h"
#include "world.h"
#include "effects.h"
#include "miniaudio.h"

enum game_state
{
    START_SCREEN,
    MENU_SCREEN,
    CHARACTER_CREATION_SCREEN,
    WORLD_SCREEN,
    WON_LEVEL_STATE,
    COFFEE_MUG_DEATH_STATE,
    LOSE_SCREEN,
    WIN_SCREEN,
    state_total_count
};

const unsigned int character_limit = 64; // also change in world.h since there's another one there, also delete one of these so that there's only one pls
const unsigned int animation_limit = 24;
const unsigned int sound_limit = 26;
const unsigned int sound_is_music_cutoff = 2;
const unsigned int window_width = 1280;
const unsigned int window_height = 720;
const unsigned int player_limit = 12;

const unsigned int light_limit = 24; // don't forget to change in fragment shader if you change this

struct character;

enum IDENTIFICATION
{
    CH_NULL,
    CH_PLAYER,
    ch_monster_ids,
    CH_GULK,
    CH_TERROR,
    CH_BLAUNCH,
    CH_WIGHT,
    CH_CROCC,
    CH_TOMMY,
    CH_BINK,
    CH_MIMIC,
    ch_npc_ids,
    CH_COFFEEMUGGUY,
    ch_other_ids
};

enum ANIMATION_MAPPINGS
{
    ANIM_IDLE,
    ANIM_WALK,
    ANIM_HURT,
    ANIM_DEAD,
    ANIM_ABILITY_0, // continue with only specials, place hardcoded animations before the special animations (which are for custom scenarios)
    ANIM_ABILITY_1,
    ANIM_ABILITY_2,
    ANIM_ABILITY_3,
    ANIM_ABILITY_4,
    ANIM_ABILITY_5,
    ANIM_ABILITY_6,
    ANIM_ABILITY_7,
    ANIM_ABILITY_8
};

enum controlset
{
    CONTROL_UP,
    CONTROL_LEFT,
    CONTROL_RIGHT,
    CONTROL_DOWN,
    CONTROL_SHIELD,
    CONTROL_SWORD,
    CONTROL_BUBBLE,
    CONTROL_SPAWN_PLAYER,
    control_limit
};

enum GAMEPAD_MAP
{
    PAD_BUTTON_A,
    PAD_BUTTON_B,
    PAD_BUTTON_X,
    PAD_BUTTON_Y,
    PAD_BUTTON_L,
    PAD_BUTTON_R,
    PAD_SELECT,
    PAD_START,
    PAD_HOME,
    PAD_THUMB_L,
    PAD_THUMB_R,
    PAD_DPAD_UP,
    PAD_DPAD_RIGHT,
    PAD_DPAD_DOWN,
    PAD_DPAD_LEFT,

    PAD_seperation_value,

    PAD_LSTICK_LEFT,
    PAD_LSTICK_RIGHT,
    PAD_LSTICK_UP,
    PAD_LSTICK_DOWN,
    PAD_RSTICK_LEFT,
    PAD_RSTICK_RIGHT,
    PAD_RSTICK_UP,
    PAD_RSTICK_DOWN,
    PAD_TRIGGER_L,
    PAD_TRIGGER_R
};

// enum for game-specific sound targets

struct player
{
    int inputs[control_limit] = {265, 263, 262, 264, 44, 46, GLFW_KEY_BACKSLASH, GLFW_KEY_1};
    GAMEPAD_MAP gamepad_inputs[control_limit] = {PAD_BUTTON_X, PAD_DPAD_LEFT, PAD_DPAD_RIGHT,
                                                 PAD_DPAD_DOWN, PAD_BUTTON_A, PAD_BUTTON_B, PAD_TRIGGER_R,
                                                 PAD_SELECT};
    int gamepad_id = -1;

    bool getInput(GLFWwindow *window, controlset action);
};

// this is simply for clarity in the code, please change according to what purpose each collision box serves in the character collider lists.
enum COLLIDER_BOX_IDS
{
    COLLIDER_SOLID,
    COLLIDER_STRIKE,
    character_collider_limit
};
struct character
{
    double velocityX = 0.0, velocityY = 0.0;
    bool onGround = false, jumped = false;
    player *plControl = nullptr;

    // everything below should be in the player struct
    float parryCooloff = 20, parryTimer = 0, parryWindow = 5;
    float strikeCooloff = 40, strikeTimer = 0, strikeWindow = 5;
    bool parrySuccess = false, striking;
    bool parryButtonPressed = false, strikeButtonPressed = false;
    bool walkingkeypressed = false;
    bool stepsoundplayed = false;
    // everything above should be in the player struct

    animation animations[animation_limit];
    void SetAnimation(ANIMATION_MAPPINGS id, unsigned int s, unsigned int e, double spd);
    void PlayAnimation(ANIMATION_MAPPINGS id, double delta_time, bool loops);
    void StopAnimation(ANIMATION_MAPPINGS id);

    sprite visual;

    aabb colliders[character_collider_limit];
    void setCollider(COLLIDER_BOX_IDS id, aabb newBox, double offsetX, double offsetY)
    {
        colliders[id] = newBox;

        colliders[id].xOffsetFromParent = offsetX;
        colliders[id].yOffsetFromParent = offsetY;

        colliders[id].parent_object = this;
    }
    void putCollider(COLLIDER_BOX_IDS id, double x, double y)
    {
        colliders[id].Put(x, y);
    }
    void scaleCollider(COLLIDER_BOX_IDS id, double w, double h)
    {
        colliders[id].Scale(w, h);
    }
    void colliderOn(COLLIDER_BOX_IDS id)
    {
        colliders[id].collisionID = id;
    }
    void colliderOff(COLLIDER_BOX_IDS id)
    {
        colliders[id].collisionID = -1;
    }

    IDENTIFICATION id = CH_NULL;
    int hp = 4, maxhp = 4;
    double runSpeed = 120.0f;

    bool animationFinished = true, animationLooping = false;
    ANIMATION_MAPPINGS playingAnim = ANIM_IDLE;

    unsigned int initiative = 0;

    character();
    character(sprite &v, IDENTIFICATION _id);
    character(shader *program, object *sprite_object, std::string filepath, double x, double y, double z, unsigned int fx, unsigned int fy, IDENTIFICATION _id);

    void MoveTo(double _x, double _y, world *currentWorld);

    void Update(double delta_time);
    void updatePosition(double delta_time);
};

struct game_system;

struct aabb_quadtree
{
    aabb_quadtree *nw = nullptr;
    aabb_quadtree *ne = nullptr;
    aabb_quadtree *sw = nullptr;
    aabb_quadtree *se = nullptr;

    bool hasChildren = false;

    aabb *linked_list; // make linked list? or array or vector we figure that out easy easy ya then relax
    aabb bounds;

    aabb_quadtree()
    {
        bounds = aabb(0.0, 0.0, 0.0, 0.0);
        linked_list = nullptr;
    }
    aabb_quadtree(aabb _b)
    {
        bounds = _b;
        linked_list = nullptr;
    }
    // ~aabb_quadtree() // I have resolved not to touch this for the time being as every deletion function I try to make causes a crash on game exit
    // {
    //     empty();
    // }
    // I will return to the above when I have learned a thing or two about deletion functions with nested pointers and all that fun stuff
    void empty()
    {
        delete nw;
        delete ne;
        delete sw;
        delete se;
        linked_list = nullptr;
        hasChildren = false;
    }

    sprite visual;
    void setSprite(shader *s, object *o)
    {
        visual = sprite(s, o, "./img/debug.png");
        visual.Put(bounds.min_x, bounds.min_y, 8.0);
        visual.Scale(bounds.max_x - bounds.min_x, bounds.max_y - bounds.min_y, 1.0);
    }

    void draw()
    {
        visual.Draw(true);

        if (nw != nullptr)
        {
            nw->setSprite(visual.shaderP, visual.objectP);
            nw->draw();
        }
        if (ne != nullptr)
        {
            ne->setSprite(visual.shaderP, visual.objectP);
            ne->draw();
        }
        if (sw != nullptr)
        {
            sw->setSprite(visual.shaderP, visual.objectP);
            sw->draw();
        }
        if (se != nullptr)
        {
            se->setSprite(visual.shaderP, visual.objectP);
            se->draw();
        }
    }
    void insert(aabb *_i, aabb workingBox)
    {
        if (std::abs(workingBox.centerX() - bounds.centerX()) < (workingBox.max_x - workingBox.min_x) || std::abs(workingBox.centerY() - bounds.centerY()) < (workingBox.max_y - workingBox.min_y))
        {
            _i->next_aabb = linked_list;
            linked_list = _i;
            return;
        }
        if (!bounds.colliding(*_i))
        {
            return;
        }

        if (workingBox.max_x < bounds.centerX() && workingBox.min_y > bounds.centerY())
        {
            if (nw == nullptr)
                nw = new aabb_quadtree(aabb(bounds.min_x, bounds.centerY(), bounds.centerX(), bounds.max_y));

            nw->insert(_i, workingBox);
        }
        if (workingBox.min_x > bounds.centerX() && workingBox.min_y > bounds.centerY())
        {
            if (ne == nullptr)
                ne = new aabb_quadtree(aabb(bounds.centerX(), bounds.centerY(), bounds.max_x, bounds.max_y));

            ne->insert(_i, workingBox);
        }
        if (workingBox.max_x < bounds.centerX() && workingBox.max_y < bounds.centerY())
        {
            if (sw == nullptr)
                sw = new aabb_quadtree(aabb(bounds.min_x, bounds.min_y, bounds.centerX(), bounds.centerY()));

            sw->insert(_i, workingBox);
        }
        if (workingBox.min_x > bounds.centerX() && workingBox.max_y < bounds.centerY())
        {
            if (se == nullptr)
                se = new aabb_quadtree(aabb(bounds.centerX(), bounds.min_y, bounds.max_x, bounds.centerY()));

            se->insert(_i, workingBox);
        }

        hasChildren = true;
    }

    void handle_collisions(game_system &game, world &floor, double delta_time);
};

enum validCollisionType
{
    VCT_INVALID,
    VCT_SOLID_SOLID,
    VCT_STRIKE_SOLID,
    VCT_SOLID_STRIKE
};
enum game_objectlist
{
    GAME_OBJECT_DEFAULT,
    GAME_OBJECT_PARTICLE,
    GAME_OBJECT_TILEMAP,
    GAME_OBJECT_TEXT,
    GAME_OBJECT_GUI,
    object_limit
};
enum game_shaderlist
{
    GAME_SHADER_DEFAULT,
    GAME_SHADER_TEXT,
    GAME_SHADER_UI,
    shader_limit
};

struct game_system
{
    game_state state, nextState;

    object *objects[object_limit];
    shader *shaders[shader_limit];

    character characters[character_limit];
    int characterCount = 0;
    aabb_quadtree collision_tree;

    sprite *sortedSprites[character_limit];
    particlesystem particles[particle_system_limit];
    int particlesystemcount;
    bool particlesenabled = true;
    int level = 0;
    bool levelincreasing = false;

    bool paused = false;
    ma_result game_sound_result;
    ma_sound game_sounds[sound_limit];
    std::string sound_paths[sound_limit];
    bool music_playing = false;

    int music_volume = 100, sound_volume = 100;
    int fishCollected = 0, fishNeeded = 5;

    light light_list[light_limit];
    unsigned int light_count = 0;

    void addLight(light l)
    {
        light_list[light_count] = l;
        ++light_count;
    }
    void removeLight(light l)
    {
        int index = -1;
        for (int i = 0; i < light_count; ++i)
        {
            if (l.position == light_list[i].position)
            {
                index = i;
            }
        }
        if (index == -1)
        {
            std::cout << "\n\twarning: light doesn't exist at position " << l.position.x << ", " << l.position.y << ", " << l.position.z << "\n";
            return;
        }

        for (int i = index; i < light_count - 1; ++i)
        {
            light_list[i] = light_list[i + 1];
        }
    }
    void clearLights()
    {
        light_count = 0;
    }
    constexpr light *lastLight()
    {
        return &light_list[light_count - 1];
    }

    game_system()
    {
    }
    ~game_system()
    {
        // delete pointers inside arrays here?
        for (int i = 0; i < object_limit; ++i)
        {
            delete objects[i];
        }
        for (int i = 0; i < shader_limit; ++i)
        {
            delete shaders[i];
        }
    }

    void Add(character e);
    void Remove(int index);
    void ClearEnemies();

    void setParticles(std::string path, unsigned int fx, unsigned int fy, unsigned int _particle_count, double _life_lower, double _life_upper,
                      double sX, double sY, double sW, double sH, unsigned int uniqueID);
    particlesystem *lastParticleSet()
    {
        return &particles[particlesystemcount - 1];
    }
    particlesystem *particleByID(unsigned int id)
    {
        for (int i = 0; i < particle_system_limit; ++i)
        {
            if (particles[i].id == id)
                return &particles[i];
        }
        return nullptr;
    }
    void removeParticles(unsigned int index);

    // void initSoundEngine();
    // void handleMusic();
    void loopSound(unsigned int id);
    void initSound(std::string path, unsigned int id, ma_engine *engine);
    void playSound(unsigned int id, int start_time, bool interrupt = false);
    void stopSound(unsigned int id);
    // void uninitMusic();

    void particle_update(double delta_time);
    void light_update();
    void update(world &floor, camera &mainCam, double delta_time);
    void handleCharacterCollisions(character &charA, character &charB, validCollisionType collisionType, double xNormal, double yNormal, double colValue, double delta_time)
    {
        switch (charA.id)
        {
        case CH_PLAYER:
            goto normal_collision;
        case CH_COFFEEMUGGUY:
            if (charB.id == CH_PLAYER)
            {
                if (collisionType == VCT_SOLID_SOLID)
                {
                    charA.PlayAnimation(ANIM_ABILITY_0, delta_time, false);
                }
                if (collisionType == VCT_SOLID_STRIKE)
                {
                    state = COFFEE_MUG_DEATH_STATE;
                }
            }
            goto normal_collision;
        case CH_GULK:
            if (charB.id == CH_PLAYER)
            {
                if (collisionType == VCT_SOLID_STRIKE)
                {
                    charA.hp = 0;
                }
            }
            goto normal_collision;
        default:
        normal_collision: // cursed? maybe.
            if (collisionType == VCT_SOLID_SOLID)
            {
                if (xNormal > 0.0 && charA.velocityX < 0.0)
                {
                    charA.visual.x = colValue;
                    charA.velocityX = 0.0;
                }
                if (xNormal < 0.0 && charA.velocityX > 0.0)
                {
                    charA.visual.x = colValue;
                    charA.velocityX = 0.0;
                }
                if (yNormal > 0.0 && charA.velocityY < 0.0)
                {
                    charA.visual.y = colValue;
                    charA.velocityY = 0.0;
                    charA.velocityX += charB.velocityX;
                    charA.onGround = true;
                }
                if (yNormal < 0.0 && charA.velocityY > 0.0)
                {
                    charA.visual.y = colValue;
                    charA.velocityY = 0.0;
                }
            }
            break;
        }
    }
    void handleTileCollisions(character *c, aabb *t, world &floor, double &delta_time)
    {
        if (t->min_x == 0.0 &&
                t->min_y == 0.0 &&
                t->max_x == 0.0 &&
                t->max_y == 0.0 ||
            t->collisionID <= -1)
        {
            return;
        }

        double xNormal = 0.0, yNormal = 0.0, distanceToClosestSide = 0.0;
        bool collision = false; // now for taking these apart into more managable functions!
        // something like
        // bool walkableInterior = (distanceToClosestSide >= -0.01);
        // and
        // void respondCollision(int collisionID, bool walkableInterior = true, bool groundBlock = true, replaceOnSpawnWith(GULK));

        double firstCollisionHitTest = c->colliders[COLLIDER_SOLID].response(c->velocityX * pixel_scale * delta_time,
                                                                             c->velocityY * pixel_scale * delta_time, 0.0, 0.0,
                                                                             *t, xNormal, yNormal, c->visual.x,
                                                                             c->visual.y, collision, distanceToClosestSide);

        tile *thisSpecialTile = &floor.tiles[t->specialTileX][t->specialTileY];
        switch (t->collisionID)
        {
        case -1:
            break;
        case 1:
            if (collision)
            {
                if (xNormal != 0.0)
                {
                    c->visual.x = firstCollisionHitTest;
                    c->velocityX = 0.0;
                }
                if (yNormal != 0.0)
                {
                    c->hp = 0;
                }
            }
            break;
        case 2:
            if (collision && c->plControl != nullptr)
            {
                levelincreasing = true;
            }
            break;
        case 3:
            if (collision && yNormal > 0.0 && distanceToClosestSide >= -0.01 && c->velocityY < 0.0)
            {
                c->visual.y = firstCollisionHitTest;
                c->velocityY = 0.0;
                c->onGround = true;
            }
            break;
        case 4:
            if (collision && xNormal < 0.0 && distanceToClosestSide >= -0.01 && c->velocityX > 0.0)
            {
                c->visual.x = firstCollisionHitTest;
                c->velocityX = 0.0;
            }
            break;
        case 5:
            if (collision && yNormal < 0.0 && distanceToClosestSide >= -0.01 && c->velocityY > 0.0)
            {
                c->visual.y = firstCollisionHitTest;
                c->velocityY = 0.0;
            }
            break;
        case 6:
            if (collision && xNormal > 0.0 && distanceToClosestSide >= -0.01 && c->velocityX < 0.0)
            {
                c->visual.x = firstCollisionHitTest;
                c->velocityX = 0.0;
            }
            break;
        case 7:
            if (collision && yNormal != 0.0 && c->velocityY < 0.0)
            {
                c->velocityY = 4.0;
            }
            break;
        case 8:
            if (collision)
            {
                floor.removeTileAnimation(thisSpecialTile->animationIndexID);
                thisSpecialTile->emptyTile();
                t->collisionID = -1;
                ++fishCollected;
                thisSpecialTile->colorIndexID = 0;
                floor.tileColorsNeedUpdate = true;
            }
            break;
        case 9:
            if (collision)
            {
                thisSpecialTile->id = 2;
                thisSpecialTile->collisionID = -1;
                t->collisionID = -1;

                floor.spawnLocationX = t->specialTileX * floor.worldSprite.trueW();
                floor.spawnLocationY = (-static_cast<double>(floor.roomHeight) + t->specialTileY) * floor.worldSprite.trueW();
                // floor.spawnLocationX = t->centerX();
                // floor.spawnLocationY = t->centerY();
            }
            break;
        case 10:
            if (collision)
            {
                floor.removeTileAnimation(thisSpecialTile->animationIndexID);
                thisSpecialTile->emptyTile();
                t->collisionID = -1;
                thisSpecialTile->colorIndexID = 0;
                floor.tileColorsNeedUpdate = true;
            }
            break;
        case 11:
        {
            thisSpecialTile->id = -1;
            thisSpecialTile->collisionID = -1;
            t->collisionID = -1;

            // Add(character("./img/char/gulk.png", t->min_x * floor.worldSprite.trueW(),
            //               t->min_y * floor.worldSprite.trueH() + 0.2, 4, 1, CH_GULK));
            Add(character(shaders[GAME_SHADER_DEFAULT], objects[GAME_OBJECT_DEFAULT], "./img/char/gulk.png", t->min_x * floor.worldSprite.trueW(),
                          t->centerY() * floor.worldSprite.trueH() + 0.2, 0.5, 4, 1, CH_GULK));
            characters[characterCount - 1].setCollider(COLLIDER_SOLID, aabb(characters[characterCount - 1].visual.x, characters[characterCount - 1].visual.y, characters[characterCount - 1].visual.x + 0.16, characters[characterCount - 1].visual.y + 0.24), 0.0, 0.0);
            characters[characterCount - 1].colliderOn(COLLIDER_SOLID);
            characters[characterCount - 1].scaleCollider(COLLIDER_STRIKE, 0.32, 0.16);
        }
        break;
        case 12:
        {
            // tile *npcTile = floor.getTileFromCollisionSpecialID(t->specialTileID);
            thisSpecialTile->id = -1;
            thisSpecialTile->collisionID = -1;
            t->collisionID = -1;
            Add(character(shaders[GAME_SHADER_DEFAULT], objects[GAME_OBJECT_DEFAULT], "./img/char/coffeemugguy.png", t->min_x * floor.worldSprite.trueW(),
                          t->min_y * floor.worldSprite.trueH() + 0.2, 0.5, 5, 1, CH_COFFEEMUGGUY));
            // Add(character("./img/char/coffeemugguy.png", floor.collision_boxes[j].min_x * floor.worldSprite.trueW(),
            //               floor.collision_boxes[j].centerY() * floor.worldSprite.trueH() + 0.2, 5, 1, CH_COFFEEMUGGUY)); FIX
            characters[characterCount - 1].setCollider(COLLIDER_SOLID, aabb(characters[characterCount - 1].visual.x, characters[characterCount - 1].visual.y, characters[characterCount - 1].visual.x + 0.16, characters[characterCount - 1].visual.y + 0.24), 0.0, 0.0);
            characters[characterCount - 1].SetAnimation(ANIM_ABILITY_0, 2, 2, 0.0);
            characters[characterCount - 1].colliderOn(COLLIDER_SOLID);
        }
        break;
        default:
            if (collision)
            {
                if (xNormal != 0.0)
                {
                    c->visual.x = firstCollisionHitTest;
                    c->velocityX = 0.0;
                }
                if (yNormal != 0.0)
                {
                    c->visual.y = firstCollisionHitTest;
                    c->velocityY = 0.0;
                }
                if (yNormal > 0.0)
                    c->onGround = true;
            }
            break;
        }

        if (collision && std::abs(c->velocityX) < 0.5 && xNormal > 0.0 && c->id == CH_GULK)
        {
            c->velocityX = 0.5;
        }
        if (collision && std::abs(c->velocityX) < 0.5 && xNormal < 0.0 && c->id == CH_GULK)
        {
            c->velocityX = -0.5;
        }
    }

    void killParticles();
};

#endif