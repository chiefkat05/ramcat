#ifndef EFFECTS_H
#define EFFECTS_H

#include "sprite.h"
#include <random>

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
    double x = 0.0, y = 0.0, velX = 0.0, velY = 0.0, life = 0.0;
    double lifestartalphamultiple = 1.0;

    void Put(double _x, double _y)
    {
        x = _x;
        y = _y;
    }
    void Move(double _offx, double _offy)
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

    double variables[pv_variable_limit] = {0.0};
    double *variable_pointers[pv_variable_limit];

    sprite visual;

    particlesystem();
    particlesystem(const char *path, unsigned int frame, unsigned int _particle_count, double _life_lower, double _life_upper,
                   double sX, double sY, double sW, double sH);

    void spawn(double delta_time);
    void push(double xVel, double yVel, double xVelMax, double yVelMax);

    void linkVariable(PARTICLE_VARIABLE pv, double *value);

    void update(double delta_time);
    void draw(GLFWwindow *win, shader &program, object &sprite_object, double delta_time);

    void kill();
};

#endif