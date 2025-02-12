// animation commands should be inside sprite class and have the following public functions:
//  sprite.runFrames(x, y, spd)
//  sprite.saveFrames(id, x, y, spd)
//  sprite.runFrames(id)

#include "../headers/system.h"
#include "../headers/gamestate.h"
#include "../headers/miniaudio.h"

// #define COLLISION_DEBUG

float texCoords[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f};

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void processInput(GLFWwindow *window);

float delta_time = 0.0f;
bool mouseClicked = false, mousePressed = false, mouseReleased = false;
float mouseX = 0.0f, mouseY = 0.0f;
bool buttonHovered = false;
int parryCooloff = 15, parryTimer = 0;
int playerGamepadCount = -1, playerCount = 1;
float playerSpawnDist = 0.0f;
float lowestCamYLevel = 0.0f;
bool playerSpawned = false;
bool playerFacingRight = true;

extern gui gui_data;
extern game_state state;

camera mainCam(CAMERA_STATIONARY);
// planning time

void playerControl(game_system &game, character &p, GLFWwindow *window, dungeon *floor)
{
    if (p.onGround)
    {
        p.velocityX = 0.0f;
    }
    else if (!p.onGround && p.velocityX > 0.0f)
    {
        p.velocityX -= 15.0f * delta_time;
    }
    else if (!p.onGround && p.velocityX < 0.0f)
    {
        p.velocityX += 15.0f * delta_time;
    }
    bool walkingkeypressed = false;
    static bool stepsoundplayed = false;
    if (p.plControl->getInput(window, CONTROL_LEFT))
    {
        p.velocityX = -p.runSpeed;
        p.visual.Rotate(0.0f, 180.0f, 0.0f);
        if (p.onGround && p.animations[ANIM_ABILITY_0].finished)
            p.PlayAnimation(ANIM_WALK, delta_time, true);
        walkingkeypressed = true;
        playerFacingRight = false;
    }
    if (p.plControl->getInput(window, CONTROL_RIGHT))
    {
        p.velocityX = p.runSpeed;
        p.visual.Rotate(0.0f, 0.0f, 0.0f);
        if (p.onGround && p.animations[ANIM_ABILITY_0].finished)
            p.PlayAnimation(ANIM_WALK, delta_time, true);

        walkingkeypressed = true;
        playerFacingRight = true;
    }
    if (p.plControl->getInput(window, CONTROL_DOWN) && !p.onGround)
    {
        p.velocityY = 3.0f * -p.runSpeed;
    }
    if (p.playingAnim != ANIM_WALK || p.animations[ANIM_WALK].frame != 1)
    {
        stepsoundplayed = false;
    }
    if (p.playingAnim == ANIM_WALK && p.animations[ANIM_WALK].frame == 1 && !stepsoundplayed)
    {
        game.playSound(2, 0.15f, 0);
        stepsoundplayed = true;
    }
    if (!walkingkeypressed)
    {
        p.StopAnimation(ANIM_WALK);
    }
    if (p.onGround && !p.plControl->getInput(window, CONTROL_UP))
    {
        p.jumped = false;
    }
    if ((p.onGround || p.parrySuccess) && !p.jumped && p.plControl->getInput(window, CONTROL_UP))
    {
        p.velocityY = 1.8f * p.runSpeed;
        p.jumped = true;
        p.onGround = false;
        p.parrySuccess = false;
    }

    static bool parryButtonPressed = false;
    if (p.parryTimer > 0)
        --p.parryTimer;
    if (p.parryTimer <= 0)
    {
        p.parrySuccess = false;
    }
    if (!p.plControl->getInput(window, CONTROL_SHIELD))
    {
        parryButtonPressed = false;
    }
    if (p.plControl->getInput(window, CONTROL_SHIELD) && !parryButtonPressed)
    {
        if (parryTimer <= 0)
        {
            p.PlayAnimation(ANIM_ABILITY_0, delta_time, false);
            p.parryTimer = p.parryCooloff;
        }
        parryButtonPressed = true;
    }

    if (p.plControl->getInput(window, CONTROL_SWORD))
    {
        p.PlayAnimation(ANIM_ABILITY_1, delta_time, false);
    }
}

float current_time = 0.0f;
float previous_time = 0.0f;

void mouseUpdate(GLFWwindow *window)
{
    mouseClicked = false;
    mouseReleased = false;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) && !mousePressed)
    {
        mouseClicked = true;
        mousePressed = true;
    }

    if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
    {
        if (mousePressed)
            mouseReleased = true;
        mouseClicked = false;
        mousePressed = false;
    }
}

