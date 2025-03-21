#include "../headers/system.h"

#define MINIAUDIO_IMPLEMENTATION
#include "../headers/miniaudio.h"

// soundhandler soundplayer;

/**
    PAD_LSTICK_LEFT,
    PAD_LSTICK_RIGHT,
    PAD_LSTICK_UP,
    PAD_LSTICK_DOWN,
    PAD_RSTICK_LEFT,
    PAD_RSTICK_RIGHT,
    PAD_RSTICK_UP,
    PAD_RSTICK_DOWN,
    PAD_TRIGGER_L,
    PAD_TRIGGER_R */
int gamepad_stick_sensitivity = 500;
bool player::getInput(GLFWwindow *window, controlset action)
{
    if (gamepad_id > -1)
    {
        // int count;
        // const unsigned char *gamepadbuttons = glfwGetJoystickButtons(gamepad_id, &count);
        GLFWgamepadstate state;
        glfwGetGamepadState(gamepad_id, &state);

        float realSensitivity = static_cast<float>(gamepad_stick_sensitivity) * 0.001f;

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
    // visual.rect.setOrigin(sf::Vector2(static_cast<float>(visual.spriteW) * 0.5f, static_cast<float>(visual.spriteH)));
    // posX = visual.rect.getPosition().x;
    // posY = visual.rect.getPosition().y;
    // walkToX = posX;
    // walkToY = posY;
    id = _id;
    hp = maxhp;

    // collider = aabb(visual.x - visual.spriteW * 0.5f, visual.y - visual.spriteH,
    //                 visual.x + visual.spriteW * 0.5f, visual.y);
    collider = aabb(visual.x, visual.y, visual.x + 0.16f, visual.y + 0.24);
    // collider = aabb(visual.x, visual.y, visual.x + 0.01f, visual.y + 0.01f);
}
character::character(std::string filepath, float x, float y, unsigned int fx, unsigned int fy, IDENTIFICATION _id)
{
    // visual = sprite(filepath.c_str(), x, y, fx, fy);
    // visual.rect.setTextureRect(sf::IntRect(0, 0, visual.spriteW, visual.spriteH));
    // visual.rect.setOrigin(sf::Vector2(static_cast<float>(visual.spriteW) * 0.5f, static_cast<float>(visual.spriteH)));
    visual = sprite(filepath.c_str(), fx, fy);
    visual.Put(x, y, 0.0f);
    // posX = visual.rect.getPosition().x;
    // posY = visual.rect.getPosition().y;
    // walkToX = posX;
    // walkToY = posY;
    id = _id;
    hp = maxhp;
    collider = aabb(visual.x, visual.y, visual.x + 0.16f, visual.y + 0.24f);
}

void character::MoveTo(float _x, float _y, dungeon *currentDungeon)
{
    if (currentDungeon != nullptr)
    {
        _x -= currentDungeon->spawnLocationX;
        _y -= currentDungeon->spawnLocationY;
    }

    // walkToX = _x;
    // walkToY = _y;
}

void character::Update(float delta_time)
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
    collider.Put(visual.x, visual.y, 0.16f, 0.24f);

    if (hp <= 0)
    {
        // visual.rect.setColor(sf::Color(50, 50, 50, 255));
        // visual.rect.setRotation(90);
        visual.Rotate(0.0f, 0.0f, 90.0f);
        // visual.Put(posX + screenOffsetX, posY + screenOffsetY);
        // visual.Move(screenOffsetX, screenOffsetY);
        return;
    }

    if (animationFinished)
    {
        PlayAnimation(ANIM_IDLE, delta_time, true);
    }

    animations[playingAnim].run(delta_time, animationLooping);

    // visual.Put(posX + screenOffsetX, posY + screenOffsetY);

    animationFinished = false;

    if (animations[playingAnim].finished)
        animationFinished = true;

    onGround = false;
}
void character::updatePosition(float delta_time)
{
    if (parrySuccess)
    {
        return;
    }
    visual.Move(velocityX * delta_time, velocityY * delta_time, 0.0f);
}

