#ifndef SYSTEM_H
#define SYSTEM_H

#include <iostream>
#include <string>
#include "collision.h"
#include "dungeon.h"
#include "effects.h"
#include "miniaudio.h"

const unsigned int entity_limit = 64; // also change in dungeon.h since there's another one there, also delete one of these so that there's only one pls
const unsigned int character_limit = 6;
const unsigned int animation_limit = 24;
const unsigned int sound_limit = 32;
const unsigned int window_width = 1280;
const unsigned int window_height = 720;
const unsigned int player_limit = 12;

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
    PAD_DPAD_LEFT,
    PAD_DPAD_RIGHT,
    PAD_DPAD_UP,
    PAD_DPAD_DOWN,
    PAD_START,
    PAD_SELECT,
    PAD_STICK_LEFT,
    PAD_STICK_RIGHT,
    PAD_STICK_UP,
    PAD_STICK_DOWN,
    PAD_BUTTON_R,
    PAD_BUTTON_L,
    PAD_TRIGGER_R,
    PAD_TRIGGER_L,
    PAD_RSTICK_LEFT,
    PAD_RSTICK_RIGHT,
    PAD_RSTICK_UP,
    PAD_RSTICK_DOWN
};

struct player
{
    int inputs[control_limit] = {265, 263, 262, 264, 44, 46, GLFW_KEY_BACKSLASH, GLFW_KEY_1};
    int gamepad_inputs[control_limit] = {GLFW_GAMEPAD_BUTTON_X, GLFW_GAMEPAD_BUTTON_DPAD_LEFT, GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,
                                         GLFW_GAMEPAD_BUTTON_DPAD_DOWN, GLFW_GAMEPAD_BUTTON_A, GLFW_GAMEPAD_BUTTON_B, GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,
                                         GLFW_GAMEPAD_BUTTON_BACK};
    int gamepad_id = -1;
    // int p2inputs[control_limit] = {87, 65, 68, 83, 86, 67};

    bool getInput(GLFWwindow *window, controlset action);
};

struct character
{
    float velocityX = 0.0f, velocityY = 0.0f;
    bool onGround = false, jumped = false;
    player *plControl;
    int parryCooloff = 23, parryTimer = 0, parryWindow = 5;
    bool parrySuccess = false;

    animation animations[animation_limit];
    // float posX = 0.0f, posY = 0.0f;
    // float walkToX = 0.0f, walkToY = 0.0f;
    sprite visual;
    aabb collider;

    IDENTIFICATION id = CH_MONSTER;
    float attackTimer = 0.0f;
    int hp = 10, maxhp = 10;
    float runSpeed = 1.7f;

    bool animationFinished = true, animationLooping = false;
    ANIMATION_MAPPINGS playingAnim = ANIM_IDLE;

    unsigned int initiative = 0;

    character();
    character(sprite &v, IDENTIFICATION _id);
    character(object *spriteObject, std::string filepath, float x, float y, unsigned int fx, unsigned int fy, IDENTIFICATION _id);

    void MoveTo(float _x, float _y, dungeon *currentDungeon);

    void Update(float delta_time);
    void updatePosition(float delta_time);

    void SetAnimation(ANIMATION_MAPPINGS id, unsigned int s, unsigned int e, float spd);
    void PlayAnimation(ANIMATION_MAPPINGS id, float delta_time, bool loops);
    void StopAnimation(ANIMATION_MAPPINGS id);
};

struct game_system
{
    character *characters[entity_limit];
    sprite *sortedSprites[entity_limit];
    particlesystem *particles[particle_system_limit];
    int particlesystemcount;
    int level = 0;
    bool levelincreasing = false;

    int characterCount = 0;
    bool paused = false;
    ma_result game_sound_result;
    ma_sound game_sounds[sound_limit];
    bool music_playing = false;

    void Add(character *e);
    void Add(particlesystem *p);

    // void initSoundEngine();
    // void handleMusic();
    void loopSound(unsigned int id);
    void initSound(const char *path, unsigned int id, ma_engine *engine);
    void playSound(unsigned int id, float volume, int start_time);
    void stopSound(unsigned int id);
    // void uninitMusic();

    void update(dungeon &floor, float delta_time);

    void killParticles();
};

#endif