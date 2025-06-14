#ifndef EFFECTS_H
#define EFFECTS_H

#include "sprite.h"
#include <random>

const unsigned int particle_limit = 2512;
const unsigned int particle_system_limit = 6;
const unsigned int transition_limit = 12;

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
    double w = 0.0, h = 0.0, red = 0.0, green = 0.0, blue = 0.0, alpha = 0.0, animationTime = 0.0;

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
    PV_WIDTH,
    PV_HEIGHT,
    PV_RED,
    PV_GREEN,
    PV_BLUE,
    PV_ALPHA,
    PV_ALPHA_LIFE_FALLOFF,
    PV_WIDTH_LIFE_FALLOFF,
    PV_HEIGHT_LIFE_FALLOFF,
    PV_ANIM_START,
    PV_ANIM_END,
    PV_ANIM_SPEED,
    pv_variable_limit
};

struct particlesystem
{
    particle particles[particle_limit];
    unsigned int particle_count = 0, particles_alive = 0;
    unsigned int totalParticlesSpawned = 0;
    unsigned int id;

    double variables[pv_variable_limit] = {0.0};
    double *variable_pointers[pv_variable_limit] = {nullptr};

    sprite visual;

    particlesystem();
    particlesystem(const char *path, shader *visualShader, object *visualObj, unsigned int fx, unsigned int fy, unsigned int _particle_count);

    void spawn(double delta_time);

    void setVariable(PARTICLE_VARIABLE pv, double value)
    {
        variables[pv] = value;
    }
    void linkVariable(PARTICLE_VARIABLE pv, double *value);

    void update(double delta_time);
    void draw(double delta_time);

    void kill();
};

enum light_type
{
    LIGHT_DIRECTIONAL,
    LIGHT_POINT,
    LIGHT_SPOT
};
enum light_material
{
    MATERIAL_DEFAULT,
    MATERIAL_DEFAULT_2D
};
struct light
{
    light_type ltype = LIGHT_DIRECTIONAL;
    glm::vec3 position = glm::vec3(0.0);
    glm::vec3 direction = glm::vec3(0.0);

    glm::vec3 color = glm::vec3(0.0);

    float ambient = 0.0;
    float diffuse = 0.0;
    float specular = 0.0;

    void setMaterialValues(light_material mat)
    {
        switch (mat)
        {
        case MATERIAL_DEFAULT:
            ambient = 0.1;
            diffuse = 0.5;
            specular = 1.0;
            break;
        case MATERIAL_DEFAULT_2D:
            ambient = 1.0;
            diffuse = 0.1;
            specular = 0.2;
            break;
        default:
            ambient = 0.1;
            diffuse = 0.5;
            specular = 1.0;
            break;
        }
    }

    light() {}
    light(light_type l, glm::vec3 p, glm::vec3 d, glm::vec3 c, light_material m) : ltype(l), position(p), direction(d), color(c)
    {
        setMaterialValues(m);
    }
};

#endif