#ifndef SYSTEM_H
#define SYSTEM_H

#include <iostream>
#include <string>
#include "collision.h"
#include "world.h"
#include "effects.h"
#include "miniaudio.h"

const unsigned int entity_limit = 64; // also change in world.h since there's another one there, also delete one of these so that there's only one pls
const unsigned int animation_limit = 24;
const unsigned int sound_limit = 26;
const unsigned int sound_is_music_cutoff = 2;
const unsigned int window_width = 1280;
const unsigned int window_height = 720;
const unsigned int player_limit = 12;

const int character_quadtree_capacity = 3;

struct character;

enum IDENTIFICATION
{
    CH_PLAYER,
    CH_MONSTER,
    CH_NPC
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

struct character
{
    double velocityX = 0.0, velocityY = 0.0;
    bool onGround = false, jumped = false;
    player *plControl;

    // everything below should be in the player struct
    float parryCooloff = 20, parryTimer = 0, parryWindow = 5;
    float strikeCooloff = 40, strikeTimer = 0, strikeWindow = 5;
    bool parrySuccess = false, striking;
    bool parryButtonPressed = false, strikeButtonPressed = false;
    bool walkingkeypressed = false;
    bool stepsoundplayed = false;
    // everything above should be in the player struct

    animation animations[animation_limit];
    // double posX = 0.0, posY = 0.0;
    // double walkToX = 0.0, walkToY = 0.0;
    sprite visual;
    aabb collider;

    IDENTIFICATION id = CH_MONSTER;
    double attackTimer = 0.0;
    int hp = 10, maxhp = 10;
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

    void SetAnimation(ANIMATION_MAPPINGS id, unsigned int s, unsigned int e, double spd);
    void PlayAnimation(ANIMATION_MAPPINGS id, double delta_time, bool loops);
    void StopAnimation(ANIMATION_MAPPINGS id);
};

struct characterQuadTree
{
    double left, top, right, bottom;

    character *characters[character_quadtree_capacity];

    characterQuadTree *topLeftTree;
    characterQuadTree *topRightTree;
    characterQuadTree *botLeftTree;
    characterQuadTree *botRightTree;

    sprite *visual;

    characterQuadTree();
    characterQuadTree(double l, double t, double r, double b);
    void insert(character *inputChar);
    bool withinBounds(double x, double y);

    void draw(shader &program, object &sprite_object);
};

struct game_system
{
    character *characters[entity_limit];
    sprite *sortedSprites[entity_limit];
    particlesystem particles[particle_system_limit];
    int particlesystemcount;
    int level = 0;
    bool levelincreasing = false;

    int characterCount = 0;
    bool paused = false;
    ma_result game_sound_result;
    ma_sound game_sounds[sound_limit];
    std::string sound_paths[sound_limit];
    bool music_playing = false;

    int music_volume = 100, sound_volume = 100;
    int fishCollected = 0, fishNeeded = 5;

    void Add(character *e);
    // void Add(particlesystem *p);
    void Remove(character *e);
    // void Remove(particlesystem *p);

    void particleSet(particlesystem sys)
    {
        particles[particlesystemcount] = sys;
        ++particlesystemcount;
    }
    particlesystem *lastParticleSet()
    {
        return &particles[particlesystemcount - 1];
    }
    void removeParticles(unsigned int index)
    {
        for (int i = index; i < particlesystemcount; ++i)
        {
            particles[i] = particles[i + 1];
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

    void killParticles();
};

#endif