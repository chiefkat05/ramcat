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
character::character(shader *program, object *sprite_object, std::string filepath, double x, double y, double z, unsigned int fx, unsigned int fy, IDENTIFICATION _id)
{
    visual = sprite(program, sprite_object, filepath.c_str(), fx, fy);
    visual.Put(x, y, z);
    visual.Scale(visual.trueW(), visual.trueH(), 0.1);
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
    visual.Move(velocityX * pixel_scale * delta_time, velocityY * pixel_scale * delta_time, 0.0);
    for (int i = 0; i < character_collider_limit; ++i)
    {
        putCollider(static_cast<COLLIDER_BOX_IDS>(i), visual.x + colliders[i].xOffsetFromParent, visual.y + colliders[i].yOffsetFromParent);
    }

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

validCollisionType getCollisionType(unsigned int idA, unsigned int idB)
{
    if (idA == COLLIDER_SOLID && idB == COLLIDER_SOLID)
        return VCT_SOLID_SOLID;
    if (idA == COLLIDER_STRIKE && idB == COLLIDER_SOLID)
        return VCT_STRIKE_SOLID;
    if (idA == COLLIDER_SOLID && idB == COLLIDER_STRIKE)
        return VCT_SOLID_STRIKE;

    return VCT_INVALID;
}
void game_system::particle_update(double delta_time)
{
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
            particles[i].draw(delta_time);
        }

        if (particles[i].particles_alive <= 0)
        {
            removeParticles(i);
        }
    }
}