void character::SetAnimation(ANIMATION_MAPPINGS id, unsigned int s, unsigned int e, float spd)
{
    animations[id] = animation(&visual, s, e, spd);
}
void character::PlayAnimation(ANIMATION_MAPPINGS id, float delta_time, bool loops)
{
    if (animations[id]._sprite == nullptr)
        return;

    if (playingAnim != id)
    {
        animations[id].frame = animations[id].start;
        animations[id].timer = 10.0f;
    }
    playingAnim = id;
    animationLooping = loops;
}
void character::StopAnimation(ANIMATION_MAPPINGS id)
{
    animations[id].finished = true;
    animations[id].timer = 0.0f;
    animations[id].frame = animations[id].end;
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
} // quadtree, finishing attack system and targetting, character creation and input to the main player.
void game_system::Add(character *e)
{
    characters[characterCount] = e;
    sortedSprites[characterCount] = &characters[characterCount]->visual;
    ++characterCount;
}
void game_system::Add(particlesystem *p)
{
    particles[particlesystemcount] = p;
    ++particlesystemcount;
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

// void game_system::handleMusic()
// {
//     if (game_music.getDuration() == sf::Time::Zero)
//     {
//         return;
//     }
//     if (!music_playing || game_music.getStatus() == sf::Music::Stopped)
//     {
//         game_music.play();
//         music_playing = true;
//     }
// }
// void handleMusic();
void game_system::loopSound(unsigned int id)
{
    // code that makes id sound loop if playing
}
void game_system::initSound(const char *path, unsigned int id, ma_engine *engine)
{
    if (game_sounds[id].pDataSource != nullptr)
    {
        if (sound_paths[id] != path)
        {
            ma_sound_uninit(&game_sounds[id]);
        }
        else
        {
            return;
        }
    }
    sound_paths[id] = path;
    game_sound_result = ma_sound_init_from_file(engine, sound_paths[id], 0, NULL, NULL, &game_sounds[id]);
    if (game_sound_result != MA_SUCCESS)
    {
        std::cout << game_sound_result << " sound error\n";
    }
}
void game_system::playSound(unsigned int id, int start_time, bool interrupt)
{
    if (!interrupt && ma_sound_is_playing(&game_sounds[id]))
        return;

    if (id < sound_is_music_cutoff)
    {
        ma_sound_set_volume(&game_sounds[id], static_cast<float>(music_volume) / 100.0f);
    }
    else
    {
        ma_sound_set_volume(&game_sounds[id], static_cast<float>(sound_volume) / 100.0f);
    }
    ma_sound_seek_to_pcm_frame(&game_sounds[id], start_time);
    ma_sound_start(&game_sounds[id]);
}
void game_system::stopSound(unsigned int id)
{
    ma_sound_stop(&game_sounds[id]);
}

void game_system::update(dungeon &floor, float delta_time)
{
    // if (paused)
    // {
    //     for (int i = 0; i < characterCount; ++i)
    //     {
    //         characters[i]->visual.Put(characters[i]->visual.x, characters[i]->visual.y, 0.0f);
    //     }
    //     return;
    // }

    // quicksortSprites(sortedSprites, 0, characterCount - 1);
    for (int i = 0; i < characterCount; ++i)
    {
        for (int j = 0; j < characterCount; ++j)
        {
            if (i == j)
                continue;

            switch (characters[i]->id)
            {
            case CH_PLAYER:
                break;
            case CH_MONSTER:
                break;
            }
        }
        characters[i]->Update(delta_time);
        if (!characters[i]->onGround)
        {
            characters[i]->velocityY -= 10.0f * delta_time;
        }

        for (int j = 0; j < floor.collision_box_count; ++j)
        {
            if (floor.collision_boxes[j].min_x == 0.0f &&
                floor.collision_boxes[j].min_y == 0.0f &&
                floor.collision_boxes[j].max_x == 0.0f &&
                floor.collision_boxes[j].max_y == 0.0f)
            {
                continue;
            }

            float xNormal = 0.0f, yNormal = 0.0f;

            float firstCollisionHitTest = characters[i]->collider.response(characters[i]->velocityX * delta_time,
                                                                           characters[i]->velocityY * delta_time,
                                                                           0.0f, 0.0f, floor.collision_boxes[j], xNormal, yNormal);

            // if (firstCollisionHitTest < 1.0f)
            // {
            //     std::cout << floor.collision_boxes[j].collisionID << ", " << floor.collision_boxes[j].specialTileID << " collision\n";
            // }

            switch (floor.collision_boxes[j].collisionID)
            {
            case 1:
                if (xNormal != 0.0f)
                    characters[i]->velocityX *= firstCollisionHitTest;
                if (yNormal != 0.0f)
                {
                    if (!characters[i]->parrySuccess)
                        characters[i]->hp = 0;
                }
                break;
            case 2:
                if (firstCollisionHitTest < 1.0f && characters[i]->plControl != nullptr)
                {
                    levelincreasing = true;
                }
                break;
            case 3:
                if (yNormal != 0.0f && characters[i]->velocityY <= 0.0f)
                {
                    // if (characters[i]->plControl == nullptr || characters[i]->plControl != nullptr && characters[i]->animations[ANIM_ABILITY_0].finished)
                    // {
                    characters[i]->velocityY *= firstCollisionHitTest;
                    characters[i]->onGround = true;
                    // }
                }
                break;
            case 4:
                if (xNormal != 0.0f || yNormal != 0.0f)
                {
                    characters[i]->velocityX *= 6.0f;
                    characters[i]->velocityY = 0.2f;
                    characters[i]->visual.Put(floor.collision_boxes[j].min_x, floor.collision_boxes[j].min_y, 0.0f);
                }
                break;
            case 7:
                if (yNormal != 0.0f && characters[i]->velocityY < 0.0f)
                {
                    characters[i]->velocityY = 4.0f;
                }
                break;
            case 5:
                if (firstCollisionHitTest < 1.0f)
                {
                    for (int x = 0; x < floor.roomWidth; ++x)
                    {
                        for (int y = 0; y < floor.roomHeight; ++y)
                        {
                            if (floor.tiles[x][y].specialTileID == floor.collision_boxes[j].specialTileID)
                            {
                                // std::cout << floor.tiles[x][y].specialTileID << " correct\n";
                                floor.tiles[x][y].id = 5;
                                floor.tiles[x][y].collisionID = -1;
                            }

                            if (floor.tiles[x][y].collisionID != 6)
                                continue;

                            floor.tiles[x][y].id = -1;
                            floor.tiles[x][y].collisionID = -1;
                        }
                    }
                    for (int x = 0; x < floor.collision_box_count; ++x)
                    {
                        if (floor.collision_boxes[x].collisionID == 6)
                        {
                            floor.collision_boxes[x].collisionID = -1;
                            floor.collision_boxes[x] = aabb(-100.0f, -100.0f, -100.0f, -100.0f);
                        }
                    }
                }
                break;
            default:
                if (yNormal != 0.0f)
                    characters[i]->velocityY *= firstCollisionHitTest;
                if (xNormal != 0.0f)
                    characters[i]->velocityX *= firstCollisionHitTest;
                if (yNormal < 0.0f)
                {
                    characters[i]->onGround = true;
                }
                break;
            }
        }
        characters[i]->updatePosition(delta_time);
    }

    for (int i = 0; i < particlesystemcount; ++i)
    {
        particles[i]->spawn(delta_time);
        particles[i]->update(delta_time);
    }
}

void game_system::killParticles()
{
    for (int i = 0; i < particlesystemcount; ++i)
    {
        particles[i]->kill();
    }
}