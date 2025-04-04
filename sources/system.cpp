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
                break;
            case PAD_LSTICK_RIGHT:
                return (state.axes[0] > realSensitivity);
                break;
            case PAD_LSTICK_DOWN:
                return (state.axes[1] > realSensitivity);
                break;
            case PAD_LSTICK_UP:
                return (state.axes[1] < -realSensitivity);
                break;
            case PAD_RSTICK_LEFT:
                return (state.axes[2] < -realSensitivity);
                break;
            case PAD_RSTICK_RIGHT:
                return (state.axes[2] > realSensitivity);
                break;
            case PAD_RSTICK_DOWN:
                return (state.axes[3] > realSensitivity);
                break;
            case PAD_RSTICK_UP:
                return (state.axes[3] < -realSensitivity);
                break;
            case PAD_TRIGGER_L:
                return (state.axes[4] > realSensitivity);
                break;
            case PAD_TRIGGER_R:
                return (state.axes[5] > realSensitivity);
                break;
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
    // visual.rect.setTextureRect(sf::IntRect(0, 0, visual.spriteW, visual.spriteH));
    // visual.rect.setOrigin(sf::Vector2(static_cast<double>(visual.spriteW) * 0.5f, static_cast<double>(visual.spriteH)));
    // posX = visual.rect.getPosition().x;
    // posY = visual.rect.getPosition().y;
    // walkToX = posX;
    // walkToY = posY;
    id = _id;
    hp = maxhp;

    collider = aabb(visual.x, visual.y, visual.x + 0.16, visual.y + 0.16);
}
character::character(std::string filepath, double x, double y, double w, double h, unsigned int fx, unsigned int fy,
                     double cOffX, double cOffY, double cW, double cH, IDENTIFICATION _id)
{
    visual = sprite(filepath.c_str(), fx, fy);
    visual.Put(x, y, 0.0);
    visual.Scale(w, h, 0.1);
    id = _id;
    hp = maxhp;
    colOffsetX = cOffX;
    colOffsetY = cOffY;
    colW = cW;
    colH = cH;
    collider = aabb(visual.x + colOffsetX, visual.y + colOffsetY, visual.x + colOffsetX + colW, visual.y + colOffsetY + colH);

    switch (id)
    {
    case CH_GULK:
        velocityX = -0.5;
        break;
    default:
        break;
    }
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
    // collider.moveCenterToPoint(visual.x, visual.y);
    // collider.min_x = visual.x - visual.spriteW * 0.5f;
    // collider.max_x = visual.x + visual.spriteW * 0.5f;
    // collider.min_y = visual.y - visual.spriteH;
    // collider.max_y = visual.y;
    // collider.min_x = visual.x;
    // collider.max_x = visual.x + 0.01f;
    // collider.min_y = visual.y;
    // collider.max_y = visual.y + 0.01f;
    // collider = aabb(visual.x, visual.y - 0.16f, visual.x + 0.16f, visual.y);
    // collider.min_x = visual.x;
    // collider.max_x = visual.x + 0.16f;
    // collider.min_y = visual.y;
    // collider.max_y = visual.y + 0.16f;
    collider.Put(visual.x, visual.y - 0.08, 0.16, 0.24);
    // collider.Move(visual.x, visual.y);

    if (hp <= 0)
    {
        // visual.rect.setColor(sf::Color(50, 50, 50, 255));
        // visual.rect.setRotation(90);
        visual.Rotate(0.0, 0.0, 90.0);
        // visual.Put(posX + screenOffsetX, posY + screenOffsetY);
        // visual.Move(screenOffsetX, screenOffsetY);
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
}
void character::updatePosition(double delta_time)
{
    if (parrySuccess)
    {
        return;
    }
    visual.Move(velocityX * delta_time, velocityY * delta_time, 0.0);
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
    animations[id].frame = animations[id].end; // fix collision stuff in character creation
}

int qsPartition(sprite *sprites[entity_limit], int low, int high) // see if there's a better optimized way to do this, such as random pivot
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
void quicksortSprites(sprite *sprites[entity_limit], int low, int high)
{
    if (low < high)
    {
        int pi = qsPartition(sprites, low, high);

        quicksortSprites(sprites, low, pi - 1);
        quicksortSprites(sprites, pi + 1, high);
    }
}

void game_system::Add(character *e)
{
    characters[characterCount] = e;
    sortedSprites[characterCount] = &characters[characterCount]->visual;
    ++characterCount;
}
// void game_system::Spawn_Enemy(std::string filepath, IDENTIFICATION _id, double x, double y, double scaleX, double scaleY, unsigned int fx, unsigned int fy, double cOffX, double cOffY, double cW, double cH)
void game_system::Spawn_Enemy(character e)
{
    for (int i = 0; i < enemyCount; ++i)
    {
        for (int j = 0; j < characterCount; ++j)
        {
            if (&enemies[i] == characters[j])
            {
                return;
            }
        }
    }
    enemies[enemyCount] = e;
    enemies[enemyCount].plControl = nullptr;
    // enemies[enemyCount].visual = sprite(filepath.c_str(), fx, fy);
    // enemies[enemyCount].visual.Put(x, y, 0.0);
    // enemies[enemyCount].id = _id;
    // enemies[enemyCount].hp = enemies[enemyCount].maxhp;
    // enemies[enemyCount].collider = aabb(enemies[enemyCount].visual.x, enemies[enemyCount].visual.y,
    //                                     enemies[enemyCount].visual.x + 0.16, enemies[enemyCount].visual.y + 0.16);
    // enemies[enemyCount].visual.Scale(scaleX, scaleY, 1.0);

    enemies[enemyCount].SetAnimation(ANIM_IDLE, 0, 0, 0.0);
    enemies[enemyCount].SetAnimation(ANIM_WALK, 0, 0, 100.0);
    enemies[enemyCount].SetAnimation(ANIM_HURT, 0, 0, 250.0);
    enemies[enemyCount].SetAnimation(ANIM_DEAD, 0, 0, 100.0);
    enemies[enemyCount].SetAnimation(ANIM_ABILITY_2, 0, 0, 100.0);
    enemies[enemyCount].SetAnimation(ANIM_ABILITY_0, 0, 0, 100.0);
    enemies[enemyCount].SetAnimation(ANIM_ABILITY_1, 0, 0, 100.0);

    Add(&enemies[enemyCount]);
    ++enemyCount;
}
void game_system::Remove(character *e)
{
    int removeIndex = -1;
    for (int i = 0; i < characterCount; ++i)
    {
        if (characters[i] == e)
        {
            removeIndex = i;
            break;
        }
    }
    if (removeIndex == -1)
    {
        std::cout << "\n\tWarning - character not found in game_system and could not be removed.\n";
        return;
    }

    for (int i = removeIndex; i < characterCount; ++i)
    {
        characters[i] = characters[i + 1];
    }
    --characterCount;
}
void game_system::Delete_Enemy(int index)
{
    for (int i = index; i < enemyCount; ++i)
    {
        enemies[i] = enemies[i + 1];
    }
    --enemyCount;
}
void game_system::Clear_Enemies()
{
    enemyCount = 0;
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
    // if (paused)
    // {
    //     for (int i = 0; i < characterCount; ++i)
    //     {
    //         characters[i]->visual.Put(characters[i]->visual.x, characters[i]->visual.y, 0.0);
    //     }
    //     return;
    // }

    // quicksortSprites(sortedSprites, 0, characterCount - 1);
    for (int i = 0; i < characterCount; ++i)
    {
        characters[i]->Update(delta_time);

        // in-game character updates
        switch (characters[i]->id)
        {
        case CH_GULK:
            break;
        }

        if (!characters[i]->onGround)
        {
            characters[i]->velocityY -= 10.0 * delta_time;
        }

        for (int j = 0; j < characterCount; ++j)
        {
            double xNormal = 0.0, yNormal = 0.0;
            double firstCollisionHitTest = characters[i]->collider.response(characters[i]->velocityX * delta_time,
                                                                            characters[i]->velocityY * delta_time,
                                                                            0.0, 0.0, characters[j]->collider, xNormal, yNormal);

            if (yNormal != 0.0)
                characters[i]->velocityY *= firstCollisionHitTest;
            if (xNormal != 0.0)
                characters[i]->velocityX *= firstCollisionHitTest;
            if (xNormal == 0.0 && yNormal == 0.0)
            {
                characters[i]->velocityY *= firstCollisionHitTest;
                characters[i]->velocityX *= firstCollisionHitTest;
            }
            if (yNormal > 0.0)
            {
                characters[i]->onGround = true;
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

            double xNormal = 0.0, yNormal = 0.0;

            double firstCollisionHitTest = characters[i]->collider.response(characters[i]->velocityX * delta_time,
                                                                            characters[i]->velocityY * delta_time,
                                                                            0.0, 0.0, floor.collision_boxes[j], xNormal, yNormal);

            switch (floor.collision_boxes[j].collisionID)
            {
            case -1:
                break;
            case 1:
                if (xNormal != 0.0)
                    characters[i]->velocityX *= firstCollisionHitTest;
                if (yNormal != 0.0)
                {
                    if (!characters[i]->parrySuccess)
                        characters[i]->hp = 0;
                }
                if (firstCollisionHitTest < 1.0 && xNormal == 0.0 && yNormal == 0.0)
                {
                    characters[i]->velocityY *= firstCollisionHitTest;
                    characters[i]->velocityX *= firstCollisionHitTest;
                }
                break;
            case 2:
                if (firstCollisionHitTest < 1.0 && characters[i]->plControl != nullptr)
                {
                    levelincreasing = true;
                }
                break;
            case 3:
                if (yNormal > 0.0)
                {
                    characters[i]->velocityY *= firstCollisionHitTest;
                    characters[i]->onGround = true;
                }
                break;
            case 4:
                if (xNormal < 0.0)
                {
                    characters[i]->velocityX *= firstCollisionHitTest;
                    characters[i]->onGround = true;
                }
                break;
            case 5:
                if (yNormal < 0.0)
                {
                    characters[i]->velocityY *= firstCollisionHitTest;
                    characters[i]->onGround = true;
                }
                break;
            case 6:
                if (xNormal > 0.0)
                {
                    characters[i]->velocityX *= firstCollisionHitTest;
                    characters[i]->onGround = true;
                }
                break;
            case 7:
                if (yNormal != 0.0 && characters[i]->velocityY < 0.0)
                {
                    characters[i]->velocityY = 4.0;
                }
                break;
            case 8:
                if (firstCollisionHitTest < 1.0f)
                {
                    for (int x = 0; x < floor.roomWidth; ++x)
                    {
                        for (int y = 0; y < floor.roomHeight; ++y)
                        {
                            if (floor.tiles[x][y].specialTileID == floor.collision_boxes[j].specialTileID)
                            {
                                floor.tiles[x][y].id = -1;
                                floor.tiles[x][y].collisionID = -1;
                            }
                        }
                    }

                    ++fishCollected;
                }
                break;
            case 9:
                if (firstCollisionHitTest < 1.0f)
                {
                    for (int x = 0; x < floor.roomWidth; ++x)
                    {
                        for (int y = 0; y < floor.roomHeight; ++y)
                        {
                            if (floor.tiles[x][y].specialTileID == floor.collision_boxes[j].specialTileID)
                            {
                                floor.tiles[x][y].id = 2;
                                floor.tiles[x][y].collisionID = -1;

                                floor.spawnLocationX = x * 0.16f;
                                floor.spawnLocationY = -0.2f + (-static_cast<double>(floor.roomHeight) + y) * 0.16f;
                            }
                        }
                    }
                }
                break;
            case 10:
                if (firstCollisionHitTest < 1.0f)
                {
                    for (int x = 0; x < floor.roomWidth; ++x)
                    {
                        for (int y = 0; y < floor.roomHeight; ++y)
                        {
                            if (floor.tiles[x][y].specialTileID == floor.collision_boxes[j].specialTileID)
                            {
                                floor.tiles[x][y].id = -1;
                                floor.tiles[x][y].collisionID = -1;
                            }
                        }
                    }
                    characters[i]->runSpeed *= 1.001;
                }
                break;
            case 11:
                for (int x = 0; x < floor.roomWidth; ++x)
                {
                    for (int y = 0; y < floor.roomHeight; ++y)
                    {
                        if (floor.tiles[x][y].specialTileID == floor.collision_boxes[j].specialTileID)
                        {
                            floor.tiles[x][y].id = -1;
                            floor.tiles[x][y].collisionID = -1;
                            floor.collision_boxes[j].collisionID = -1;

                            Spawn_Enemy(character("./img/char/gulk.png", x * 0.16, static_cast<double>(floor.roomHeight - y) * 0.16, 0.32, 0.32,
                                                  4, 1, 0.0, -0.64, 0.16, 0.16, CH_GULK));
                        }
                    }
                }
                break;
            default:
                if (yNormal != 0.0)
                    characters[i]->velocityY *= firstCollisionHitTest;
                if (xNormal != 0.0)
                    characters[i]->velocityX *= firstCollisionHitTest;
                if (firstCollisionHitTest < 1.0 && xNormal == 0.0 && yNormal == 0.0)
                {
                    characters[i]->velocityY *= firstCollisionHitTest;
                    characters[i]->velocityX *= firstCollisionHitTest;
                }
                if (yNormal > 0.0)
                {
                    characters[i]->onGround = true;
                }

                break;
            }

            if (firstCollisionHitTest < 1.0 && std::abs(characters[i]->velocityX) < 0.5 && xNormal > 0.0 && characters[i]->id == CH_GULK)
            {
                characters[i]->velocityX = 0.5;
            }
            if (firstCollisionHitTest < 1.0 && std::abs(characters[i]->velocityX) < 0.5 && xNormal < 0.0 && characters[i]->id == CH_GULK)
            {
                characters[i]->velocityX = -0.5;
            }
        }
        characters[i]->updatePosition(delta_time);
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

        if (particles[i].particles_alive <= 0 && particles[i].totalParticlesSpawned >= particles[i].particle_count)
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