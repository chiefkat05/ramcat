#include "../headers/system.h"

#define MINIAUDIO_IMPLEMENTATION
#include "../headers/miniaudio.h"

// soundhandler soundplayer;

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

    collider = aabb(visual.x - visual.spriteW * 0.5f, visual.y - visual.spriteH,
                    visual.x + visual.spriteW * 0.5f, visual.y);
}
character::character(object *spriteObject, std::string filepath, float x, float y, unsigned int fx, unsigned int fy, IDENTIFICATION _id)
{
    // visual = sprite(filepath.c_str(), x, y, fx, fy);
    // visual.rect.setTextureRect(sf::IntRect(0, 0, visual.spriteW, visual.spriteH));
    // visual.rect.setOrigin(sf::Vector2(static_cast<float>(visual.spriteW) * 0.5f, static_cast<float>(visual.spriteH)));
    visual = sprite(spriteObject, filepath.c_str(), fx, fy);
    visual.Put(x, y, 0.0f);
    // posX = visual.rect.getPosition().x;
    // posY = visual.rect.getPosition().y;
    // walkToX = posX;
    // walkToY = posY;
    id = _id;
    hp = maxhp;
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
    // collider.moveCenterToPoint(visual.rect.getPosition().x, visual.rect.getPosition().y + 4.0f);
    collider.min_x = visual.x - visual.spriteW * 0.5f;
    collider.max_x = visual.x + visual.spriteW * 0.5f;
    collider.min_y = visual.y - visual.spriteH;
    collider.max_y = visual.y;

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
// void game_system::Add(particlesystem *p)
// {
//     particles[particlesystemcount] = p;
//     ++particlesystemcount;
// }

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
    game_sound_result = ma_sound_init_from_file(engine, path, 0, NULL, NULL, &game_sounds[id]);
    if (game_sound_result != MA_SUCCESS)
    {
        std::cout << game_sound_result << " sound error\n";
    }
}
void game_system::playSound(unsigned int id, float volume, int start_time)
{
    ma_sound_set_volume(&game_sounds[id], volume);
    ma_sound_start(&game_sounds[id]);
}

void game_system::update(dungeon &floor, float delta_time)
{
    if (paused)
    {
        for (int i = 0; i < characterCount; ++i)
        {
            characters[i]->visual.Put(characters[i]->visual.x, characters[i]->visual.y, 0.0f);
        }
        return;
    }

    quicksortSprites(sortedSprites, 0, characterCount - 1);
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
            characters[i]->velocityY += 900.0f * delta_time;
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
                                                                           characters[i]->velocityY * delta_time, floor.collision_boxes[j], xNormal, yNormal);

            switch (floor.collision_boxes[j].collisionID)
            {
            case 1:
                if (firstCollisionHitTest < 1.0f)
                    characters[i]->hp = 0;
                break;
            case 2:
                if (firstCollisionHitTest < 1.0f && characters[i]->isAPlayer)
                    levelincreasing = true;
                break;
            default:
                if (firstCollisionHitTest < 1.0f && xNormal < 0.0f && characters[i]->velocityX > 0.0f)
                {
                    characters[i]->velocityX *= firstCollisionHitTest;
                }
                if (firstCollisionHitTest < 1.0f && xNormal > 0.0f && characters[i]->velocityX < 0.0f)
                {
                    characters[i]->velocityX *= firstCollisionHitTest;
                }
                if (firstCollisionHitTest < 1.0f && yNormal < 0.0f && characters[i]->velocityY > 0.0f)
                {
                    characters[i]->onGround = true;
                    characters[i]->velocityY *= firstCollisionHitTest;
                }
                if (firstCollisionHitTest < 1.0f && yNormal > 0.0f && characters[i]->velocityY < 0.0f)
                {
                    characters[i]->velocityY *= firstCollisionHitTest;
                }
                break;
            }
        }
        characters[i]->updatePosition(delta_time);
    }

    // for (int i = 0; i < particlesystemcount; ++i)
    // {
    //     particles[i]->spawn(delta_time);
    //     particles[i]->update(delta_time);
    // }
}

// void game_system::killParticles()
// {
//     for (int i = 0; i < particlesystemcount; ++i)
//     {
//         particles[i]->kill();
//     }
// }