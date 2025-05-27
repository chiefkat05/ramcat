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

const int quadtree_leaf_capacity = 4;
const int quadtree_node_limit = 12;

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
    void setCollider(COLLIDER_BOX_IDS id, aabb newBox)
    {
        colliders[id] = newBox;
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
    double runSpeed = 1.7f;

    bool animationFinished = true, animationLooping = false;
    ANIMATION_MAPPINGS playingAnim = ANIM_IDLE;

    unsigned int initiative = 0;

    character();
    character(sprite &v, IDENTIFICATION _id);
    character(std::string filepath, double x, double y, unsigned int fx, unsigned int fy, IDENTIFICATION _id);

    void MoveTo(double _x, double _y, world *currentWorld);

    void Update(double delta_time);
    void updatePosition(double delta_time);
};

// struct quadtree_node
// {
//     aabb bounds;
//     quadtree_node *northwest = nullptr;
//     quadtree_node *northeast = nullptr;
//     quadtree_node *southwest = nullptr;
//     quadtree_node *southeast = nullptr;
//     aabb *obj_list;
//     unsigned int obj_count = 0;

//     sprite visual;

//     quadtree_node()
//     {
//         visual = sprite("./img/debug.png", 1, 1, false);
//     }

//     void insert(aabb *input)
//     {
//         if (input->max_x < bounds.min_x || input->min_x > bounds.max_x || input->max_y < bounds.min_y || input->min_y > bounds.max_y)
//             return;

//         bool straddle = false;
//         double xCenter = (bounds.max_x + bounds.min_x) * 0.5;
//         double yCenter = (bounds.max_y + bounds.min_y) * 0.5;

//         if (input->min_x <= xCenter && input->max_x >= xCenter)
//             straddle = true;
//         if (input->min_y <= yCenter && input->max_y >= yCenter)
//             straddle = true;

//         if (straddle)
//         {
//             input->next_obj = obj_list;
//             obj_list = input;
//             return;
//         }

//         if (input->min_x > xCenter && input->min_y > yCenter)
//         {
//             if (northeast == nullptr)
//             {
//                 northeast = new quadtree_node;
//                 northeast->bounds = bounds;
//                 northeast->bounds.min_x = xCenter;
//                 northeast->bounds.min_y = yCenter;
//             }
//             northeast->insert(input);
//         }
//         if (input->max_x < xCenter && input->min_y > yCenter)
//         {
//             if (northwest == nullptr)
//             {
//                 northwest = new quadtree_node;
//                 northwest->bounds = bounds;
//                 northwest->bounds.max_x = xCenter;
//                 northwest->bounds.min_y = yCenter;
//             }
//             northwest->insert(input);
//         }
//         if (input->min_x > xCenter && input->max_y < yCenter)
//         {
//             if (southeast == nullptr)
//             {
//                 southeast = new quadtree_node;
//                 southeast->bounds = bounds;
//                 southeast->bounds.min_x = xCenter;
//                 southeast->bounds.max_y = yCenter;
//             }
//             southeast->insert(input);
//         }
//         if (input->max_x < xCenter && input->max_y < yCenter)
//         {
//             if (southwest == nullptr)
//             {
//                 southwest = new quadtree_node;
//                 southwest->bounds = bounds;
//                 southwest->bounds.max_x = xCenter;
//                 southwest->bounds.max_y = yCenter;
//             }
//             southwest->insert(input);
//         }
//     }

//     void draw(shader &program, object &sprite_obj)
//     {
//         visual.Put((bounds.min_x + bounds.max_x) * 0.5, (bounds.min_y + bounds.max_y) * 0.5, 0.0);
//         visual.Scale(bounds.max_x - bounds.min_x, bounds.max_y - bounds.min_y, 0.0);

//         visual.Draw(program, sprite_obj, true);
//         if (northwest != nullptr)
//             northwest->draw(program, sprite_obj);
//         if (northeast != nullptr)
//             northeast->draw(program, sprite_obj);
//         if (southwest != nullptr)
//             southwest->draw(program, sprite_obj);
//         if (southeast != nullptr)
//             southeast->draw(program, sprite_obj);
//     }

