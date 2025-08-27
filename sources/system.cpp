#include "../headers/system.h"

#define MINIAUDIO_IMPLEMENTATION
#include "../headers/miniaudio.h"

int gamepad_stick_sensitivity = 500;
extern double windowAspectDivision;

validCollisionType getCollisionType(unsigned int idA, unsigned int idB);

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
        // PlayAnimation(ANIM_IDLE, delta_time, true);
        break;
    case CH_GULK:
        if (velocityX == 0.0)
        {
            velocityX = 25.0;
        }
        if (playingAnim == ANIM_ABILITY_1 && animations[playingAnim].frame == 5)
        {
            colliderOn(COLLIDER_STRIKE);
        }
        if (colliders[COLLIDER_STRIKE].colliderID != -1 && (playingAnim != ANIM_ABILITY_1 || playingAnim == ANIM_ABILITY_1 && animations[playingAnim].frame != 5))
        {
            colliderOff(COLLIDER_STRIKE);
        }
        // if (colliders[COLLIDER_SOLID].collisionThisFrame && colliders[COLLIDER_SOLID].) // this isn't triggering for some reason
        // {
        //     std::cout << "???\n";
        //     velocityX = -velocityX;
        // } // wouldn't this be better in the collision response function? I think so, try it.
        break;
    default:
        break;
    }

    for (int i = 0; i < character_collider_limit; ++i)
    {
        colliders[i].collisionThisFrame = false;
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

const int max_depth = 6;
void aabb_quadtree::handle_collisions(game_system &game, world &floor, double delta_time)
{
    static aabb_quadtree *ancestor_stack[max_depth] = {nullptr};
    static int depth = 0;

    if (depth >= max_depth)
        return;

    ancestor_stack[depth] = this;
    ++depth;
    for (int i = 0; i < depth; ++i)
    {
        aabb *pA = nullptr, *pB = nullptr;

        for (pA = ancestor_stack[i]->linked_list; pA; pA = pA->next_aabb)
        {
            for (pB = linked_list; pB; pB = pB->next_aabb)
            {
                if (pA == pB)
                    continue; // should be break? would be more complicated collision resolution if it's break but better performance...

                if (pA->parent_object == nullptr && pB->parent_object == nullptr) // yay
                    continue;

                if (pA->parent_object != nullptr && pB->parent_object != nullptr)
                {
                    bool collisionHappening = false;
                    pA->collisionSnapValue = pA->response(pA->parent_object->velocityX * pixel_scale * delta_time,
                                                          pA->parent_object->velocityY * pixel_scale * delta_time,
                                                          pB->parent_object->velocityX * pixel_scale * delta_time,
                                                          pB->parent_object->velocityY * pixel_scale * delta_time,
                                                          *pB, pA->collisionNormalX, pA->collisionNormalY,
                                                          pA->parent_object->visual.x, pA->parent_object->visual.y,
                                                          collisionHappening, pA->collisionDistanceToSide);

                    if (collisionHappening)
                    {
                        pA->collisionThisFrame = true;
                        validCollisionType cType = getCollisionType(pA->colliderID, pB->colliderID);
                        game.handleCharacterCollisions(*pA->parent_object, *pB->parent_object, cType, pA->collisionNormalX, pA->collisionNormalY, pA->collisionSnapValue, delta_time);
                    }
                }

                if (pA->parent_object != nullptr && pB->parent_object == nullptr)
                {
                    game.handleTileCollisions(pA->parent_object, pB, floor, delta_time);
                }
            }
        }
    }

    if (nw != nullptr)
    {
        nw->handle_collisions(game, floor, delta_time);
    }
    if (ne != nullptr)
    {
        ne->handle_collisions(game, floor, delta_time);
    }
    if (sw != nullptr)
    {
        sw->handle_collisions(game, floor, delta_time);
    }
    if (se != nullptr)
    {
        se->handle_collisions(game, floor, delta_time);
    }

    --depth;
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
        characters[characterCount].SetAnimation(ANIM_ABILITY_1, 4, 7, 100.0); // make this sense frames correct (IT WORKS YES CELEBRATE BUT THE FRAMES AND HITBOXES ARE OFF FIX THOSE)
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
    if (idA == COLLIDER_SIGHT && idB == COLLIDER_SOLID)
        return VCT_SIGHT_SOLID;

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
        shaders[GAME_SHADER_DEFAULT]->setUniformDouble(std::string(uniformName + ".outer_cutoff").c_str(), light_list[i].outer_cutoff);
    }
    shaders[GAME_SHADER_DEFAULT]->setUniformInt("light_count", light_count);
}
void game_system::update(world &floor, camera &mainCam, double delta_time)
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

    // test
    // for (int i = 0; i < characterCount; ++i)
    // {
    //     std::cout << i << " character " << characters[i].visual.texture_path << " p = " << &characters[i] << " huh\n";
    // }
    // std::cout << " / " << characterCount << "\n";

    collision_tree = aabb_quadtree(aabb(mainCam.cameraPosition.x - windowAspectDivision, mainCam.cameraPosition.y - 1.0,
                                        mainCam.cameraPosition.x + windowAspectDivision, mainCam.cameraPosition.y + 1.0));
    // aabb_quadtree temp_collision_tree = aabb_quadtree(aabb(mainCam.cameraPosition.x - windowAspectDivision, mainCam.cameraPosition.y - 1.0,
    //                                                        mainCam.cameraPosition.x + windowAspectDivision, mainCam.cameraPosition.y + 1.0));
    // collision_tree.setSprite(shaders[GAME_SHADER_DEFAULT], objects[GAME_OBJECT_DEFAULT]);
    for (int i = 0; i < characterCount; ++i)
    {
        for (int j = 0; j < character_collider_limit; ++j)
        {
            aabb box = characters[i].colliders[j];
            aabb boundingBox(box.min_x + std::min(characters[i].velocityX * delta_time, 0.0), box.min_y + std::min(characters[i].velocityY * delta_time, 0.0),
                             box.max_x + std::max(characters[i].velocityX * delta_time, 0.0), box.max_y + std::max(characters[i].velocityY * delta_time, 0.0));
            collision_tree.insert(&characters[i].colliders[j], boundingBox);
        }
    }
    for (int i = 0; i < floor.collision_box_count; ++i)
    {
        collision_tree.insert(&floor.collision_boxes[i], floor.collision_boxes[i]);
    }
    // collision_tree.draw();

    for (int i = 0; i < characterCount; ++i)
    {
        aabb box = characters[i].colliders[COLLIDER_SOLID];
        aabb boundingBox(box.min_x + std::min(characters[i].velocityX, 0.0), box.min_y + std::min(characters[i].velocityY, 0.0),
                         box.max_x + std::max(characters[i].velocityX, 0.0), box.max_y + std::max(characters[i].velocityY, 0.0));

        if (i > 0 && !boundingBox.colliding(collision_tree.bounds))
            return;

        characters[i].Update(delta_time);

        if (!characters[i].onGround)
        {
            characters[i].velocityY -= 1000.0 * delta_time;
        }
    }

    collision_tree.handle_collisions(*this, floor, delta_time);
    collision_tree.empty(); // should probably make this not update every frame

    for (int i = 0; i < characterCount; ++i)
    {
        aabb box = characters[i].colliders[COLLIDER_SOLID];
        aabb boundingBox(box.min_x + std::min(characters[i].velocityX, 0.0), box.min_y + std::min(characters[i].velocityY, 0.0),
                         box.max_x + std::max(characters[i].velocityX, 0.0), box.max_y + std::max(characters[i].velocityY, 0.0));

        if (i > 0 && !boundingBox.colliding(collision_tree.bounds))
            return;

        characters[i].updatePosition(delta_time);
    }
}

