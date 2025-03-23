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
particlesystem::particlesystem(const char *path, unsigned int frame, unsigned int _particle_count, double _life_lower, double _life_upper,
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

    // visual = sprite(path, 0.0, 0.0, 1, 1);
    visual = sprite(path, 1, 1);
    // visual.rect.setTextureRect(sf::IntRect(visual.spriteW * ((frame % visual.framesX) - 1), visual.spriteH * (frame / visual.framesX),
    //                                        visual.spriteW, visual.spriteH));
    // visual.textureX = visual.spriteW * ((frame % visual.framesX) - 1);
}

void particlesystem::spawn(double delta_time)
{
    // if (variable_pointers[PV_SPAWN_TIMER] == nullptr)
    variables[PV_SPAWN_TIMER] -= 10.0 * delta_time;

    double spawnTimer = variables[PV_SPAWN_TIMER];
    // if (variable_pointers[PV_SPAWN_TIMER] != nullptr)
    //     spawnTimer = *variable_pointers[PV_SPAWN_TIMER];

    if (particles_alive >= particle_count || spawnTimer > 0.0)
        return;

    std::uniform_real_distribution<double> posXRand(variables[PV_SPAWN_X], variables[PV_SPAWN_W]);
    std::uniform_real_distribution<double> posYRand(variables[PV_SPAWN_Y], variables[PV_SPAWN_H]);
    std::uniform_real_distribution<double> lifeRand(variables[PV_LIFE_LOW], variables[PV_LIFE_HIGH]);

    particles[particles_alive].Put(posXRand(numGen), posYRand(numGen));
    particles[particles_alive].velX = 0.0;
    particles[particles_alive].velY = 0.0;
    particles[particles_alive].life = lifeRand(numGen);
    particles[particles_alive].lifestartalphamultiple = 255.0 / particles[particles_alive].life;

    std::uniform_real_distribution<double> parXVel(variables[PV_PUSHMIN_X], variables[PV_PUSHMAX_X]);
    std::uniform_real_distribution<double> parYVel(variables[PV_PUSHMIN_Y], variables[PV_PUSHMAX_Y]);

    particles[particles_alive].velX = parXVel(numGen);
    particles[particles_alive].velY = parYVel(numGen);
    ++particles_alive;

    variables[PV_SPAWN_TIMER] = variables[PV_SPAWN_SPEED];
}

void particlesystem::push(double xVel, double yVel, double xVelMax, double yVelMax)
{
    variables[PV_PUSHMIN_X] = xVel;
    variables[PV_PUSHMIN_Y] = yVel;
    variables[PV_PUSHMAX_X] = xVelMax;
    variables[PV_PUSHMAX_Y] = yVelMax;
}

void particlesystem::update(double delta_time)
{
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
        // if (fadewithlife)  // should go in particlesystem::draw()
        //     particles[i].visual.rect.setColor(sf::Color(255, 255, 255, static_cast<int>(particles[i].life * particles[i].lifestartalphamultiple)));
    }
}
void particlesystem::draw(GLFWwindow *win, shader &program, object &sprite_object, double delta_time)
{
    for (int i = 0; i < particle_count; ++i)
    {
        visual.Put(particles[i].x, particles[i].y, 0.0);

        if (fadewithlife)
            visual.SetColor(255, 255, 255, static_cast<int>(particles[i].life * particles[i].lifestartalphamultiple));

        // std::cout << visual.rect.getPosition().x << ", " << visual.rect.getPosition().y << " pos\n";

        // win.draw(visual.rect);
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