//     void empty()
//     {
//         obj_list = nullptr;
//         obj_count = 0;

//         if (northwest != nullptr)
//             northwest->empty();
//         if (northeast != nullptr)
//             northeast->empty();
//         if (southwest != nullptr)
//             southwest->empty();
//         if (southeast != nullptr)
//             southeast->empty();
//     }
// };

// this is tough
// I'm post-poning the collision detection bounding structure stuff since I can't figure it out right now
// struct aabb_bhv_node
// {
//     aabb bounds;
//     aabb *obj_list_first;
//     const unsigned int min_objects = 3;

//     aabb_bhv_node *children[2];
//     unsigned int child_count = 0;

//     void construct()
// };
enum validCollisionType
{
    VCT_INVALID,
    VCT_SOLID_SOLID,
    VCT_STRIKE_SOLID,
    VCT_SOLID_STRIKE
};

struct game_system
{
    game_state state, nextState;

    character characters[character_limit];
    int characterCount = 0;

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

    void Add(character e);
    // void Spawn_Enemy(std::string filepath, IDENTIFICATION _id, double x, double y, double scaleX, double scaleY, unsigned int fx, unsigned int fy, double cOffX, double cOffY, double cW, double cH);
    // void Spawn_Enemy(character e);
    void Remove(int index);
    void ClearEnemies();

    void setParticles(std::string path, unsigned int fx, unsigned int fy, unsigned int _particle_count, double _life_lower, double _life_upper,
                      double sX, double sY, double sW, double sH, unsigned int uniqueID)
    {
        if (particlesystemcount >= particle_system_limit || !particlesenabled)
            return;

        particles[particlesystemcount].totalParticlesSpawned = 0;

        for (int i = 0; i < particlesystemcount; ++i)
        {
            if (uniqueID == particles[i].id)
            {
                return;
            }
        }

        particles[particlesystemcount].visual.texture_path = path;
        particles[particlesystemcount].visual.framesX = fx;
        particles[particlesystemcount].visual.framesY = fy;
        particles[particlesystemcount].visual.textureInit();

        particles[particlesystemcount].particle_count = _particle_count;
        if (_particle_count >= particle_limit)
            particles[particlesystemcount].particle_count = particle_limit;
        particles[particlesystemcount].variables[PV_LIFE_LOW] = _life_lower;
        particles[particlesystemcount].variables[PV_LIFE_HIGH] = _life_upper;
        particles[particlesystemcount].variables[PV_SPAWN_X] = sX;
        particles[particlesystemcount].variables[PV_SPAWN_Y] = sY;
        particles[particlesystemcount].variables[PV_SPAWN_W] = sW;
        particles[particlesystemcount].variables[PV_SPAWN_H] = sH;

        particles[particlesystemcount].id = uniqueID;
        ++particlesystemcount;
    }
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
    void removeParticles(unsigned int index)
    {
        particles[index] = particlesystem();
        for (int i = 0; i < pv_variable_limit; ++i)
        {
            particles[index].variable_pointers[i] = nullptr;
        }

        for (int i = index; i < particlesystemcount - 1; ++i)
        {
            particles[i] = particles[i + 1];
        }
        particles[particlesystemcount] = particlesystem();
        for (int i = 0; i < pv_variable_limit; ++i)
        {
            particles[particlesystemcount].variable_pointers[i] = nullptr;
        }
        --particlesystemcount;
    }

    // void initSoundEngine();
    // void handleMusic();
    void loopSound(unsigned int id);
    void initSound(std::string path, unsigned int id, ma_engine *engine);
    void playSound(unsigned int id, int start_time, bool interrupt = false);
    void stopSound(unsigned int id);
    // void uninitMusic();

    void update(world &floor, shader &particle_program, object &particle_sprite, double delta_time);
    void handleCollisionSpecifics(character &charA, character &charB, validCollisionType collisionType, double xNormal, double yNormal, double colValue, double delta_time)
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

    void killParticles();
};

#endif