#include "../headers/effects.h"

std::default_random_engine numGen;

particlesystem::particlesystem()
{
    variables[PV_SPAWN_TIMER] = 0.0;
    variables[PV_LIFE_LOW] = 0.0;
    variables[PV_LIFE_HIGH] = 0.0;
    variables[PV_SPAWN_X] = 0.0;
    variables[PV_SPAWN_Y] = 0.0;
    variables[PV_SPAWN_W] = 0.0;
    variables[PV_SPAWN_H] = 0.0;

    particle_count = 0.0;
}
particlesystem::particlesystem(const char *path, unsigned int fx, unsigned int fy, unsigned int _particle_count, double _life_lower, double _life_upper,
                               double sX, double sY, double sW, double sH)
{
    variables[PV_SPAWN_TIMER] = 0.0;
    variables[PV_LIFE_LOW] = _life_lower;
    variables[PV_LIFE_HIGH] = _life_upper;
    variables[PV_SPAWN_X] = sX;
    variables[PV_SPAWN_Y] = sY;
    variables[PV_SPAWN_W] = sW;
    variables[PV_SPAWN_H] = sH;

    particle_count = particle_limit;
    if (_particle_count < particle_limit)
        particle_count = _particle_count;

    visual = sprite(path, fx, fy);
}

void particlesystem::spawn(double delta_time)
{
    for (int i = 0; i < pv_variable_limit; ++i)
    {
        if (variable_pointers[i] != nullptr)
        {
            variables[i] = *variable_pointers[i];
        }
    }
    variables[PV_SPAWN_TIMER] -= 10.0 * delta_time;

    double spawnTimer = variables[PV_SPAWN_TIMER];

    if (particles_alive >= particle_count || spawnTimer > 0.0)
        return;

    std::uniform_real_distribution<double> posXRand(variables[PV_SPAWN_X], variables[PV_SPAWN_W]);
    std::uniform_real_distribution<double> posYRand(variables[PV_SPAWN_Y], variables[PV_SPAWN_H]);
    std::uniform_real_distribution<double> lifeRand(variables[PV_LIFE_LOW], variables[PV_LIFE_HIGH]);

    particles[particles_alive].Put(posXRand(numGen), posYRand(numGen));
    particles[particles_alive].velX = 0.0;
    particles[particles_alive].velY = 0.0;
    particles[particles_alive].life = lifeRand(numGen);
    particles[particles_alive].w = variables[PV_WIDTH];
    particles[particles_alive].h = variables[PV_HEIGHT];
    particles[particles_alive].red = variables[PV_RED];
    particles[particles_alive].green = variables[PV_GREEN];
    particles[particles_alive].blue = variables[PV_BLUE];
    particles[particles_alive].alpha = variables[PV_ALPHA];

    std::uniform_real_distribution<double> parXVel(variables[PV_PUSHMIN_X], variables[PV_PUSHMAX_X]);
    std::uniform_real_distribution<double> parYVel(variables[PV_PUSHMIN_Y], variables[PV_PUSHMAX_Y]);

    particles[particles_alive].velX = parXVel(numGen);
    particles[particles_alive].velY = parYVel(numGen);
    ++particles_alive;

    variables[PV_SPAWN_TIMER] = variables[PV_SPAWN_SPEED];
}

void particlesystem::linkVariable(PARTICLE_VARIABLE pv, double *value)
{
    variable_pointers[pv] = value;
}

void particlesystem::update(double delta_time)
{
    for (int i = 0; i < pv_variable_limit; ++i)
    {
        if (variable_pointers[i] != nullptr)
        {
            variables[i] = *variable_pointers[i];
        }
    }

    for (int i = 0; i < particles_alive; ++i)
    {
        if (particles[i].life < 0.0)
        {
            particles[i] = particles[particles_alive - 1];
            --particles_alive;
            continue;
        }

        particles[i].Move(particles[i].velX * delta_time, particles[i].velY * delta_time);
        particles[i].life -= 10.0 * delta_time;
    }
}
void particlesystem::draw(shader &program, object &sprite_object, double delta_time)
{
    for (int i = 0; i < particle_count; ++i)
    {
        particles[i].w = variables[PV_WIDTH];
        particles[i].h = variables[PV_HEIGHT];
        particles[i].red = variables[PV_RED];
        particles[i].green = variables[PV_GREEN];
        particles[i].blue = variables[PV_BLUE];
        if (variables[PV_ALPHA_LIFE_FALLOFF] > 0.0)
        {
            particles[i].alpha = particles[i].life * variables[PV_ALPHA_LIFE_FALLOFF];
        }
        else
        {
            particles[i].alpha = variables[PV_ALPHA];
        }

        particles[i].animationTime += variables[PV_ANIM_SPEED] * delta_time;
        if (particles[i].animationTime < variables[PV_ANIM_START])
            particles[i].animationTime = variables[PV_ANIM_START];
        if (particles[i].animationTime >= variables[PV_ANIM_END])
            particles[i].animationTime = variables[PV_ANIM_START];

        visual.Put(particles[i].x, particles[i].y, 0.0);

        visual.SetColor(particles[i].red, particles[i].green, particles[i].blue, particles[i].alpha);
        if (variables[PV_WIDTH_LIFE_FALLOFF] > 0.0)
        {
            visual.w = particles[i].life * variables[PV_WIDTH_LIFE_FALLOFF] * variables[PV_WIDTH];
        }
        else
        {
            visual.w = variables[PV_WIDTH];
        }
        if (variables[PV_HEIGHT_LIFE_FALLOFF] > 0.0)
        {
            visual.h = particles[i].life * variables[PV_HEIGHT_LIFE_FALLOFF] * variables[PV_HEIGHT];
        }
        else
        {
            visual.h = variables[PV_HEIGHT];
        }
        visual.textureX = static_cast<int>(particles[i].animationTime) % visual.framesX;
        visual.textureY = static_cast<int>(particles[i].animationTime) / visual.framesX;

        visual.Draw(program, sprite_object);
    }
}

void particlesystem::kill()
{
    for (int i = 0; i < particle_count; ++i)
    {
        particles[i].life = 0.0;
        particles[i].velX = 0.0;
        particles[i].velY = 0.0;
        particles[i].Put(0.0, 0.0);
    }
    particles_alive = 0;
}