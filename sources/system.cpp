#include "../headers/system.h"

#define MINIAUDIO_IMPLEMENTATION
#include "../headers/miniaudio.h"

int gamepad_stick_sensitivity = 500;
extern double windowAspectDivision;

bool player::getInput(GLFWwindow *window, controlset action)
{
    if (gamepad_id > -1)
    {
        // int count;
        // const unsigned char *gamepadbuttons = glfwGetJoystickButtons(gamepad_id, &count);
        GLFWgamepadstate state;
        glfwGetGamepadState(gamepad_id, &state);

        double realSensitivity = static_cast<double>(gamepad_stick_sensitivity) * 0.001f;

        if (gamepad_inputs[action] < PAD_seperation_value && state.buttons[gamepad_inputs[action]])
        {
            return true;
        }
        else if (gamepad_inputs[action] > PAD_seperation_value)
        {
            switch (gamepad_inputs[action])
            {
            case PAD_LSTICK_LEFT:
                return (state.axes[0] < -realSensitivity);
            case PAD_LSTICK_RIGHT:
                return (state.axes[0] > realSensitivity);
            case PAD_LSTICK_DOWN:
                return (state.axes[1] > realSensitivity);
            case PAD_LSTICK_UP:
                return (state.axes[1] < -realSensitivity);
            case PAD_RSTICK_LEFT:
                return (state.axes[2] < -realSensitivity);
            case PAD_RSTICK_RIGHT:
                return (state.axes[2] > realSensitivity);
            case PAD_RSTICK_DOWN:
                return (state.axes[3] > realSensitivity);
            case PAD_RSTICK_UP:
                return (state.axes[3] < -realSensitivity);
            case PAD_TRIGGER_L:
                return (state.axes[4] > realSensitivity);
            case PAD_TRIGGER_R:
                return (state.axes[5] > realSensitivity);
            default:
                break;
            }
        }
    }
    if (gamepad_id == -1)
    {
        if (glfwGetKey(window, inputs[action]))
        {
            return true;
        }
    }

    return false;
}

character::character() {}
character::character(sprite &v, IDENTIFICATION _id) : visual(v)
{
    id = _id;
    hp = maxhp;
}
// COLLISION BOXES SHOULD BE HANDLED IN WHATEVER FUNCTION IS CALLING CHARACTER CREATION
character::character(std::string filepath, double x, double y, double w, double h, unsigned int fx, unsigned int fy, IDENTIFICATION _id)
{
    visual = sprite(filepath.c_str(), fx, fy);
    visual.Put(x, y, 0.0);
    visual.Scale(w, h, 0.1);
    id = _id;
    hp = maxhp;
}

void character::MoveTo(double _x, double _y, world *currentWorld)
{
    if (currentWorld != nullptr)
    {
        _x -= currentWorld->spawnLocationX;
        _y -= currentWorld->spawnLocationY;
    }
}

void character::Update(double delta_time)
{
    if (hp <= 0)
    {
        visual.Rotate(0.0, 0.0, 90.0);
        return;
    }

    if (animationFinished)
    {
        PlayAnimation(ANIM_IDLE, delta_time, true);
    }

    animations[playingAnim].run(delta_time, animationLooping);

    animationFinished = false;

    if (animations[playingAnim].finished)
        animationFinished = true;

    onGround = false;

    switch (id)
    {
    case CH_COFFEEMUGGUY:
        PlayAnimation(ANIM_IDLE, delta_time, true);
        break;
    default:
        break;
    }
}
void character::updatePosition(double delta_time)
{
    if (parrySuccess)
    {
        return;
    }
    visual.Move(velocityX * delta_time, velocityY * delta_time, 0.0);
    putCollider(COLLIDER_SOLID, visual.x, visual.y - 0.08);
    putCollider(COLLIDER_STRIKE, visual.x - 0.08, visual.y);
    if (onGround && velocityY < 0.0)
    {
        velocityY = 0.0;
    }
}