void game_system::killParticles()
{
    for (int i = 0; i < particlesystemcount; ++i)
    {
        particles[i].kill();
    }
    particlesystemcount = 0;
}
// should be: (id, sprite&, particle_count, vec2 pos, vec2 size);
// void game_system::setParticles(std::string path, unsigned int fx, unsigned int fy, unsigned int _particle_count, double _life_lower, double _life_upper,
//                                double sX, double sY, double sW, double sH, glm::vec4 col, unsigned int uniqueID)
void game_system::setParticles(unsigned int uid, sprite v, unsigned int count, glm::vec2 pos, glm::vec2 size)
{
    if (particlesystemcount >= particle_system_limit || !particlesenabled)
        return;

    int workingIndex = -1;
    for (int i = 0; i < particlesystemcount; ++i)
    {
        if (uid == particles[i].id)
        {
            workingIndex = i;
        }
    }

    if (workingIndex != -1)
    {
        particles[workingIndex].totalParticlesSpawned = 0;
        return;
    }

    particles[particlesystemcount] = particlesystem(v.texture_path.c_str(), shaders[GAME_SHADER_DEFAULT], objects[GAME_OBJECT_PARTICLE],
                                                    v.framesX, v.framesY, count);

    particles[particlesystemcount].variables[PV_LIFE_LOW] = 1.0;
    particles[particlesystemcount].variables[PV_LIFE_HIGH] = 1.0;
    particles[particlesystemcount].variables[PV_SPAWN_X] = pos.x;
    particles[particlesystemcount].variables[PV_SPAWN_Y] = pos.y;
    particles[particlesystemcount].variables[PV_WIDTH] = size.x;
    particles[particlesystemcount].variables[PV_HEIGHT] = size.y;
    particles[particlesystemcount].variables[PV_SPAWN_X2] = pos.x;
    particles[particlesystemcount].variables[PV_SPAWN_Y2] = pos.y;
    particles[particlesystemcount].variables[PV_RED] = v.colr;
    particles[particlesystemcount].variables[PV_GREEN] = v.colg;
    particles[particlesystemcount].variables[PV_BLUE] = v.colb;
    particles[particlesystemcount].variables[PV_ALPHA] = v.cola;

    particles[particlesystemcount].id = uid;
    ++particlesystemcount;
}
void game_system::removeParticles(unsigned int index)
{
    particles[index] = particlesystem();
    for (int i = 0; i < pv_variable_limit; ++i)
    {
        particles[index].variable_pointers[i] = nullptr;
    }

    for (int i = index; i < particlesystemcount - 1; ++i)
    {
        particles[i] = particles[i + 1];
    }
    particles[particlesystemcount] = particlesystem();
    for (int i = 0; i < pv_variable_limit; ++i)
    {
        particles[particlesystemcount].variable_pointers[i] = nullptr;
    }
    --particlesystemcount;
}