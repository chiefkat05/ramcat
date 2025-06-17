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

    double ambient = 0.0;
    double diffuse = 0.0;
    double specular = 0.0;

    double falloff_constant = 0.0;
    double falloff_linear = 0.0;
    double falloff_quadtratic = 0.0;
    double falloff_scale = 0.0;

    double cutoff = 0.0, outer_cutoff = 0.0;

    double *position_x_p = nullptr, *direction_x_p = nullptr, *color_x_p = nullptr;
    double *position_y_p = nullptr, *direction_y_p = nullptr, *color_y_p = nullptr;
    double *position_z_p = nullptr, *direction_z_p = nullptr, *color_z_p = nullptr;
    double *falloff_scale_p = nullptr, *cutoff_p = nullptr, *outer_cutoff_p = nullptr;

    void update_values()
    {
        if (position_x_p != nullptr)
        {
            position.x = *position_x_p;
        }
        if (position_y_p != nullptr)
        {
            position.y = *position_y_p;
        }
        if (position_z_p != nullptr)
        {
            position.z = *position_z_p;
        }
        if (direction_x_p != nullptr)
        {
            direction.x = *direction_x_p;
        }
        if (direction_y_p != nullptr)
        {
            direction.y = *direction_y_p;
        }
        if (direction_z_p != nullptr)
        {
            direction.z = *direction_z_p;
        }
        if (color_x_p != nullptr)
        {
            color.x = *color_x_p;
        }
        if (color_y_p != nullptr)
        {
            color.y = *color_y_p;
        }
        if (color_z_p != nullptr)
        {
            color.z = *color_z_p;
        }
        if (falloff_scale_p != nullptr)
        {
            falloff_scale = *falloff_scale_p;
        }
        if (cutoff_p != nullptr)
        {
            cutoff = *cutoff_p;
        }
        if (outer_cutoff_p != nullptr)
        {
            outer_cutoff = *outer_cutoff_p;
        }
    }

    void link_position(double *x = nullptr, double *y = nullptr, double *z = nullptr)
    {
        position_x_p = x;
        position_y_p = y;
        position_z_p = z;
    }
    void link_direction(double *x = nullptr, double *y = nullptr, double *z = nullptr)
    {
        direction_x_p = x;
        direction_y_p = y;
        direction_z_p = z;
    }
    void link_color(double *x = nullptr, double *y = nullptr, double *z = nullptr)
    {
        color_x_p = x;
        color_y_p = y;
        color_z_p = z;
    }
    void link_falloff_scale(double *p)
    {
        falloff_scale_p = p;
    }
    void link_cutoff(double *p, double *out_p)
    {
        cutoff_p = p;
        outer_cutoff_p = out_p;
    }

    void setMaterialValues(light_material mat)
    {
        switch (mat)
        {
        case MATERIAL_DEFAULT:
            ambient = 0.1;
            diffuse = 0.5;
            specular = 1.0;

            falloff_constant = 1.0;
            falloff_linear = 0.09;
            falloff_quadtratic = 0.032;
            break;
        case MATERIAL_DEFAULT_2D:
            ambient = 1.0;
            diffuse = 0.1;
            specular = 0.2;

            falloff_constant = 1.0;
            falloff_linear = 1.0;
            falloff_quadtratic = 1.0;
            break;
        default:
            ambient = 0.1;
            diffuse = 0.5;
            specular = 1.0;

            falloff_constant = 1.0;
            falloff_linear = 0.09;
            falloff_quadtratic = 0.032;
            break;
        }
    }

    light() {}
    light(light_type l, glm::vec3 p, glm::vec3 d, glm::vec3 c, double fs, double cut = 0.0, double out_cut = 0.0, light_material m = MATERIAL_DEFAULT)
        : ltype(l), position(p), direction(d), color(c), falloff_scale(fs), cutoff(cut), outer_cutoff(out_cut)
    {
        setMaterialValues(m);
    }
};

#endif