void character::SetAnimation(ANIMATION_MAPPINGS id, unsigned int s, unsigned int e, double spd)
{
    animations[id] = animation(&visual, s, e, spd);
}
void character::PlayAnimation(ANIMATION_MAPPINGS id, double delta_time, bool loops)
{
    if (animations[id]._sprite == nullptr)
        return;

    if (playingAnim != id)
    {
        animations[id].frame = animations[id].start;
        animations[id].timer = 10.0;
        animations[playingAnim].finished = true;
    }
    playingAnim = id;
    animationLooping = loops;
}
void character::StopAnimation(ANIMATION_MAPPINGS id)
{
    animations[id].finished = true;
    animations[id].timer = 0.0;
    animations[id].frame = animations[id].end;
}

int qsPartition(sprite *sprites[character_limit], int low, int high) // see if there's a better optimized way to do this, such as random pivot
{
    sprite *pivot = sprites[low];

    int i = high + 1;
    for (int j = low + 1; j <= high; ++j)
    {
        if (sprites[j]->y > pivot->y)
        {
            --i;
            std::swap(sprites[i], sprites[j]);
        }
    }

    std::swap(sprites[i - 1], sprites[low]);
    return i - 1;
}
void quicksortSprites(sprite *sprites[character_limit], int low, int high)
{
    if (low < high)
    {
        int pi = qsPartition(sprites, low, high);

        quicksortSprites(sprites, low, pi - 1);
        quicksortSprites(sprites, pi + 1, high);
    }
}

void game_system::Add(character e)
{
    characters[characterCount] = e;
    sortedSprites[characterCount] = &characters[characterCount].visual;

    if (characters[characterCount].id >= ch_monster_ids)
    {
        characters[characterCount].SetAnimation(ANIM_IDLE, 0, 0, 0.0);
        characters[characterCount].SetAnimation(ANIM_WALK, 0, 0, 100.0);
        characters[characterCount].SetAnimation(ANIM_HURT, 0, 0, 250.0);
        characters[characterCount].SetAnimation(ANIM_DEAD, 0, 0, 100.0);
        characters[characterCount].SetAnimation(ANIM_ABILITY_2, 0, 0, 100.0);
        characters[characterCount].SetAnimation(ANIM_ABILITY_0, 0, 0, 100.0);
        characters[characterCount].SetAnimation(ANIM_ABILITY_1, 0, 0, 100.0);
    }
    switch (characters[characterCount].id)
    {
    case CH_GULK:
        characters[characterCount].velocityX = -0.5;
        break;
    default:
        break;
    }
    ++characterCount;
}
void game_system::Remove(int index)
{
    for (int i = index; i < characterCount - 1; ++i)
    {
        characters[i] = characters[i + 1];
    }
    --characterCount;
}
void game_system::ClearEnemies()
{
    for (int i = 0; i < characterCount; ++i)
    {
        if (characters[i].plControl != nullptr || characters[i].id <= ch_monster_ids || characters[i].id >= ch_other_ids)
            continue;

        Remove(i);
    }
}

void game_system::loopSound(unsigned int id)
{
    // code that makes id sound loop if playing
}
void game_system::initSound(std::string path, unsigned int id, ma_engine *engine)
{
    if (game_sounds[id].pDataSource != nullptr)
    {
        if (sound_paths[id] != path && !sound_paths[id].empty())
        {
            ma_sound_uninit(&game_sounds[id]);
        }
        else
        {
            return;
        }
    }

    sound_paths[id] = path;
    game_sound_result = ma_sound_init_from_file(engine, sound_paths[id].c_str(), 0, NULL, NULL, &game_sounds[id]);
    if (game_sound_result != MA_SUCCESS)
    {
        std::cout << game_sound_result << " sound error\n";
    }
}
void game_system::playSound(unsigned int id, int start_time, bool interrupt)
{
    if (&game_sounds[id].pDataSource == nullptr)
    {
        std::cout << "/n/tWarning: sound " << id << " at path \n";
        std::cout << sound_paths[id] << " has no data!\n";
        return;
    }

    if (!interrupt && ma_sound_is_playing(&game_sounds[id]))
        return;

    if (id < sound_is_music_cutoff)
    {
        ma_sound_set_volume(&game_sounds[id], static_cast<double>(music_volume) / 100.0);
    }
    else
    {
        ma_sound_set_volume(&game_sounds[id], static_cast<double>(sound_volume) / 100.0);
    }
    ma_sound_seek_to_pcm_frame(&game_sounds[id], start_time);

    if (!ma_sound_is_playing(&game_sounds[id]))
        ma_sound_start(&game_sounds[id]);
}
void game_system::stopSound(unsigned int id)
{
    ma_sound_stop(&game_sounds[id]);
}