void dungeonInit(game_system &game, dungeon &dg, std::string tilePath, std::string levelPath, object *dungeon_object, unsigned int fx, unsigned int fy);
void addPlayer(character *ch, game_system *gs, dungeon *dg, int x);

int prevState = -1;
#ifdef COLLISION_DEBUG
sprite delots[20];
sprite dePl;
#endif
// edit all guis here
void menuData(game_system &mainG, character &p1, dungeon &floor, object &gui_object, ma_engine &s_engine)
{
    if (state == prevState)
        return;

    gui_data.elements.clear();
    // gui_data.background = sprite(&gui_object, "./test.png", 1, 1);
    // gui_data.background = sprite(&gui_object, "./null.png", 1, 1);
    // gui_data.elements.push_back(ui_element(UI_IMAGE, &gui_object, "./null.png", 0.0f, 0.0f, 1.0f, 1.0f, 1, 1, nullFunc, true));

    std::string temp_path;
    // mainG.killParticles();
    switch (state)
    {
    case START_SCREEN:
        p1.visual.Put(0.0f, 0.0f, 0.0f);
        mainCam.setBoundary(0.0f, -0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
        mainCam.lockTo(nullptr, nullptr, nullptr);
        mainCam.cameraPosition = glm::vec3(0.0f, 0.0f, 1.0f);
        mainCam.offsetX = 0.0f;
        mainCam.offsetY = 0.0f;
        mainCam.offsetZ = 0.0f;

        mainG.stopSound(1);
        mainG.stopSound(2);
        mainG.initSound("./snd/mus/fellowtheme.mp3", 0, &s_engine);
        mainG.playSound(0, 1, 0);
        gui_data.elements.push_back(ui_element(UI_IMAGE, &gui_object, "./img/menu.png", 0.0f, 0.0f, 128.0f, 64.0f, 3, 1, nullFunc, true));
        gui_data.elements.push_back(ui_element(UI_CLICKABLE, &gui_object, "./img/play.png", -0.5f, -0.5f, 9.0f, 10.0f, 1, 1, startGame, false, nullptr, nullptr, nullptr, CHARACTER_CREATION_SCREEN));
        mainG.level = 0;
        mainG.levelincreasing = false;
        break;
    case CHARACTER_CREATION_SCREEN:
        // gui_data.background = sprite(&gui_object, "./test.png", 1, 1);
        // gui_data.background.Scale(3.5556f, 2.0f, 1.0f);
        gui_data.elements.push_back(ui_element(UI_IMAGE, &gui_object, "./img/menu-char.png", 0.0f, 0.0f, 128.0f, 64.0f, 1, 1, nullFunc, true));
        gui_data.elements.push_back(ui_element(UI_CLICKABLE, &gui_object, "./img/add_player.png", 0.2f, 0.4f, 16.0f, 16.0f, 1, 1, addPlayer, false, &p1, &mainG, &floor));
        gui_data.elements.push_back(ui_element(UI_CLICKABLE, &gui_object, "./img/play.png", -0.5f, -0.5f, 9.0f, 10.0f, 1, 1, startGame, false, nullptr, nullptr, nullptr, DUNGEON_SCREEN));
        break;
    case DUNGEON_SCREEN:
        mainCam.setBoundary(1.9f, -0.0f, -1.0f, 15.0f, 5.0f, 1.0f);
        mainCam.lockTo(&p1.visual.x, &lowestCamYLevel);
        mainG.stopSound(0);
        // sound cuts out on level 2
        mainG.initSound("./snd/fx/kstep.wav", 2, &s_engine);
        mainG.initSound("./snd/mus/castle-1.mp3", 1, &s_engine);
        mainG.playSound(1, 1, 0);
        // gui_data.background = sprite(&gui_object, "./img/bg/01.png", 1, 1);
        // gui_data.background.x = 6.5f;
        // gui_data.background.y = 0.4f;
        // gui_data.background.Scale(17.778f, 2.0f, 1.0f);
        gui_data.elements.push_back(ui_element(UI_IMAGE, &gui_object, "./img/bg/01.png", 0.0f, 0.0f, 1280.0f, 520.0f, 3, 1, nullFunc, true));
        if (mainG.levelincreasing)
        {
            mainG.levelincreasing = false;
            mainG.level++;
        }
        switch (mainG.level)
        {
        case 0:
            dungeonInit(mainG, floor, "./img/tiles.png", "./levels/01.lvl", &gui_object, 4, 4);
            break;
        case 1:
            dungeonInit(mainG, floor, "./img/tiles.png", "./levels/02.lvl", &gui_object, 4, 4);
            break;
        case 2:
            dungeonInit(mainG, floor, "./img/tiles.png", "./levels/03.lvl", &gui_object, 4, 4);
            break;
            // break;
        case 3:
            dungeonInit(mainG, floor, "./img/tiles.png", "./levels/04.lvl", &gui_object, 4, 4);
            break;
        case 4:
            state = START_SCREEN;
            // dungeonInit(mainG, floor, "./img/tiles.png", "./levels/B.lvl", &gui_object, 4, 4);
            return;
        case 5:
            dungeonInit(mainG, floor, "./img/tiles.png", "./levels/W.lvl", &gui_object, 4, 4);
            break;
        case 6:
            // state = MENU_SCREEN;
            state = START_SCREEN;
            return;
        default:
            std::cout << ":megamind: no level?\n";
            dungeonInit(mainG, floor, "./img/tiles.png", "./levels/01.lvl", &gui_object, 4, 2);
            break;
        }

#ifdef COLLISION_DEBUG
        for (int i = 0; i < 20; ++i)
        {
            if (floor.collision_boxes[i].collisionID < 0)
                continue;
            delots[i] = sprite(&gui_object, "./img/debug.png", 1, 1);
        }
        dePl = sprite(&gui_object, "./img/debug.png", 1, 1);
#endif

        playerSpawnDist = 0;
        for (int i = 0; i < mainG.characterCount; ++i)
        {
            if (mainG.characters[i]->plControl == nullptr)
                continue;

            mainG.characters[i]->visual.Put(floor.spawnLocationX, -floor.spawnLocationY + playerSpawnDist, 0.0f);
            playerSpawnDist += mainG.characters[i]->visual.h + 0.02f;
        }
        lowestCamYLevel = p1.visual.y;

        break;
    case WON_LEVEL_STATE:
        std::cout << "wow\n";
        if (!mainG.levelincreasing)
        {
            std::cout << "how did you do this\n";
        }
        break;
    default:
        break;
    }

    prevState = state;
}

bool pauseKeyHeld = false, uiKeyHeld = false, showUI = true;
player playerControllers[player_limit];
character players[player_limit];
void playerInit(character &pl, game_system &game, object *player_object, player &controller)
{
    pl = character(player_object, "./img/char/knight.png", -120.0f, -40.0f, 4, 3, CH_PLAYER);
    pl.visual.Scale(0.32f, 0.32f, 1.0f);
    if (glfwJoystickIsGamepad(playerGamepadCount + 1))
    {
        ++playerGamepadCount;
        controller.gamepad_id = playerGamepadCount;
    }
    pl.plControl = &controller;

    pl.SetAnimation(ANIM_IDLE, 0, 0, 0.0f);
    pl.SetAnimation(ANIM_WALK, 0, 1, 100.0f);
    pl.SetAnimation(ANIM_HURT, 6, 10, 250.0f);
    pl.SetAnimation(ANIM_DEAD, 11, 11, 100.0f);
    pl.SetAnimation(ANIM_ABILITY_2, 1, 1, 100.0f);
    pl.SetAnimation(ANIM_ABILITY_0, 2, 2, 100.0f);
    pl.SetAnimation(ANIM_ABILITY_1, 3, 3, 100.0f);
    // pl.SetAnimation(ANIM_ABILITY_0, 2, 2, 0.0f);
    // pl.SetAnimation(ANIM_ABILITY_1, 3, 3, 0.0f);

    game.Add(&pl);
}
void dungeonInit(game_system &game, dungeon &dg, std::string tilePath, std::string levelPath, object *dungeon_object, unsigned int fx, unsigned int fy)
{
    dg = dungeon(tilePath.c_str(), dungeon_object, fx, fy);
    dg.readRoomFile(levelPath.c_str());
}

// feh
void updateView(shader &_program)
{
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 proj;

    mainCam.cameraPosition += mainCam.cameraVelocity;
    mainCam.update(0.2f);

    _program.use();
    view = glm::lookAt(mainCam.cameraPosition, mainCam.cameraPosition + mainCam.cameraFront, mainCam.cameraUp);
    proj = glm::perspective(glm::radians(mainCam.current_fov), static_cast<float>(window_width) / static_cast<float>(window_height), 0.01f, 200.0f);
    _program.setUniformMat4("projection", proj);
    _program.setUniformMat4("view", view);
}
void addPlayer(character *ch, game_system *gs, dungeon *dg, int x)
{
    if (playerCount >= player_limit)
        return;

    playerInit(players[playerCount], *gs, ch->visual.sprite_object, playerControllers[playerCount]);
    playerCount++;
}

game_system game;
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(window_width, window_height, "Rambunctious- \"Cat edition\"", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "no window :(\n";
        glfwTerminate();
        return 0;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, mouse_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "GLAD FAILED TO LOAD EVERYONE PANIC\n";
        glfwTerminate();
        return 0;
    }

    shader shaderProgram("./shaders/default.vertex", "./shaders/default.fragment");
    shader testProgram("./shaders/default.vertex", "./shaders/white.fragment");

    object spriteRect(OBJ_QUAD);
    object spriteCube(OBJ_CUBE);

    // declare 'sprites' or game objects here
    // sprite yoursprite(&[spriteRect | spriteCube]);
    // yoursprite.setTexture([path], yoursprite.sprite_texture);
    // yoursprite.Put(10.0f, 10.0f, 0.0f);
    // yoursprite.Scale(2.0f, 2.0f, 2.0f);
    // yoursprite.Rotate(0.0f, 0.0f, 45.0f);
    // yoursprite.Draw(shader, [spriteRect | spriteCube].VAO, [spriteRect | spriteCube].EBO);

    float current_time = 0;

    glEnable(GL_DEPTH_TEST);

    dungeon mainDungeon("./img/tiles.png", &spriteRect, 4, 2); // lmao
    // mainDungeon.readRoomFile("./levels/01.lvl");
    mainCam.cameraPosition = glm::vec3(0.0f, 0.0f, 1.0f);
    playerInit(players[0], game, &spriteRect, playerControllers[0]);
    prevState = WIN_SCREEN;

    ma_engine soundEngine;

    ma_result game_sound_result = ma_engine_init(nullptr, &soundEngine);
    if (game_sound_result != MA_SUCCESS)
    {
        std::cout << game_sound_result << " sound error\n";
    }

    while (!glfwWindowShouldClose(window))
    {
        float past_time = current_time;
        current_time = glfwGetTime();
        delta_time = current_time - past_time;

        mouseUpdate(window);
        processInput(window);

        updateView(shaderProgram);

        // window.clear();
        if (state == WON_LEVEL_STATE && game.levelincreasing)
        {
            prevState = WON_LEVEL_STATE;
            state = DUNGEON_SCREEN;
        }
        // menuData(game, mainPlayer, mainDungeon, spriteRect);
        menuData(game, players[0], mainDungeon, spriteRect, soundEngine);
        gui_data.screenDraw(window, shaderProgram, mouseX, mouseY, mousePressed, mouseReleased, delta_time, true);
        for (int i = 0; i < game.particlesystemcount; ++i)
        {
            game.particles[i]->draw(window, shaderProgram, delta_time);
        }

        if (state == CHARACTER_CREATION_SCREEN)
        {
            for (int i = 0; i < playerCount; ++i)
            {
                players[i].visual.Put(-1.4f + i * 0.22f, 0.0f, 0.0f);
                players[i].visual.Draw(shaderProgram);
            }
        }
        if (state == DUNGEON_SCREEN && mainDungeon.dungeonInitialized)
        {
            if (playerFacingRight && mainCam.offsetX < 0.2f)
            {
                mainCam.offsetX += 2.0f * delta_time;
            }
            if (!playerFacingRight && mainCam.offsetX > -0.2f)
            {
                mainCam.offsetX -= 2.0f * delta_time;
            }
            if (players[0].onGround && players[0].visual.y > -0.5f + lowestCamYLevel)
            {
                lowestCamYLevel += 2.0f * delta_time;
            }
            if (players[0].onGround && players[0].visual.y < lowestCamYLevel)
            {
                lowestCamYLevel -= 2.0f * delta_time;
            }
// mainDungeon.changeScreenViewPosition(screen, mainPlayer.visual.rect.getPosition().x, mainPlayer.visual.rect.getPosition().y);
// window.setView(screen);
#ifdef COLLISION_DEBUG
            for (int i = 0; i < 20; ++i)
            {
                if (delots[i].empty)
                    continue;
                delots[i].Draw(shaderProgram);

                if (mainDungeon.collision_boxes[i].collisionID < 0)
                    continue;

                float mdXScale = mainDungeon.collision_boxes[i].max_x - mainDungeon.collision_boxes[i].min_x;
                float mdYScale = mainDungeon.collision_boxes[i].max_y - mainDungeon.collision_boxes[i].min_y;
                delots[i].Scale(mdXScale,
                                mdYScale, 1.0f);
                delots[i].Put(mainDungeon.collision_boxes[i].min_x + 0.5f * mdXScale, mainDungeon.collision_boxes[i].min_y + 0.5f * mdYScale, 0.0f);
                delots[i].SetColor(0.5f, 0.5f, 0.5f, 0.5f);
            }
#endif

            game.update(mainDungeon, delta_time);

            float mpcXScale = players[0].collider.max_x - players[0].collider.min_x;
            float mpcYScale = players[0].collider.max_y - players[0].collider.min_y;
#ifdef COLLISION_DEBUG
            dePl.Scale(mpcXScale, mpcYScale, 1.0f);
            dePl.Put(mainPlayer.collider.min_x + 0.5f * mpcXScale, mainPlayer.collider.min_y + 0.5f * mpcYScale, 0.0f);
            dePl.SetColor(0.5f, 0.5f, 0.5f, 0.5f);
            if (!dePl.empty)
                dePl.Draw(shaderProgram); // debug pls get rid of this later
#endif

            mainDungeon.draw(window, shaderProgram);

            for (int i = 0; i < game.characterCount; ++i)
            {
                if (game.characters[i]->visual.empty)
                    continue;

                // window.draw(game.characters[i]->visual.rect);
                game.characters[i]->visual.Draw(shaderProgram);

                if (game.characters[i]->plControl == nullptr)
                    continue;

                playerControl(game, *game.characters[i], window, &mainDungeon);

                if (game.characters[i]->hp <= 0 || game.characters[i]->visual.y < -20.0f)
                {
                    if (game.characters[i] == &players[0])
                    {
                        game.characters[i]->visual.Put(mainDungeon.spawnLocationX, -mainDungeon.spawnLocationY, 0.0f);
                    }
                    else
                    {
                        game.characters[i]->visual.Put(players[0].visual.x, players[0].visual.y, 0.0f);
                    }
                    // player animation here???
                    game.characters[i]->hp = game.characters[i]->maxhp;
                }
            }

            if (game.levelincreasing)
            {
                state = WON_LEVEL_STATE;
            }
        }
        gui_data.screenDraw(window, shaderProgram, mouseX, mouseY, mousePressed, mouseReleased, delta_time, false);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // ma_engine_uninit(&soundEngine);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    mouseX = xpos;
    mouseY = ypos;
    if (mainCam.firstMouse)
    {
        mainCam.lastMouseX = xpos;
        mainCam.lastMouseY = ypos;
        mainCam.firstMouse = false;
    }

    float offsetX = xpos - mainCam.lastMouseX;
    float offsetY = mainCam.lastMouseY - ypos;

    const float sensitivity = 0.1f;
    glm::vec3 direction = glm::vec3(0.0f);

    switch (mainCam.type)
    {
    case CAMERA_STATIONARY:
        break;
    case CAMERA_2D_FOLLOW:
        if (mainCam.firstMouse)
        {
            mainCam.lastMouseX = xpos;
            mainCam.lastMouseY = ypos;
            mainCam.firstMouse = false;
        }

        mainCam.lastMouseX = xpos;
        mainCam.lastMouseY = ypos;

        offsetX *= sensitivity;
        offsetY *= sensitivity;

        mainCam.cameraPosition.x += offsetX;
        mainCam.cameraPosition.y += offsetY;
        break;
    default:
        mainCam.lastMouseX = xpos;
        mainCam.lastMouseY = ypos;

        offsetX *= sensitivity;
        offsetY *= sensitivity;

        mainCam.yaw += offsetX;
        mainCam.pitch += offsetY;

        if (mainCam.pitch > 89.0f)
            mainCam.pitch = 89.0f;
        if (mainCam.pitch < -89.0f)
            mainCam.pitch = -89.0f;

        direction.x = std::cos(glm::radians(mainCam.yaw)) * std::cos(glm::radians(mainCam.pitch));
        direction.y = std::sin(glm::radians(mainCam.pitch));
        direction.z = std::sin(glm::radians(mainCam.yaw)) * std::cos(glm::radians(mainCam.pitch));
        mainCam.cameraFront = glm::normalize(direction);
        direction = glm::vec3(0.0f);
        direction.x = std::cos(glm::radians(mainCam.yaw));
        direction.z = std::sin(glm::radians(mainCam.yaw));
        mainCam.cameraLockedFront = glm::normalize(direction);

        mainCam.cameraRight = glm::cross(mainCam.cameraUp, mainCam.cameraFront);
        mainCam.cameraLockedRight = glm::cross(mainCam.cameraUp, mainCam.cameraLockedFront);
        break;
    }
}
void processInput(GLFWwindow *window)
{
    if (mainCam.type == CAMERA_3D)
    {
        mainCam.cameraVelocity.x = 0.0f;
        mainCam.cameraVelocity.z = 0.0f;

        if (mainCam.cameraPosition.y > 0.0f)
            mainCam.cameraVelocity.y -= 2.0f * delta_time;
        if (mainCam.cameraPosition.y <= 0.0f)
        {
            mainCam.cameraPosition.y = 0.0f;
            mainCam.cameraVelocity.y = 0.0f;
            mainCam.jumped = false;
        }

        if (glfwGetKey(window, GLFW_KEY_W))
        {
            mainCam.cameraVelocity += mainCam.cameraLockedFront * 20.0f * delta_time;
        }
        if (glfwGetKey(window, GLFW_KEY_S))
        {
            mainCam.cameraVelocity -= mainCam.cameraLockedFront * 20.0f * delta_time;
        }
        if (glfwGetKey(window, GLFW_KEY_A))
        {
            mainCam.cameraVelocity -= glm::normalize(glm::cross(mainCam.cameraFront, mainCam.cameraUp)) * 20.0f * delta_time;
        }
        if (glfwGetKey(window, GLFW_KEY_D))
        {
            mainCam.cameraVelocity += glm::normalize(glm::cross(mainCam.cameraFront, mainCam.cameraUp)) * 20.0f * delta_time;
        }
        if (!mainCam.jumped && glfwGetKey(window, GLFW_KEY_SPACE))
        {
            mainCam.cameraVelocity.y = 2.0f;
            mainCam.jumped = true;
        }
    }
    if (mainCam.type == CAMERA_2D_FOLLOW)
    {
        if (glfwGetKey(window, GLFW_KEY_W))
        {
            mainCam.cameraPosition.y += 10.0f * delta_time;
        }
        if (glfwGetKey(window, GLFW_KEY_S))
        {
            mainCam.cameraPosition.y -= 10.0f * delta_time;
        }
        if (glfwGetKey(window, GLFW_KEY_A))
        {
            mainCam.cameraPosition.x -= 10.0f * delta_time;
        }
        if (glfwGetKey(window, GLFW_KEY_D))
        {
            mainCam.cameraPosition.x += 10.0f * delta_time;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_C))
    {
        mainCam.current_fov = mainCam.zoomed_fov;
    }
    if (!glfwGetKey(window, GLFW_KEY_C))
    {
        mainCam.current_fov = mainCam.fov;
    }

    if (!glfwGetKey(window, GLFW_KEY_F) && mainCam.swappedFullscreen)
        mainCam.swappedFullscreen = false;

    if (glfwGetKey(window, GLFW_KEY_F) && !mainCam.swappedFullscreen)
    {
        mainCam.fullscreen = !mainCam.fullscreen;
        if (mainCam.fullscreen)
            glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0.0f, 0.0f, window_width, window_height, GLFW_DONT_CARE);
        if (!mainCam.fullscreen)
            glfwSetWindowMonitor(window, NULL, 0.0f, 0.0f, window_width, window_height, GLFW_DONT_CARE);

        mainCam.swappedFullscreen = true;
    }

    if (!glfwGetKey(window, GLFW_KEY_TAB) && mainCam.swappedMouseCapture)
        mainCam.swappedMouseCapture = false;

    if (glfwGetKey(window, GLFW_KEY_TAB) && !mainCam.swappedMouseCapture)
    {
        mainCam.mouseCapture = !mainCam.mouseCapture;
        if (mainCam.mouseCapture)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (!mainCam.mouseCapture)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        mainCam.swappedMouseCapture = true;
    }

    // if (!pauseKeyHeld && glfwGetKey(window, GLFW_KEY_SPACE))
    // {
    //     game.paused = !game.paused;
    // }
    // pauseKeyHeld = false;
    // if (glfwGetKey(window, GLFW_KEY_SPACE))
    //     pauseKeyHeld = true;
}