void game_system::light_update()
{
    // lighting test
    for (int i = 0; i < light_count; ++i)
    {
        light_list[i].update_values();
        std::string uniformName = "lights[" + std::to_string(i) + "]";
        shaders[GAME_SHADER_DEFAULT]->setUniformInt(std::string(uniformName + ".ltype").c_str(), light_list[i].ltype);
        shaders[GAME_SHADER_DEFAULT]->setUniformVec3(std::string(uniformName + ".position").c_str(), light_list[i].position);
        shaders[GAME_SHADER_DEFAULT]->setUniformVec3(std::string(uniformName + ".direction").c_str(), light_list[i].direction);
        shaders[GAME_SHADER_DEFAULT]->setUniformVec3(std::string(uniformName + ".color").c_str(), light_list[i].color);
        shaders[GAME_SHADER_DEFAULT]->setUniformDouble(std::string(uniformName + ".ambient").c_str(), light_list[i].ambient);
        shaders[GAME_SHADER_DEFAULT]->setUniformDouble(std::string(uniformName + ".diffuse").c_str(), light_list[i].diffuse);
        shaders[GAME_SHADER_DEFAULT]->setUniformDouble(std::string(uniformName + ".specular").c_str(), light_list[i].specular);
        shaders[GAME_SHADER_DEFAULT]->setUniformDouble(std::string(uniformName + ".falloff_constant").c_str(), light_list[i].falloff_constant);
        shaders[GAME_SHADER_DEFAULT]->setUniformDouble(std::string(uniformName + ".falloff_linear").c_str(), light_list[i].falloff_linear);
        shaders[GAME_SHADER_DEFAULT]->setUniformDouble(std::string(uniformName + ".falloff_quadratic").c_str(), light_list[i].falloff_quadtratic);
        shaders[GAME_SHADER_DEFAULT]->setUniformDouble(std::string(uniformName + ".falloff_scale").c_str(), light_list[i].falloff_scale);
        shaders[GAME_SHADER_DEFAULT]->setUniformDouble(std::string(uniformName + ".cutoff").c_str(), light_list[i].cutoff);
    }
    shaders[GAME_SHADER_DEFAULT]->setUniformInt("light_count", light_count);
}
void game_system::update(world &floor, double delta_time)
{
    bool floorTilesNeedUpdate = false;
    for (int i = 0; i < floor.tileAnimationCount; ++i)
    {
        if (floor.tileAnimations[i]._sprite == nullptr)
            continue;

        floor.tileAnimations[i].run(delta_time, true, false, &floorTilesNeedUpdate);
    }
    if (floorTilesNeedUpdate)
    {
        floor.updateTileTextures(*objects[GAME_OBJECT_TILEMAP]);
    }
    if (floor.tileColorsNeedUpdate)
    {
        floor.updateTileColors(*objects[GAME_OBJECT_TILEMAP]);
    }

    for (int i = 0; i < characterCount; ++i)
    {
        characters[i].Update(delta_time);

        if (!characters[i].onGround)
        {
            characters[i].velocityY -= 1000.0 * delta_time;
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
                    bool collision = false;
                    double firstCollisionHitTest = characters[i].colliders[k].response(characters[i].velocityX * delta_time,
                                                                                       characters[i].velocityY * delta_time,
                                                                                       characters[j].velocityX * delta_time,
                                                                                       characters[j].velocityY * delta_time,
                                                                                       characters[j].colliders[l], xNormal, yNormal,
                                                                                       characters[i].visual.x, characters[i].visual.y,
                                                                                       collision, distanceToClosestSide);

                    validCollisionType cType = getCollisionType(k, l);
                    if (collision)
                        handleCollisionSpecifics(characters[i], characters[j], cType, xNormal, yNormal, firstCollisionHitTest, delta_time);
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
            bool collision = false; // now for taking these apart into more managable functions!
            // something like
            // bool walkableInterior = (distanceToClosestSide >= -0.01);
            // and
            // void respondCollision(int collisionID, bool walkableInterior = true, bool groundBlock = true, replaceOnSpawnWith(GULK));

            double firstCollisionHitTest = characters[i].colliders[COLLIDER_SOLID].response(characters[i].velocityX * pixel_scale * delta_time,
                                                                                            characters[i].velocityY * pixel_scale * delta_time, 0.0, 0.0,
                                                                                            floor.collision_boxes[j], xNormal, yNormal, characters[i].visual.x,
                                                                                            characters[i].visual.y, collision, distanceToClosestSide);

            tile *thisSpecialTile = &floor.tiles[floor.collision_boxes[j].specialTileX][floor.collision_boxes[j].specialTileY];
            switch (floor.collision_boxes[j].collisionID)
            {
            case -1:
                break;
            case 1:
                if (collision)
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
                if (collision && characters[i].plControl != nullptr)
                {
                    levelincreasing = true;
                }
                break;
            case 3:
                if (collision && yNormal > 0.0 && distanceToClosestSide >= -0.01 && characters[i].velocityY < 0.0)
                {
                    characters[i].visual.y = firstCollisionHitTest;
                    characters[i].velocityY = 0.0;
                    characters[i].onGround = true;
                }
                break;
            case 4:
                if (collision && xNormal < 0.0 && distanceToClosestSide >= -0.01 && characters[i].velocityX > 0.0)
                {
                    characters[i].visual.x = firstCollisionHitTest;
                    characters[i].velocityX = 0.0;
                }
                break;
            case 5:
                if (collision && yNormal < 0.0 && distanceToClosestSide >= -0.01 && characters[i].velocityY > 0.0)
                {
                    characters[i].visual.y = firstCollisionHitTest;
                    characters[i].velocityY = 0.0;
                }
                break;
            case 6:
                if (collision && xNormal > 0.0 && distanceToClosestSide >= -0.01 && characters[i].velocityX < 0.0)
                {
                    characters[i].visual.x = firstCollisionHitTest;
                    characters[i].velocityX = 0.0;
                }
                break;
            case 7:
                if (collision && yNormal != 0.0 && characters[i].velocityY < 0.0)
                {
                    characters[i].velocityY = 4.0;
                }
                break;
            case 8:
                if (collision)
                {
                    floor.removeTileAnimation(thisSpecialTile->animationIndexID);
                    thisSpecialTile->emptyTile();
                    floor.collision_boxes[j].collisionID = -1;
                    ++fishCollected;
                    thisSpecialTile->colorIndexID = 0;
                    floor.tileColorsNeedUpdate = true;
                }
                break;
            case 9:
                if (collision)
                {
                    thisSpecialTile->id = 2;
                    thisSpecialTile->collisionID = -1;
                    floor.collision_boxes[j].collisionID = -1;

                    floor.spawnLocationX = floor.collision_boxes[j].specialTileX * floor.worldSprite.trueW();
                    floor.spawnLocationY = (-static_cast<double>(floor.roomHeight) + floor.collision_boxes[j].specialTileY) * floor.worldSprite.trueW();
                    // floor.spawnLocationX = floor.collision_boxes[j].centerX();
                    // floor.spawnLocationY = floor.collision_boxes[j].centerY();
                }
                break;
            case 10:
                if (collision)
                {
                    floor.removeTileAnimation(thisSpecialTile->animationIndexID);
                    thisSpecialTile->emptyTile();
                    floor.collision_boxes[j].collisionID = -1;
                    thisSpecialTile->colorIndexID = 0;
                    floor.tileColorsNeedUpdate = true;
                }
                break;
            case 11:
            {
                thisSpecialTile->id = -1;
                thisSpecialTile->collisionID = -1;
                floor.collision_boxes[j].collisionID = -1;

                // Add(character("./img/char/gulk.png", floor.collision_boxes[j].min_x * floor.worldSprite.trueW(),
                //               floor.collision_boxes[j].min_y * floor.worldSprite.trueH() + 0.2, 4, 1, CH_GULK));
                Add(character(shaders[GAME_SHADER_DEFAULT], objects[GAME_OBJECT_DEFAULT], "./img/char/gulk.png", floor.collision_boxes[j].min_x * floor.worldSprite.trueW(),
                              floor.collision_boxes[j].centerY() * floor.worldSprite.trueH() + 0.2, 0.5, 4, 1, CH_GULK));
                characters[characterCount - 1].setCollider(COLLIDER_SOLID, aabb(characters[characterCount - 1].visual.x, characters[characterCount - 1].visual.y, characters[characterCount - 1].visual.x + 0.16, characters[characterCount - 1].visual.y + 0.24), 0.0, 0.0);
                characters[characterCount - 1].colliderOn(COLLIDER_SOLID);
                characters[characterCount - 1].scaleCollider(COLLIDER_STRIKE, 0.32, 0.16);
            }
            break;
            case 12:
            {
                // tile *npcTile = floor.getTileFromCollisionSpecialID(floor.collision_boxes[j].specialTileID);
                thisSpecialTile->id = -1;
                thisSpecialTile->collisionID = -1;
                floor.collision_boxes[j].collisionID = -1;
                Add(character(shaders[GAME_SHADER_DEFAULT], objects[GAME_OBJECT_DEFAULT], "./img/char/coffeemugguy.png", floor.collision_boxes[j].min_x * floor.worldSprite.trueW(),
                              floor.collision_boxes[j].min_y * floor.worldSprite.trueH() + 0.2, 0.5, 5, 1, CH_COFFEEMUGGUY));
                // Add(character("./img/char/coffeemugguy.png", floor.collision_boxes[j].min_x * floor.worldSprite.trueW(),
                //               floor.collision_boxes[j].centerY() * floor.worldSprite.trueH() + 0.2, 5, 1, CH_COFFEEMUGGUY)); FIX
                characters[characterCount - 1].setCollider(COLLIDER_SOLID, aabb(characters[characterCount - 1].visual.x, characters[characterCount - 1].visual.y, characters[characterCount - 1].visual.x + 0.16, characters[characterCount - 1].visual.y + 0.24), 0.0, 0.0);
                characters[characterCount - 1].SetAnimation(ANIM_ABILITY_0, 2, 2, 0.0);
                characters[characterCount - 1].colliderOn(COLLIDER_SOLID);
            }
            break;
            default:
                if (collision)
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

            if (collision && std::abs(characters[i].velocityX) < 0.5 && xNormal > 0.0 && characters[i].id == CH_GULK)
            {
                characters[i].velocityX = 0.5;
            }
            if (collision && std::abs(characters[i].velocityX) < 0.5 && xNormal < 0.0 && characters[i].id == CH_GULK)
            {
                characters[i].velocityX = -0.5;
            }
        }
        characters[i].updatePosition(delta_time);
    }
}

void game_system::killParticles()
{
    for (int i = 0; i < particlesystemcount; ++i)
    {
        particles[i].kill();
    }
}