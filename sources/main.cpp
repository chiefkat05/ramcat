// animation commands should be inside sprite class and have the following public functions:
//  sprite.runFrames(x, y, spd)
//  sprite.saveFrames(id, x, y, spd)
//  sprite.runFrames(id)

#include "../headers/system.h"
#include "../headers/gamestate.h"
#include "../headers/miniaudio.h"

const unsigned int window_width = 1280;
const unsigned int window_height = 720;

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

extern gui gui_data;
extern game_state state;

camera mainCam(CAMERA_STATIONARY);
// planning time

void playerControl(game_system &game, character &p, GLFWwindow *window, dungeon *floor)
{
    p.velocityX = 0.0f;
    bool walkingkeypressed = false;
    static bool stepsoundplayed = false;
    if (glfwGetKey(window, GLFW_KEY_A) || glfwGetKey(window, GLFW_KEY_LEFT))
    {
        p.velocityX = -p.runSpeed;
        p.PlayAnimation(ANIM_WALK, delta_time, true);
        p.visual.Rotate(0.0f, 180.0f, 0.0f);
        walkingkeypressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_D) || glfwGetKey(window, GLFW_KEY_RIGHT))
    {
        p.velocityX = p.runSpeed;
        p.visual.Rotate(0.0f, 0.0f, 0.0f);
        p.PlayAnimation(ANIM_WALK, delta_time, true);
        walkingkeypressed = true;
    }
    if (p.playingAnim != ANIM_WALK || p.animations[ANIM_WALK].frame != 1)
    {
        stepsoundplayed = false;
    }
    if (p.playingAnim == ANIM_WALK && p.animations[ANIM_WALK].frame == 1 && !stepsoundplayed)
    {
        game.playSound(2, 0.3f, 0);
        stepsoundplayed = true;
    }
    if (!walkingkeypressed)
    {
        p.StopAnimation(ANIM_WALK);
    }
    if (!p.onGround)
    {
        p.jumped = false;
    }
    // if (p.onGround && !p.jumped && (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)))
    // {
    //     // p.MoveTo(p.visual.rect.getPosition().x, p.visual.rect.getPosition().y - 4.0f, floor);
    //     p.velocityY -= 3.2f * p.runSpeed;
    //     p.jumped = true;
    // }
    if (p.onGround && !p.jumped && (glfwGetKey(window, GLFW_KEY_W) || glfwGetKey(window, GLFW_KEY_UP)))
    {
        // p.MoveTo(p.visual.rect.getPosition().x, p.visual.rect.getPosition().y - 4.0f, floor);
        p.velocityY = 30.2f * p.runSpeed;
        p.jumped = true;
        p.PlayAnimation(ANIM_WALK, delta_time, true);
    }
    // if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    // {
    //     // p.allies[i].MoveTo(p.allies[i].visual.rect.getPosition().x, p.allies[i].visual.rect.getPosition().y + 4.0f, floor);
    // }
    if (glfwGetKey(window, GLFW_KEY_S) || glfwGetKey(window, GLFW_KEY_DOWN))
    {
        // p.allies[i].MoveTo(p.allies[i].visual.rect.getPosition().x, p.allies[i].visual.rect.getPosition().y + 4.0f, floor);
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

int prevState = -1;
int entityHP[entity_limit];
int entityHP_UI_Index = 0;
// edit all guis here
void menuData(game_system &mainG, character &mainP, dungeon &floor, object &gui_object, ma_engine &s_engine)
{
    if (state == prevState)
        return;

    gui_data.elements.clear();
    // gui_data.background = sprite(&gui_object, "./test.png", 1, 1);
    gui_data.background = sprite(&gui_object, "./null.png", 1, 1);

    std::string temp_path;
    // mainG.killParticles();
    switch (state)
    {
    case START_SCREEN:
        mainG.initSound("./snd/mus/fellowtheme.mp3", 0, &s_engine);
        mainG.playSound(0, 1, 0);
        gui_data.background = sprite(&gui_object, "./img/menu.png", 3, 1);
        gui_data.bgAnim = animation(&gui_data.background, 0, 1, 30.0f);
        gui_data.background.Scale(3.5556f, 2.0f, 1.0f);
        gui_data.elements.push_back(ui_element(UI_CLICKABLE, &gui_object, "./img/play.png", -0.5f, -0.5f, 64.0f, 128.0f, 1, 1, startGame, nullptr, nullptr, nullptr, CHARACTER_CREATION_SCREEN));
        break;
    case MENU_SCREEN:
        gui_data.background = sprite(&gui_object, "./test.png", 1, 1);
        gui_data.background.Scale(3.5556f, 2.0f, 1.0f);
        gui_data.elements.push_back(ui_element(UI_CLICKABLE, &gui_object, "./img/play.png", 40.0f, 40.0f, 1.0f, 1.0f, 1, 1,
                                               startGame, nullptr, nullptr, nullptr, prevState));
        gui_data.elements[gui_data.elements.size() - 1].anim = animation(&gui_data.elements[gui_data.elements.size() - 1].visual, 0, 3, 180.0f);
        break;
    case CHARACTER_CREATION_SCREEN:
        gui_data.background = sprite(&gui_object, "./test-2.png", 1, 1);
        gui_data.background.Scale(3.5556f, 2.0f, 1.0f);
        gui_data.elements.push_back(ui_element(UI_CLICKABLE, &gui_object, "./img/play.png", -0.5f, -0.5f, 32.0f, 32.0f, 1, 1, startGame, nullptr, nullptr, nullptr, DUNGEON_SCREEN));
        break;
    case DUNGEON_SCREEN:
        mainG.initSound("./snd/fx/kstep.wav", 2, &s_engine);
        mainG.initSound("./snd/mus/castle-1.mp3", 1, &s_engine);
        mainG.playSound(1, 1, 0);
        gui_data.background = sprite(&gui_object, "./test.png", 1, 1);
        gui_data.background.Scale(3.5556f, 2.0f, 1.0f);

        if (mainG.levelincreasing)
        {
            mainG.levelincreasing = false;
            mainG.level++;
        }
        switch (mainG.level)
        {
        case 0:
            dungeonInit(mainG, floor, "./img/tiles.png", "./levels/01.wer", &gui_object, 4, 2);
            break;
        case 1:
            dungeonInit(mainG, floor, "./img/tiles.png", "./levels/02.wer", &gui_object, 4, 2);
            break;
        case 2:
            dungeonInit(mainG, floor, "./img/tiles.png", "./levels/03.wer", &gui_object, 4, 2);
            break;
        case 3:
            dungeonInit(mainG, floor, "./img/tiles.png", "./levels/04.wer", &gui_object, 4, 2);
            break;
        case 4:
            dungeonInit(mainG, floor, "./img/tiles.png", "./levels/B.wer", &gui_object, 4, 2);
            break;
        case 5:
            dungeonInit(mainG, floor, "./img/tiles.png", "./levels/W.wer", &gui_object, 4, 2);
            break;
        case 6:
            state = MENU_SCREEN;
            return;
        default:
            std::cout << ":megamind: no level?\n";
            dungeonInit(mainG, floor, "./img/tiles.png", "./levels/01.wer", &gui_object, 4, 2);
            break;
        }

        // gui_data.bgAnim = animation(&gui_data.background, 0, 1, 50.0f);
        // gui_data.bgAnim.setScale(2.0f, 1.0f);

        if (prevState == CHARACTER_CREATION_SCREEN)
            mainP.visual.Put(floor.spawnLocationX, floor.spawnLocationY, 0.0f);

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

void loadtest(object &gui_object)
{
    gui_data.background = sprite(&gui_object, "./test.png", 1, 1);
}

bool pauseKeyHeld = false, uiKeyHeld = false, showUI = true;
void playerInit(character &pl, game_system &game, object *player_object)
{
    pl = character(player_object, "./img/char/knight.png", -120.0f, -40.0f, 4, 3, CH_PLAYER);
    pl.visual.Scale(0.32f, 0.32f, 1.0f);
    // pl.visual.Scale(pl.visual.spriteW, pl.visual.spriteH);
    // pl.visual.Scale(0.1f, 0.1f, 1.0f);
    // std::cout << pl.visual.textureWidth << " hmm\n";
    pl.isAPlayer = true;

    pl.SetAnimation(ANIM_IDLE, 0, 0, 0.0f);
    pl.SetAnimation(ANIM_WALK, 0, 1, 100.0f);
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
    mainCam.update();

    _program.use();
    view = glm::lookAt(mainCam.cameraPosition, mainCam.cameraPosition + mainCam.cameraFront, mainCam.cameraUp);
    proj = glm::perspective(glm::radians(mainCam.current_fov), static_cast<float>(window_width) / static_cast<float>(window_height), 0.01f, 200.0f);
    _program.setUniformMat4("projection", proj);
    _program.setUniformMat4("view", view);
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
    sprite bg(&spriteRect, "./test.png");
    sprite floor(&spriteRect, "./test.png");
    sprite fren(&spriteCube, "./fren.png");

    // declare 'sprites' or game objects here
    // sprite yoursprite(&[spriteRect | spriteCube]);
    // yoursprite.setTexture([path], yoursprite.sprite_texture);
    // yoursprite.Put(10.0f, 10.0f, 0.0f);
    // yoursprite.Scale(2.0f, 2.0f, 2.0f);
    // yoursprite.Rotate(0.0f, 0.0f, 45.0f);
    // yoursprite.Draw(shader, [spriteRect | spriteCube].VAO, [spriteRect | spriteCube].EBO);

    float current_time = 0;

    glEnable(GL_DEPTH_TEST);

    // mainCam.lockTo();
    // mainCam.setBoundary(-50, -50, -50, 50, 50, 50);
    dungeon mainDungeon("./img/tiles.png", &spriteRect, 4, 2); // lmao
    mainDungeon.readRoomFile("./levels/01.wer");
    mainCam.cameraPosition = glm::vec3(0.0f, 0.0f, 1.0f);
    character mainPlayer;
    playerInit(mainPlayer, game, &spriteRect);
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
        menuData(game, mainPlayer, mainDungeon, spriteRect, soundEngine);

        if (state == DUNGEON_SCREEN && mainDungeon.dungeonInitialized)
        {
            // mainDungeon.changeScreenViewPosition(screen, mainPlayer.visual.rect.getPosition().x, mainPlayer.visual.rect.getPosition().y);
            // window.setView(screen);

            game.update(mainDungeon, delta_time);
            playerControl(game, mainPlayer, window, &mainDungeon);

            mainDungeon.draw(window, shaderProgram);

            for (int i = 0; i < game.characterCount; ++i)
            {
                if (game.characters[i]->visual.empty)
                    continue;

                // window.draw(game.characters[i]->visual.rect);
                game.characters[i]->visual.Draw(shaderProgram);
            }

            if (mainPlayer.hp <= 0)
            {
                mainPlayer.visual.Put(mainDungeon.spawnLocationX, mainDungeon.spawnLocationY, 0.0f);
                mainPlayer.hp = mainPlayer.maxhp;
            }

            if (game.levelincreasing)
            {
                state = WON_LEVEL_STATE;
            }
        }
        // for (int i = 0; i < game.particlesystemcount; ++i)
        // {
        //     game.particles[i]->draw(window, delta_time);
        // }
        if (!uiKeyHeld && glfwGetKey(window, GLFW_KEY_TAB))
        {
            showUI = !showUI;
        }
        uiKeyHeld = false;
        if (glfwGetKey(window, GLFW_KEY_TAB))
        {
            uiKeyHeld = true;
        }
        if (state != DUNGEON_SCREEN)
        {
            gui_data.screenDraw(window, shaderProgram, mouseX, mouseY, mousePressed, mouseReleased, delta_time);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    ma_engine_uninit(&soundEngine);

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
        std::cout << " wow close window please\n";
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