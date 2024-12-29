// animation commands should be inside sprite class and have the following public functions:
//  sprite.runFrames(x, y, spd)
//  sprite.saveFrames(id, x, y, spd)
//  sprite.runFrames(id)

#include "../headers/system.h"
#include "../headers/gamestate.h"

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
    // if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    // {
    //     // p.MoveTo(p.visual.rect.getPosition().x - 4.0f, p.visual.rect.getPosition().y, floor);
    //     p.velocityX = -p.runSpeed;
    // }
    if (glfwGetKey(window, GLFW_KEY_A) || glfwGetKey(window, GLFW_KEY_LEFT))
    {
        p.velocityX = -p.runSpeed;
    }
    // if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    // {
    //     // p.MoveTo(p.visual.rect.getPosition().x + 4.0f, p.visual.rect.getPosition().y, floor);
    //     p.velocityX = p.runSpeed;
    //     // p.visual.Move(50.0f * delta_time, 0.0f);
    // }
    if (glfwGetKey(window, GLFW_KEY_D) || glfwGetKey(window, GLFW_KEY_RIGHT))
    {
        p.velocityX = p.runSpeed;
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
        p.velocityY -= 3.2f * p.runSpeed;
        p.jumped = true;
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

void dungeonInit(game_system &game, dungeon &dg, std::string tilePath, std::string levelPath, object *dungeon_object, shader *dungeon_shader, unsigned int fx, unsigned int fy);

int prevState = -1;
int entityHP[entity_limit];
int entityHP_UI_Index = 0;
// edit all guis here
void menuData(game_system &mainG, character &mainP, dungeon &floor, object *gui_object, shader *gui_shader)
{
    if (state == prevState)
        return;

    gui_data.elements.clear();

    std::string temp_path;
    // mainG.killParticles();
    switch (state)
    {
    case START_SCREEN:
        // if (!mainG.game_music.openFromFile("../snd/mus/M-01.mp3"))
        // {
        //     std::cout << "failed to load ../snd/mus/M-01.mp3\n";
        // }
        // gui_data.background = sprite("../img/pond.png", 0.0f, 0.0f, 2, 5);
        gui_data.background = sprite(gui_object, gui_shader);
        gui_data.background.setTexture("./test.png", 1, 1);
        // gui_data.bgAnim = animation(&gui_data.background, 0, 9, 50.0f);
        // gui_data.background.setColor(sf::Color(255, 255, 255, 255));
        // gui_data.elements.push_back(ui_element(UI_CLICKABLE, gui_object, gui_shader, "./test-2.png", 10.0f, 50.0f, 4, 1, startGame, nullptr, nullptr, nullptr, CHARACTER_CREATION_SCREEN));
        // gui_data.elements[gui_data.elements.size() - 1].anim = animation(&gui_data.elements[gui_data.elements.size() - 1].visual, 0, 3, 180.0f);
        // gui_data.elements[gui_data.elements.size() - 1].anim.run(delta_time, true);
        break;
    case MENU_SCREEN:
        // floor.screenPositionX = 0.0f;
        // floor.screenPositionY = 0.0f;
        // gui_data.background = sprite("../img/particles.png", 0.0f, 0.0f, 3, 1);
        // gui_data.background.rect.setColor(sf::Color(255, 255, 255, 255));
        gui_data.elements.push_back(ui_element(UI_CLICKABLE, gui_object, gui_shader, "./test-2.png", 40.0f, 40.0f, 1, 1,
                                               startGame, nullptr, nullptr, nullptr, prevState));
        gui_data.elements[gui_data.elements.size() - 1].anim = animation(&gui_data.elements[gui_data.elements.size() - 1].visual, 0, 3, 180.0f);
        break;
    case CHARACTER_CREATION_SCREEN:
        // gui_data.background = sprite("../img/c_s.png", 0.0f, 0.0f, 1, 1);
        // gui_data.background.rect.setColor(sf::Color(255, 255, 255, 255));
        gui_data.elements.push_back(ui_element(UI_CLICKABLE, gui_object, gui_shader, "./test.png", 40.0f, 40.0f, 1, 1, startGame, nullptr, nullptr, nullptr, DUNGEON_SCREEN));
        break;
    case DUNGEON_SCREEN:
        if (mainG.levelincreasing)
        {
            mainG.levelincreasing = false;
            mainG.level++;
        }
        switch (mainG.level)
        {
        case 0:
            dungeonInit(mainG, floor, "../img/01-tiles.png", "../dungeons/L01.txt", gui_object, gui_shader, 1, 1);
            break;
        case 1:
            dungeonInit(mainG, floor, "../img/01-tiles.png", "../dungeons/L02.txt", gui_object, gui_shader, 1, 1);
            break;
        case 2:
            dungeonInit(mainG, floor, "../img/01-tiles.png", "../dungeons/L03.txt", gui_object, gui_shader, 1, 1);
            break;
        case 3:
            dungeonInit(mainG, floor, "../img/01-tiles.png", "../dungeons/W.txt", gui_object, gui_shader, 1, 1);
            break;
        case 4:
            state = MENU_SCREEN;
            return;
        default:
            std::cout << ":megamind: no level?\n";
            dungeonInit(mainG, floor, "../img/01-tiles.png", "../dungeons/L01.txt", gui_object, gui_shader, 1, 1);
            break;
        }

        // if (!mainG.game_music.openFromFile("../snd/mus/L-04.mp3"))
        // {
        //     std::cout << "failed to load ../snd/mus/L-04.mp3\n";
        // }
        // gui_data.background = sprite("../img/01.png", 0.0f, 0.0f, 1, 2);
        // gui_data.background.rect.setColor(sf::Color(255, 255, 255, 255));
        gui_data.bgAnim = animation(&gui_data.background, 0, 1, 50.0f);
        gui_data.bgAnim.setScale(2.0f, 1.0f);

        mainP.visual.Put(floor.spawnLocationX, floor.spawnLocationY, 0.0f);
        // gui_data.elements.push_back(ui_element(UI_CLICKABLE, "../img/s.png", 218.0f, 102.0f, 1, 1, optionsTab, nullptr, &mainG));

        // if (prevState == CHARACTER_CREATION_SCREEN)
        // {
        //     mainP.posX = floor.spawnLocationX;
        //     mainP.posY = floor.spawnLocationY;
        //     mainP.walkToX = floor.spawnLocationX;
        //     mainP.walkToY = floor.spawnLocationY;
        // }

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
void playerInit(character &pl, game_system &game, object *player_object, shader *player_shader)
{
    pl = character(player_object, player_shader, "./img/stick.png", 120.0f, 40.0f, 4, 1, CH_PLAYER);
    pl.isAPlayer = true;

    pl.SetAnimation(ANIM_IDLE, 0, 0, 0.0f);
    pl.SetAnimation(ANIM_WALK, 0, 1, 150.0f);
    pl.SetAnimation(ANIM_ABILITY_0, 2, 2, 0.0f);
    pl.SetAnimation(ANIM_ABILITY_1, 3, 3, 0.0f);

    game.Add(&pl);
}
void dungeonInit(game_system &game, dungeon &dg, std::string tilePath, std::string levelPath, object *dungeon_object, shader *dungeon_shader, unsigned int fx, unsigned int fy)
{
    dg = dungeon(tilePath.c_str(), dungeon_object, dungeon_shader, fx, fy);
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
    sprite bg(&spriteRect, &shaderProgram);
    sprite floor(&spriteRect, &shaderProgram);
    sprite fren(&spriteCube, &shaderProgram);
    bg.setTexture("./test.png", bg.sprite_texture);
    fren.setTexture("./fren.png", fren.sprite_texture);
    floor.setTexture("./img/soot.png", floor.sprite_texture);

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
    dungeon mainDungeon("./img/tiles.png", &spriteRect, &shaderProgram, 4, 2); // lmao
    mainDungeon.readRoomFile("./levels/01.wer");
    mainCam.cameraPosition = glm::vec3(0.0f, 0.0f, 10.0f);
    character mainPlayer;
    playerInit(mainPlayer, game, &spriteRect, &shaderProgram);

    // while (!glfwWindowShouldClose(window))
    // {
    //     float past_time = current_time;
    //     current_time = glfwGetTime();
    //     delta_time = current_time - past_time;

    //     processInput(window);

    //     updateView(shaderProgram);

    //     mainDungeon.draw(window);

    // floor.Put(0.0f, -5.0f, 0.0f);
    // floor.Scale(400.0f, 400.0f, 400.0f);
    // floor.Rotate(90.0f, 0.0f, 0.0f);
    // floor.Draw(shaderProgram, spriteRect.VAO, spriteRect.EBO);

    //     glfwSwapBuffers(window);
    //     glfwPollEvents();
    // }

    sprite testBG(&spriteRect, &shaderProgram);
    testBG.setTexture("./test.png", 1, 1);

    // gui_data.background = sprite(&spriteRect, &shaderProgram);
    // gui_data.background.setTexture("./test.png", 1, 1);

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
        menuData(game, mainPlayer, mainDungeon, &spriteRect, &shaderProgram);

        // game.handleMusic();

        // window.draw(gui_data.background);
        // std::cout << gui_data.background.x << ", " << gui_data.background.texture_path << ", " << gui_data.background.w << "\n";
        // testBG.Draw();
        if (!gui_data.background.empty)
        {
            std::cout << "drawing BG\n";
            gui_data.background.Draw();
        }
        floor.Put(0.0f, -5.0f, 0.0f);
        floor.Scale(400.0f, 400.0f, 400.0f);
        floor.Rotate(90.0f, 0.0f, 0.0f);
        floor.Draw();
        if (state == DUNGEON_SCREEN && mainDungeon.dungeonInitialized)
        {
            // mainDungeon.changeScreenViewPosition(screen, mainPlayer.visual.rect.getPosition().x, mainPlayer.visual.rect.getPosition().y);
            // window.setView(screen);

            // game.update(mainDungeon, delta_time);
            playerControl(game, mainPlayer, window, &mainDungeon);

            // mainDungeon.draw(window);

            for (int i = 0; i < game.characterCount; ++i)
            {
                if (game.characters[i]->visual.empty)
                    continue;

                // window.draw(game.characters[i]->visual.rect);
                // game.characters[i]->visual.Draw();
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
        if (showUI)
        {
            // gui_data.screenDraw(window, mouseX, mouseY, mousePressed, mouseReleased, delta_time);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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

    if (!pauseKeyHeld && glfwGetKey(window, GLFW_KEY_SPACE))
    {
        game.paused = !game.paused;
    }
    pauseKeyHeld = false;
    if (glfwGetKey(window, GLFW_KEY_SPACE))
        pauseKeyHeld = true;
}