void game_system::update(world &floor, shader &particle_program, object &particle_sprite, double delta_time)
{
    for (int i = 0; i < characterCount; ++i)
    {
        characters[i].Update(delta_time);

        if (!characters[i].onGround)
        {
            characters[i].velocityY -= 10.0 * delta_time;
        }

        for (int j = 0; j < characterCount; ++j)
        {
            if (j == i)
                continue;

            for (int k = 0; k < character_collider_limit; ++k)
            {
                if (characters[i].colliders[k].collisionID != k)
                    continue;
                for (int l = 0; l < character_collider_limit; ++l)
                {
                    if (characters[j].colliders[l].collisionID != l)
                        continue;

                    double xNormal = 0.0, yNormal = 0.0, distanceToClosestSide = 0.0;
                    bool insideCollision = false;
                    double firstCollisionHitTest = characters[i].colliders[k].response(characters[i].velocityX * delta_time,
                                                                                       characters[i].velocityY * delta_time,
                                                                                       characters[j].velocityX * delta_time,
                                                                                       characters[j].velocityY * delta_time,
                                                                                       characters[j].colliders[l], xNormal, yNormal,
                                                                                       characters[i].visual.x, characters[i].visual.y,
                                                                                       insideCollision, distanceToClosestSide);

                    if (k == COLLIDER_SOLID && l == COLLIDER_SOLID && insideCollision)
                    {
                        if (xNormal != 0.0)
                        {
                            characters[i].visual.x = firstCollisionHitTest;
                            characters[i].velocityX = 0.0;
                        }
                        if (yNormal != 0.0)
                        {
                            characters[i].visual.y = firstCollisionHitTest;
                            characters[i].velocityY = 0.0;
                        }
                        if (yNormal > 0.0)
                        {
                            characters[i].velocityX += characters[j].velocityX;
                            characters[i].onGround = true;
                            if (characters[j].id == CH_COFFEEMUGGUY)
                            {
                                characters[j].PlayAnimation(ANIM_ABILITY_0, delta_time, false);
                            }
                        }
                    }
                    if (k == COLLIDER_STRIKE && l == COLLIDER_SOLID && insideCollision)
                    {
                        characters[j].hp = 0;
                        if (characters[j].id == CH_COFFEEMUGGUY)
                        {
                            state = COFFEE_MUG_DEATH_STATE;
                        }
                    }
                }
            }
        }

        for (int j = 0; j < floor.collision_box_count; ++j)
        {
            if (floor.collision_boxes[j].min_x == 0.0 &&
                    floor.collision_boxes[j].min_y == 0.0 &&
                    floor.collision_boxes[j].max_x == 0.0 &&
                    floor.collision_boxes[j].max_y == 0.0 ||
                floor.collision_boxes[j].collisionID <= -1)
            {
                continue;
            }

            double xNormal = 0.0, yNormal = 0.0, distanceToClosestSide = 0.0;
            bool insideCollision = false; // now for taking these apart into more managable functions!
            // something like
            // bool walkableInterior = (distanceToClosestSide >= -0.01);
            // and
            // void respondCollision(int collisionID, bool walkableInterior = true, bool groundBlock = true, replaceOnSpawnWith(GULK));

            double firstCollisionHitTest = characters[i].colliders[COLLIDER_SOLID].response(characters[i].velocityX * delta_time, characters[i].velocityY * delta_time, 0.0, 0.0,
                                                                                            floor.collision_boxes[j], xNormal, yNormal, characters[i].visual.x,
                                                                                            characters[i].visual.y, insideCollision, distanceToClosestSide);

            switch (floor.collision_boxes[j].collisionID)
            {
            case -1:
                break;
            case 1:
                if (insideCollision)
                {
                    if (xNormal != 0.0)
                    {
                        characters[i].visual.x = firstCollisionHitTest;
                        characters[i].velocityX = 0.0;
                    }
                    if (yNormal != 0.0)
                    {
                        characters[i].hp = 0;
                    }
                }
                break;
            case 2:
                if (insideCollision && characters[i].plControl != nullptr)
                {
                    levelincreasing = true;
                }
                break;
            case 3:
                if (insideCollision && yNormal > 0.0 && distanceToClosestSide >= -0.01 && characters[i].velocityY < 0.0)
                {
                    characters[i].visual.y = firstCollisionHitTest;
                    characters[i].velocityY = 0.0;
                    characters[i].onGround = true;
                }
                break;
            case 4:
                if (insideCollision && xNormal < 0.0 && distanceToClosestSide >= -0.01 && characters[i].velocityX > 0.0)
                {
                    characters[i].visual.x = firstCollisionHitTest;
                    characters[i].velocityX = 0.0;
                    characters[i].onGround = true;
                }
                break;
            case 5:
                if (insideCollision && yNormal < 0.0 && distanceToClosestSide >= -0.01 && characters[i].velocityY > 0.0)
                {
                    characters[i].visual.y = firstCollisionHitTest;
                    characters[i].velocityY = 0.0;
                    characters[i].onGround = true;
                }
                break;
            case 6:
                if (insideCollision && xNormal > 0.0 && distanceToClosestSide >= -0.01 && characters[i].velocityX < 0.0)
                {
                    characters[i].visual.x = firstCollisionHitTest;
                    characters[i].velocityX = 0.0;
                    characters[i].onGround = true;
                }
                break;
            case 7:
                if (insideCollision && yNormal != 0.0 && characters[i].velocityY < 0.0)
                {
                    characters[i].velocityY = 4.0;
                }
                break;
            case 8:
                if (insideCollision)
                {
                    tile *fishTile = floor.getTileFromCollisionSpecialID(j);
                    if (fishTile == nullptr)
                        break;

                    fishTile->id = -1;
                    fishTile->collisionID = -1;
                    floor.collision_boxes[j].collisionID = -1;
                    ++fishCollected;
                }
                break;
            case 9:
                if (insideCollision)
                {
                    tile *checkpointTile = floor.getTileFromCollisionSpecialID(j);
                    checkpointTile->id = 2;
                    checkpointTile->collisionID = -1;
                    floor.collision_boxes[j].collisionID = -1;

                    floor.spawnLocationX = floor.collision_boxes[j].min_x * 0.16f;
                    floor.spawnLocationY = -0.2f + (-static_cast<double>(floor.roomHeight) + floor.collision_boxes[j].min_y) * 0.16f;
                }
                break;
            case 10:
                if (insideCollision)
                {
                    tile *coinTile = floor.getTileFromCollisionSpecialID(j);
                    coinTile->emptyTile();
                    floor.collision_boxes[j].collisionID = -1;
                    characters[i].runSpeed *= 1.001;
                }
                break;
            case 11:
                // for (int x = 0; x < floor.roomWidth; ++x)
                // {
                //     for (int y = 0; y < floor.roomHeight; ++y)
                //     {
                //         if (floor.tiles[x][y].specialTileID == floor.collision_boxes[j].specialTileID)
                //         {
                //             floor.tiles[x][y].id = -1;
                //             floor.tiles[x][y].collisionID = -1;
                //             floor.collision_boxes[j].collisionID = -1;

                //             Add(character("./img/char/gulk.png", x * 0.16, y * 0.16f + 0.2, 0.32, 0.32,
                //                           4, 1, CH_GULK));
                //             characters[characterCount - 1].setCollider(COLLIDER_SOLID, aabb(characters[characterCount - 1].visual.x,
                //                                                                             characters[characterCount - 1].visual.y,
                //                                                                             characters[characterCount - 1].visual.x + 0.16,
                //                                                                             characters[characterCount - 1].visual.y + 0.24));
                //             characters[characterCount - 1].colliderOn(COLLIDER_SOLID);
                //             characters[characterCount - 1].scaleCollider(COLLIDER_STRIKE, 0.32, 0.16);
                //         }
                //     }
                // }
                if (true)
                {
                    tile *gulkTile = floor.getTileFromCollisionSpecialID(j);

                    gulkTile->id = -1;
                    gulkTile->collisionID = -1;
                    floor.collision_boxes[j].collisionID = -1;

                    Add(character("./img/char/gulk.png", floor.collision_boxes[j].min_x * 0.16, floor.collision_boxes[j].min_y * 0.16f + 0.2, 0.32, 0.32,
                                  4, 1, CH_GULK));
                    characters[characterCount - 1].setCollider(COLLIDER_SOLID, aabb(characters[characterCount - 1].visual.x,
                                                                                    characters[characterCount - 1].visual.y,
                                                                                    characters[characterCount - 1].visual.x + 0.16,
                                                                                    characters[characterCount - 1].visual.y + 0.24));
                    characters[characterCount - 1].colliderOn(COLLIDER_SOLID);
                    characters[characterCount - 1].scaleCollider(COLLIDER_STRIKE, 0.32, 0.16);
                }
                break;
            case 12:
                for (int x = 0; x < floor.roomWidth; ++x)
                {
                    for (int y = 0; y < floor.roomHeight; ++y)
                    {
                        if (floor.tiles[x][y].specialTileID == floor.collision_boxes[j].specialTileID)
                        {
                            floor.tiles[x][y].id = -1;
                            floor.tiles[x][y].collisionID = -1;
                            floor.collision_boxes[j].collisionID = -1;

                            Add(character("./img/char/coffeemugguy.png", x * 0.16, y * 0.16f + 0.2, 0.32, 0.32,
                                          5, 1, CH_COFFEEMUGGUY));
                            characters[characterCount - 1].setCollider(COLLIDER_SOLID, aabb(characters[characterCount - 1].visual.x,
                                                                                            characters[characterCount - 1].visual.y,
                                                                                            characters[characterCount - 1].visual.x + 0.16,
                                                                                            characters[characterCount - 1].visual.y + 0.24));
                            characters[characterCount - 1].SetAnimation(ANIM_ABILITY_0, 2, 2, 0.0);
                            characters[characterCount - 1].colliderOn(COLLIDER_SOLID);
                        }
                    }
                }
                break;
            default:
                if (insideCollision)
                {
                    if (xNormal != 0.0)
                    {
                        characters[i].visual.x = firstCollisionHitTest;
                        characters[i].velocityX = 0.0;
                    }
                    if (yNormal != 0.0)
                    {
                        characters[i].visual.y = firstCollisionHitTest;
                        characters[i].velocityY = 0.0;
                    }
                    if (yNormal > 0.0)
                        characters[i].onGround = true;
                }
                break;
            }

            if (insideCollision && std::abs(characters[i].velocityX) < 0.5 && xNormal > 0.0 && characters[i].id == CH_GULK)
            {
                characters[i].velocityX = 0.5;
            }
            if (insideCollision && std::abs(characters[i].velocityX) < 0.5 && xNormal < 0.0 && characters[i].id == CH_GULK)
            {
                characters[i].velocityX = -0.5;
            }
        }
        characters[i].updatePosition(delta_time);
    }

    if (!particlesenabled)
        return;

    for (int i = 0; i < particlesystemcount; ++i)
    {
        if (particles[i].totalParticlesSpawned < particles[i].particle_count)
        {
            particles[i].spawn(delta_time);
        }
        particles[i].update(delta_time);
        if (particles[i].particles_alive > 0)
        {
            particles[i].draw(particle_program, particle_sprite, delta_time);
        }

        if (particles[i].particles_alive <= 0)
        {
            removeParticles(i);
        }
    }
}

void game_system::killParticles()
{
    for (int i = 0; i < particlesystemcount; ++i)
    {
        particles[i].kill();
    }
}