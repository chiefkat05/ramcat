#ifndef SYSTEM_H
#define SYSTEM_H

#include <iostream>
#include <string>
#include "collision.h"
#include "dungeon.h"
// #include "effects.h"
#include "miniaudio.h"

const unsigned int entity_limit = 64; // also change in dungeon.h since there's another one there, also delete one of these so that there's only one pls
const unsigned int character_limit = 6;
const unsigned int animation_limit = 24;
const unsigned int sound_limit = 32;

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

struct character
{
    float velocityX = 0.0f, velocityY = 0.0f;
    bool onGround = false, jumped = false;
    bool isAPlayer = false;

    animation animations[animation_limit];
    // float posX = 0.0f, posY = 0.0f;
    // float walkToX = 0.0f, walkToY = 0.0f;
    sprite visual;
    aabb collider;

    IDENTIFICATION id = CH_MONSTER;
    float attackTimer = 0.0f;
    int hp = 10, maxhp = 10;
    float runSpeed = 3.0f;

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
    // particlesystem *particles[particle_system_limit];
    // int particlesystemcount;
    int level = 0;
    bool levelincreasing = false;

    int characterCount = 0;
    bool paused = false;
    ma_result game_sound_result;
    ma_sound game_sounds[sound_limit];
    bool music_playing = false;

    void Add(character *e);
    // void Add(particlesystem *p);

    // void initSoundEngine();
    // void handleMusic();
    void loopSound(unsigned int id);
    void initSound(const char *path, unsigned int id, ma_engine *engine);
    void playSound(unsigned int id, float volume, int start_time);
    void stopSound(unsigned int id);
    // void uninitMusic();

    void update(dungeon &floor, float delta_time);

    // void killParticles();
};

#endif