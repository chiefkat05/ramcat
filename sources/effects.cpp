#include "../headers/effects.h"

std::default_random_engine numGen;

// void soundhandler::load(const std::string filepath)
// {
//     if (!buffer.loadFromFile(filepath))
//     {
//         std::cout << "! sound effect at " << filepath << " failed to load.\n";
//         return;
//     }
// }
// void soundhandler::play()
// {
//     sound.setBuffer(buffer);
//     sound.play();
// }

particlesystem::particlesystem()
{
    variables[PV_SPAWN_TIMER] = 0.0f;
    variables[PV_LIFE_LOW] = 0.0f;
    variables[PV_LIFE_HIGH] = 0.0f;
    variables[PV_SPAWN_X] = 0.0f;
    variables[PV_SPAWN_Y] = 0.0f;
    variables[PV_SPAWN_W] = 0.0f;
    variables[PV_SPAWN_H] = 0.0f;

    particle_count = 0.0f;
}
particlesystem::particlesystem(const char *path, object *obj, unsigned int frame, unsigned int _particle_count, float _life_lower, float _life_upper,
                               float sX, float sY, float sW, float sH)
{
    variables[PV_SPAWN_TIMER] = 0.0f;
    variables[PV_LIFE_LOW] = _life_lower;
    variables[PV_LIFE_HIGH] = _life_upper;
    variables[PV_SPAWN_X] = sX;
    variables[PV_SPAWN_Y] = sY;
    variables[PV_SPAWN_W] = sW;
    variables[PV_SPAWN_H] = sH;

    particle_count = particle_limit;
    if (_particle_count < particle_limit)
        particle_count = _particle_count;

    // visual = sprite(path, 0.0f, 0.0f, 1, 1);
    visual = sprite(obj, path, 1, 1);
    // visual.rect.setTextureRect(sf::IntRect(visual.spriteW * ((frame % visual.framesX) - 1), visual.spriteH * (frame / visual.framesX),
    //                                        visual.spriteW, visual.spriteH));
    // visual.textureX = visual.spriteW * ((frame % visual.framesX) - 1);
}

void particlesystem::spawn(float delta_time)
{
    // if (variable_pointers[PV_SPAWN_TIMER] == nullptr)
    variables[PV_SPAWN_TIMER] -= 10.0f * delta_time;

    float spawnTimer = variables[PV_SPAWN_TIMER];
    // if (variable_pointers[PV_SPAWN_TIMER] != nullptr)
    //     spawnTimer = *variable_pointers[PV_SPAWN_TIMER];

    if (particles_alive >= particle_count || spawnTimer > 0.0f)
        return;

    std::uniform_real_distribution<double> posXRand(variables[PV_SPAWN_X], variables[PV_SPAWN_W]);
    std::uniform_real_distribution<double> posYRand(variables[PV_SPAWN_Y], variables[PV_SPAWN_H]);
    std::uniform_real_distribution<double> lifeRand(variables[PV_LIFE_LOW], variables[PV_LIFE_HIGH]);

    particles[particles_alive].Put(posXRand(numGen), posYRand(numGen));
    particles[particles_alive].velX = 0.0f;
    particles[particles_alive].velY = 0.0f;
    particles[particles_alive].life = lifeRand(numGen);
    particles[particles_alive].lifestartalphamultiple = 255.0f / particles[particles_alive].life;

    std::uniform_real_distribution<double> parXVel(variables[PV_PUSHMIN_X], variables[PV_PUSHMAX_X]);
    std::uniform_real_distribution<double> parYVel(variables[PV_PUSHMIN_Y], variables[PV_PUSHMAX_Y]);

    particles[particles_alive].velX = parXVel(numGen);
    particles[particles_alive].velY = parYVel(numGen);
    ++particles_alive;

    variables[PV_SPAWN_TIMER] = variables[PV_SPAWN_SPEED];
}

void particlesystem::push(float xVel, float yVel, float xVelMax, float yVelMax)
{
    variables[PV_PUSHMIN_X] = xVel;
    variables[PV_PUSHMIN_Y] = yVel;
    variables[PV_PUSHMAX_X] = xVelMax;
    variables[PV_PUSHMAX_Y] = yVelMax;
}

void particlesystem::update(float delta_time)
{
    for (int i = 0; i < particles_alive; ++i)
    {
        if (particles[i].life < 0.0f)
        {
            particles[i] = particles[particles_alive - 1];
            --particles_alive;
            continue;
        }

        particles[i].Move(particles[i].velX * delta_time, particles[i].velY * delta_time);
        particles[i].life -= 10.0f * delta_time;
        // if (fadewithlife)  // should go in particlesystem::draw()
        //     particles[i].visual.rect.setColor(sf::Color(255, 255, 255, static_cast<int>(particles[i].life * particles[i].lifestartalphamultiple)));
    }
}
void particlesystem::draw(GLFWwindow *win, shader &program, float delta_time)
{
    for (int i = 0; i < particle_count; ++i)
    {
        visual.Put(particles[i].x, particles[i].y, 0.0f);

        if (fadewithlife)
            visual.SetColor(255, 255, 255, static_cast<int>(particles[i].life * particles[i].lifestartalphamultiple));

        // std::cout << visual.rect.getPosition().x << ", " << visual.rect.getPosition().y << " pos\n";

        // win.draw(visual.rect);
        visual.Draw(program);
    }
}

void particlesystem::kill()
{
    for (int i = 0; i < particle_count; ++i)
    {
        particles[i].life = 0.0f;
        particles[i].velX = 0.0f;
        particles[i].velY = 0.0f;
        particles[i].Put(0.0f, 0.0f);
    }
    particles_alive = 0;
}