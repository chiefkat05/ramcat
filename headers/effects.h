#ifndef EFFECTS_H
#define EFFECTS_H

#include "sprite.h"
#include <random>

const unsigned int sound_limit = 16;
const unsigned int particle_limit = 2512;
const unsigned int particle_system_limit = 6;

// struct soundhandler
// {
//     sf::SoundBuffer buffer;
//     sf::Sound sound;
//     int preparedSound = 0;

//     void load(const std::string filepath);
//     void play();
// };

struct particle
{
    float x = 0.0f, y = 0.0f, velX = 0.0f, velY = 0.0f, life = 0.0f;
    float lifestartalphamultiple = 1.0f;

    void Put(float _x, float _y)
    {
        x = _x;
        y = _y;
    }
    void Move(float _offx, float _offy)
    {
        x += _offx;
        y += _offy;
    }
};

enum PARTICLE_VARIABLE
{
    PV_SPAWN_TIMER,
    PV_SPAWN_X,
    PV_SPAWN_Y,
    PV_SPAWN_W,
    PV_SPAWN_H,
    PV_LIFE_LOW,
    PV_LIFE_HIGH,
    PV_PUSHMIN_X,
    PV_PUSHMIN_Y,
    PV_PUSHMAX_X,
    PV_PUSHMAX_Y,
    PV_SPAWN_SPEED,
    pv_variable_limit
};

struct particlesystem
{
    particle particles[particle_limit];
    unsigned int particle_count, particles_alive = 0;
    bool fadewithlife = true;

    float variables[pv_variable_limit] = {0.0f};
    float *variable_pointers[pv_variable_limit];

    sprite visual;

    particlesystem();
    particlesystem(const char *path, object *obj, unsigned int frame, unsigned int _particle_count, float _life_lower, float _life_upper,
                   float sX, float sY, float sW, float sH);

    void spawn(float delta_time);
    void push(float xVel, float yVel, float xVelMax, float yVelMax);

    void linkVariable(PARTICLE_VARIABLE pv, float *value);

    void update(float delta_time);
    void draw(GLFWwindow *win, shader &program, float delta_time);

    void kill();
};

#endif