/**
 *  Copyright (c) 2025 James Mathis. Licensed under MIT (see LICENSE.txt for more details)
 *
 *  I apologize in advance for the psuedo-organized, chaotic nature of this code.
 **/

// TO-DO LIST

// doing: Alpha OIT here we go // oh HECK no we're doing ordered transparency and not wasting another second of life on this nonsense

// text rendering

// lighting ( maybe lighting using attributes? )

// alpha

// quadtree

// save function

// Make the actual game

// use linkvalue in the ui_element initialization to take out some of the need for the variable updates happening in if (state == MENU_SCREEN) in main
// ^ isn't that what I made it for anyway??

// Online multiplayer with asio

// add threading where needed

// eventually, better animation structure

// clean this up and remove as much code as possible, especially variables and functions that should be in system.cpp or other files

// Add #define and #ifdef statements to exclude and include different parts of the engine when it gets big enough
// Like not everyone is going to need tilemaps or UI or even collision detection so engine customization will be nice to avoid bloat
// Maybe make one .h file with configs and all the #define statements that you can change to customize the engine

#include "../headers/system.h"
#include "../headers/userinterface.h"
#include "../headers/miniaudio.h"

// #define COLLISION_DEBUG

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

double delta_time = 0.0;
bool mouseClicked = false, mousePressed = false, mouseReleased = false;
double mouseX = 0.0, mouseY = 0.0;
bool buttonHovered = false;
int playerGamepadCount = -1, playerCount = 1;
double playerSpawnDist = 0.0;
double lowestCamYLevel = 0.0;
bool playerFacingRight = true;
double transitionTimer = 1.0;

extern double windowAspectDivision;

extern gui gui_data;

camera mainCam(CAMERA_STATIONARY);

void playerControl(game_system &game, character &p, GLFWwindow *window, world *floor);

double current_time = 0.0;
double previous_time = 0.0;

void mouseUpdate(GLFWwindow *window);

bool pauseKeyHeld = false, uiKeyHeld = false, showUI = true;
player playerControllers[player_limit];

std::string controlsetstrings[] = {
    "Up", "Left", "Right", "Down", "Shield", "Sword", "Bubble", "Spawn"};

// special thanks to https://gist.github.com/0xD34DC0DE/910855d41786b962127ae401da2a3441
// for the string arrays and convertion functions so I don't have to do this myself
constexpr const char *KeyCodeToString(int keycode) noexcept;

const char *gamepadInputStrings[] = {"PAD_BUTTON_A", "PAD_BUTTON_B", "PAD_BUTTON_X", "PAD_BUTTON_Y", "PAD_BUTTON_LBUTTON",
                                     "PAD_BUTTON_RBUTTON", "PAD_BUTTON_BACK", "PAD_BUTTON_START", "PAD_BUTTON_HOME", "PAD_BUTTON_LSTICK",
                                     "PAD_BUTTON_RSTICK", "PAD_DPAD_UP", "PAD_DPAD_RIGHT", "PAD_DPAD_DOWN", "PAD_DPAD_LEFT", "how did you do this",
                                     "PAD_LSTICK_LEFT", "PAD_LSTICK_RIGHT", "PAD_LSTICK_UP", "PAD_LSTICK_DOWN", "PAD_RSTICK_LEFT", "PAD_RSTICK_RIGHT",
                                     "PAD_RSTICK_UP", "PAD_RSTICK_DOWN", "PAD_LTRIGGER", "PAD_RTRIGGER"};

void playerInit(character &pl, game_system &game, player &controller);

void worldInit(game_system &game, world &dg, std::string tilePath, std::string levelPath, unsigned int fx, unsigned int fy);

// feh
void updateView(shader &_program, bool orthographic = false);
void fullscreenChangeFunction(GLFWwindow *window);
void addPlayer(character *ch, game_system *gs, world *wo, int x);
void removePlayer(character *ch, game_system *gs, world *wo, int x);

controlset changingControl = control_limit;
int uiElementForControlChangeIndex = 0;
int playerIDForControl = 0;
int watchGamepadID = -1;
void changeControlFunc(character *ch, game_system *gs, world *wo, int x);
void volumeChangeSoundPlayFunc(character *ch, game_system *gs, world *wo, int x);

extern int gamepad_stick_sensitivity;
int gamepadInputWatch();
void increaseLevel(character *ch, game_system *gs, world *wo, int x);
void incrementPlayerIDForControlFunc(character *ch, game_system *gs, world *wo, int x);
void fullScreenToggleFunc(character *ch, game_system *gs, world *wo, int x);
void particleToggleFunc(character *ch, game_system *gs, world *wo, int x);
void goMenuScreen(character *p, game_system *gs, world *w, int argv);
void leaveMenuScreen(character *p, game_system *gs, world *w, int argv);

int playerIDForControlStrElementIndex = -1;
int prevState = -1;
double camCenterX = 0.0, camCenterY = 0.0;
#ifdef COLLISION_DEBUG
sprite deCollision;
sprite dePl;
#endif

void sceneInit(game_system &mainG, character &p1, world &floor, ma_engine &s_engine, double &transition_timer)
{
    if (mainG.state != mainG.nextState && transition_timer > 0.0)
    {
        transition_timer -= 2.0 * delta_time;
    }
    if (transition_timer <= 0.0)
    {
        mainG.state = mainG.nextState;
    }
    if (mainG.state == mainG.nextState && transition_timer < 1.0)
    {
        transition_timer += 2.0 * delta_time;
    }
    if (transition_timer > 1.0)
        transition_timer = 1.0;

    if (mainG.state == prevState)
        return;

    gui_data.elements.clear();
    mainCam.fov = mainCam.default_fov * 0.01;

    std::string playerIDForControlStr = "Player " + std::to_string(playerIDForControl);

    switch (mainG.state)
    {
    case START_SCREEN:
        p1.visual.Put(0.0, 0.0, 0.0);
        mainCam.setBoundary(0.0, -0.0, -1.0, 0.0, 0.0, 1.0);
        mainCam.lockTo(nullptr, nullptr, nullptr);
        mainCam.cameraPosition = glm::dvec3(0.0, 0.0, 1.0);
        mainCam.offsetX = 0.0;
        mainCam.offsetY = 0.0;
        mainCam.offsetZ = 0.0;

        mainG.initSound("./snd/mus/fellowtheme.mp3", 0, &s_engine);
        mainG.playSound(0, 0.0);
        gui_data.elements.push_back(ui_element(&mainG, UI_IMAGE, "./img/menu.png", -1.0, -1.0, 3, 1, nullFunc, true));
        gui_data.elements.push_back(ui_element(&mainG, UI_CLICKABLE, "./img/quit.png", -0.1f, -0.5f, 1, 1, quitGame));
        gui_data.elements.push_back(ui_element(&mainG, UI_CLICKABLE, "./img/options.png", -0.3f, -0.5f, 1, 1, goMenuScreen));
        gui_data.elements.push_back(ui_element(&mainG, UI_CLICKABLE, "./img/play.png", -0.5f, -0.5f, 1, 1, changeScene, false, nullptr, nullptr, CHARACTER_CREATION_SCREEN));
        mainG.level = 0;
        mainG.levelincreasing = false;
        break;
    case MENU_SCREEN:
        mainCam.setBoundary(0.0, -0.0, -1.0, 0.0, 0.0, 1.0);
        mainCam.lockTo(nullptr, nullptr, nullptr);
        mainCam.cameraPosition = glm::dvec3(0.0, 0.0, 1.0);
        mainCam.offsetX = 0.0;
        mainCam.offsetY = 0.0;
        mainCam.offsetZ = 0.0;
        gui_data.elements.push_back(ui_element(&mainG, UI_IMAGE, "./img/menu.png", -1.0, -1.0, 3, 1, nullFunc, true));
        gui_data.elements.push_back(ui_element(&mainG, UI_CLICKABLE, "./img/back.png", 0.8f, -0.2f, 1, 1, leaveMenuScreen));
        gui_data.elements.push_back(ui_element(&mainG, UI_CLICKABLE, "./img/home.png", 0.8f, -0.5f, 1, 1, changeScene, false, nullptr, nullptr, START_SCREEN));
        gui_data.elements.push_back(ui_element(&mainG, UI_CLICKABLE, "./img/quit.png", 0.8f, -0.8f, 1, 1, quitGame));

        mainG.initSound("./snd/mus/fellowtheme.mp3", 0, &s_engine);
        mainG.playSound(0, 0.0);
        mainG.initSound("./snd/fx/volume.wav", 6, &s_engine);

        gui_data.elements.push_back(ui_element(&mainG, UI_TEXT, "Settings", -0.8f, 0.75f, 1, 1, nullFunc, false));
        gui_data.elements.push_back(ui_element(&mainG, UI_TEXT, "Controls", -0.8f, 0.55f, 1, 1, nullFunc, false));
        gui_data.elements.push_back(ui_element(&mainG, UI_TEXT, playerIDForControlStr.c_str(), -0.65f, 0.45f, 1, 1, nullFunc, false));
        playerIDForControlStrElementIndex = gui_data.elements.size() - 1;
        gui_data.elements.push_back(ui_element(&mainG, UI_CLICKABLE, "./img/arrow_left.png", -0.8f, 0.45f, 1, 1, incrementPlayerIDForControlFunc, false, nullptr, nullptr, -1));
        gui_data.elements.push_back(ui_element(&mainG, UI_CLICKABLE, "./img/arrow_right.png", -0.2f, 0.45f, 1, 1, incrementPlayerIDForControlFunc, false, nullptr, nullptr, 1));

        gui_data.elements.push_back(ui_element(&mainG, UI_CLICKABLE_TEXT, "Next Level", 0.0f, 0.0f, 1, 1, increaseLevel));
        for (int i = 0; i < control_limit; ++i)
        {
            gui_data.elements.push_back(ui_element(&mainG, UI_TEXT, controlsetstrings[i].c_str(), -0.95f, 0.2f - i * 0.15f, 1, 1));
        }
        if (playerIDForControl >= playerCount)
            playerIDForControl = playerCount - 1;

        uiElementForControlChangeIndex = gui_data.elements.size();
        for (int i = 0; i < control_limit; ++i)
        {
            // control buttons here
            if (mainG.characters[playerIDForControl].plControl == nullptr)
            {
                continue;
            }
            if (mainG.characters[playerIDForControl].plControl->gamepad_id <= -1)
            {
                gui_data.elements.push_back(ui_element(&mainG, UI_CLICKABLE_TEXT, KeyCodeToString(mainG.characters[playerIDForControl].plControl->inputs[i]), -0.65f, 0.2f - i * 0.15f,
                                                       1, 1, changeControlFunc, false, &mainG.characters[playerIDForControl], &floor, i));
                gui_data.mostRecentCreatedElement()->visual.SetColor(0.5f, 0.8f, 0.5f, 1.0);
            }
            else
            {
                gui_data.elements.push_back(ui_element(&mainG, UI_TEXT, gamepadInputStrings[mainG.characters[playerIDForControl].plControl->gamepad_inputs[i]], -0.65f, 0.2f - i * 0.15f,
                                                       1, 1, changeControlFunc, false, &mainG.characters[playerIDForControl], &floor, i));
            }
        }

        gui_data.elements.push_back(ui_element(&mainG, UI_TEXT, "Gamepad Stick Sensitivity", 0.2f, 0.8f, 1, 1));
        gui_data.elements.push_back(ui_element(&mainG, UI_SLIDER, "./img/debug.png", 0.5f, 0.75f, 1, 1, nullFunc,
                                               false, nullptr, nullptr, 0, &gamepad_stick_sensitivity));
        gui_data.mostRecentCreatedElement()->scale(40.0, 2.0);
        gui_data.mostRecentCreatedElement()->slider_values(0, 1000);

        gui_data.elements.push_back(ui_element(&mainG, UI_TEXT, "Music Volume", 0.2f, 0.6f, 1, 1));
        gui_data.elements.push_back(ui_element(&mainG, UI_SLIDER, "./img/debug.png", 0.5f, 0.55f, 1, 1, nullFunc,
                                               false, nullptr, nullptr, 0, &mainG.music_volume));
        gui_data.mostRecentCreatedElement()->scale(40.0, 2.0);
        gui_data.mostRecentCreatedElement()->slider_values(0, 125);

        gui_data.elements.push_back(ui_element(&mainG, UI_TEXT, "Sound Volume", 0.2f, 0.45f, 1, 1));
        gui_data.elements.push_back(ui_element(&mainG, UI_SLIDER, "./img/debug.png", 0.5f, 0.4f, 1, 1, volumeChangeSoundPlayFunc,
                                               false, nullptr, nullptr, 0, &mainG.sound_volume));
        gui_data.mostRecentCreatedElement()->scale(40.0, 2.0);
        gui_data.mostRecentCreatedElement()->slider_values(0, 125);

        gui_data.elements.push_back(ui_element(&mainG, UI_TEXT, "Camera FOV", 0.2f, 0.3f, 1, 1));
        gui_data.elements.push_back(ui_element(&mainG, UI_SLIDER, "./img/debug.png", 0.5f, 0.25f, 1, 1, nullFunc,
                                               false, nullptr, nullptr, 0, &mainCam.default_fov));
        gui_data.mostRecentCreatedElement()->scale(40.0, 2.0);
        gui_data.mostRecentCreatedElement()->slider_values(7000, 12000);

        gui_data.elements.push_back(ui_element(&mainG, UI_CLICKABLE_TEXT, "Toggle Fullscreen", 0.15f, -0.7f, 1, 1, fullScreenToggleFunc));
        gui_data.elements.push_back(ui_element(&mainG, UI_CLICKABLE_TEXT, "Toggle Particles", 0.2f, -0.8f, 1, 1, particleToggleFunc));

        break;
    case CHARACTER_CREATION_SCREEN:
        for (int i = 0; i < playerCount; ++i)
        {
            mainG.characters[i].visual.Scale(0.48f, 0.48f, 1.0);
        }
        gui_data.elements.push_back(ui_element(&mainG, UI_IMAGE, "./img/menu-char.png", -1.0, -1.0, 4, 1, nullFunc, true));
        gui_data.elements.push_back(ui_element(&mainG, UI_CLICKABLE, "./img/add_player.png", 0.2f, 0.4f, 1, 1, addPlayer, false, &p1, &floor));
        gui_data.elements.push_back(ui_element(&mainG, UI_CLICKABLE, "./img/del_player.png", 0.45f, 0.4f, 1, 1, removePlayer, false, &p1, &floor));
        gui_data.elements.push_back(ui_element(&mainG, UI_CLICKABLE, "./img/play.png", -0.5f, -0.5f, 1, 1, changeScene, false, nullptr, nullptr, WORLD_SCREEN));
        break;
    case WORLD_SCREEN:
        for (int i = playerCount; i < mainG.characterCount; ++i)
        {
            if (mainG.characters[i].plControl != nullptr)
                continue;

            mainG.Remove(i);
        }

        mainG.setParticles("./img/gfx/spawn.png", 4, 1, 15, 4.0, 4.0, 0.0, 0.0, 0.2, 0.2, 30);

        if (mainG.particleByID(30) != nullptr)
        {
            mainG.particleByID(30)->setVariable(PV_PUSHMIN_Y, -1.0);
            mainG.particleByID(30)->setVariable(PV_PUSHMAX_Y, 1.0);
            mainG.particleByID(30)->setVariable(PV_PUSHMIN_X, -1.0);
            mainG.particleByID(30)->setVariable(PV_PUSHMAX_X, 1.0);
            mainG.particleByID(30)->setVariable(PV_RED, 0.0);
            mainG.particleByID(30)->setVariable(PV_GREEN, 0.0);
            mainG.particleByID(30)->setVariable(PV_BLUE, 0.0);
            mainG.particleByID(30)->setVariable(PV_ALPHA, 1.0);
            mainG.particleByID(30)->setVariable(PV_WIDTH, 0.1);
            mainG.particleByID(30)->setVariable(PV_HEIGHT, 0.1);
            mainG.particleByID(30)->setVariable(PV_WIDTH_LIFE_FALLOFF, 0.2);
            mainG.particleByID(30)->setVariable(PV_HEIGHT_LIFE_FALLOFF, -0.2);
            mainG.particleByID(30)->setVariable(PV_ANIM_START, 0.0);
            mainG.particleByID(30)->setVariable(PV_ANIM_END, 4.0);
            mainG.particleByID(30)->setVariable(PV_ANIM_SPEED, 8.0);
            mainG.particleByID(30)->linkVariable(PV_SPAWN_X, &mainG.characters[0].visual.x);
            mainG.particleByID(30)->linkVariable(PV_SPAWN_W, &mainG.characters[0].visual.x);
            mainG.particleByID(30)->linkVariable(PV_SPAWN_Y, &mainG.characters[0].visual.y);
            mainG.particleByID(30)->linkVariable(PV_SPAWN_H, &mainG.characters[0].visual.y);
        }

        for (int i = 0; i < playerCount; ++i)
        {
            mainG.characters[i].visual.Scale(mainG.characters[i].visual.trueW(), mainG.characters[i].visual.trueH(), 1.0);
        }
        mainCam.lockTo(&camCenterX, &lowestCamYLevel);
        mainG.initSound("./snd/mus/castle-1.mp3", 0, &s_engine);
        mainG.initSound("./snd/fx/kstep.wav", 3, &s_engine);
        mainG.playSound(0, 0.0);
        if (mainG.levelincreasing)
        {
            mainG.levelincreasing = false;
            mainG.level++;
        }
        switch (mainG.level)
        {
        case 0:
            gui_data.elements.push_back(ui_element(&mainG, UI_IMAGE, "./img/bg/01.png", -1.0, -1.0, 1, 1, nullFunc, true));
            gui_data.mostRecentCreatedElement()->scale(128.0, 72.0);
            worldInit(mainG, floor, "./img/tiles.png", "./levels/01.lvl", 6, 6);
            break;
        case 1:
            gui_data.elements.push_back(ui_element(&mainG, UI_IMAGE, "./img/bg/01.png", -1.0, -1.0, 1, 1, nullFunc, true));
            gui_data.mostRecentCreatedElement()->scale(128.0, 72.0);
            worldInit(mainG, floor, "./img/tiles.png", "./levels/02.lvl", 6, 6);
            break;
        case 2:
            gui_data.elements.push_back(ui_element(&mainG, UI_IMAGE, "./img/bg/01.png", -1.0, -1.0, 1, 1, nullFunc, true));
            gui_data.mostRecentCreatedElement()->scale(128.0, 72.0);
            worldInit(mainG, floor, "./img/tiles.png", "./levels/03.lvl", 6, 6);
            break;
        case 3:
            gui_data.elements.push_back(ui_element(&mainG, UI_IMAGE, "./img/bg/01-2.png", -1.0, -1.0, 1, 1, nullFunc, true));
            gui_data.mostRecentCreatedElement()->scale(128.0, 72.0);
            worldInit(mainG, floor, "./img/tiles.png", "./levels/04.lvl", 6, 6);
            break;
        case 4:
            gui_data.elements.push_back(ui_element(&mainG, UI_IMAGE, "./img/bg/01-2.png", -1.0, -1.0, 1, 1, nullFunc, true));
            gui_data.mostRecentCreatedElement()->scale(128.0, 72.0);
            break;
        case 5:
            gui_data.elements.push_back(ui_element(&mainG, UI_IMAGE, "./img/bg/01-2.png", -1.0, -1.0, 1, 1, nullFunc, true));
            gui_data.mostRecentCreatedElement()->scale(128.0, 72.0);
            break;
        case 6:
            gui_data.elements.push_back(ui_element(&mainG, UI_IMAGE, "./img/bg/01-3.png", -1.0, -1.0, 1, 1, nullFunc, true));
            gui_data.mostRecentCreatedElement()->scale(128.0, 72.0);
            break;
        case 7:
            gui_data.elements.push_back(ui_element(&mainG, UI_IMAGE, "./img/bg/01-3.png", -1.0, -1.0, 1, 1, nullFunc, true));
            gui_data.mostRecentCreatedElement()->scale(128.0, 72.0);
            break;
        case 8:
            gui_data.elements.push_back(ui_element(&mainG, UI_IMAGE, "./img/bg/01-3.png", -1.0, -1.0, 1, 1, nullFunc, true));
            gui_data.mostRecentCreatedElement()->scale(128.0, 72.0);
            break;
        case 9:
            gui_data.elements.push_back(ui_element(&mainG, UI_IMAGE, "./img/bg/01-4.png", -1.0, -1.0, 1, 1, nullFunc, true));
            gui_data.mostRecentCreatedElement()->scale(128.0, 72.0);
            break;
        case 10:
            gui_data.elements.push_back(ui_element(&mainG, UI_IMAGE, "./img/bg/01-4.png", -1.0, -1.0, 1, 1, nullFunc, true));
            gui_data.mostRecentCreatedElement()->scale(128.0, 72.0);
            break;
        case 11:
            gui_data.elements.push_back(ui_element(&mainG, UI_IMAGE, "./img/bg/01-4.png", -1.0, -1.0, 1, 1, nullFunc, true));
            gui_data.mostRecentCreatedElement()->scale(128.0, 72.0);
            break;
        case 16:
            gui_data.elements.push_back(ui_element(&mainG, UI_IMAGE, "./img/bg/01-2.png", -1.0, -1.0, 1, 1, nullFunc, true));
            gui_data.mostRecentCreatedElement()->scale(128.0, 72.0);
            worldInit(mainG, floor, "./img/tiles.png", "./levels/04.lvl", 6, 6);
            break;
        case 17:
            mainG.nextState = START_SCREEN;
            return;
        default:
            std::cout << ":megamind: no level?\n";
            worldInit(mainG, floor, "./img/tiles.png", "./levels/01.lvl", 1, 1);
            break;
        }
        mainCam.setBoundary(1.9f, -0.0, -50.0, floor.roomWidth * floor.worldSprite.w - 1.84, 50.0, 1.0);

#ifdef COLLISION_DEBUG
        deCollision = sprite(mainG.shaders[GAME_SHADER_DEFAULT], mainG.objects[GAME_OBJECT_DEFAULT], "./img/debug.png", 1, 1);
        dePl = sprite(mainG.shaders[GAME_SHADER_DEFAULT], mainG.objects[GAME_OBJECT_DEFAULT], "./img/debug.png", 1, 1);
#endif

        playerSpawnDist = 0;
        for (int i = 0; i < playerCount; ++i)
        {
            if (mainG.characters[i].plControl == nullptr)
                continue;

            mainG.characters[i].visual.Put(floor.spawnLocationX, -floor.spawnLocationY + playerSpawnDist, 0.0);
            playerSpawnDist += mainG.characters[i].visual.h + 0.02f;
        }
        lowestCamYLevel = p1.visual.y;

        break;
    case WON_LEVEL_STATE:
        if (!mainG.levelincreasing)
        {
            std::cout << "how did you do this\n";
        }
        break;
    case COFFEE_MUG_DEATH_STATE:
        gui_data.elements.push_back(ui_element(&mainG, UI_TEXT, "HOW COULD YOU :C", -0.2, 0.0, 1, 1));
        gui_data.elements.push_back(ui_element(&mainG, UI_TEXT, "(btw the little square where his hand should be is a coffee mug)", -0.6, -0.7, 1, 1));
        break;
    default:
        break;
    }

    prevState = mainG.state;
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
    glfwSetKeyCallback(window, key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "GLAD FAILED TO LOAD EVERYONE PANIC\n";
        glfwTerminate();
        return 0;
    }

    game.shaders[GAME_SHADER_DEFAULT] = new shader("./shaders/map.vertex", "./shaders/default.fragment");
    game.shaders[GAME_SHADER_TEXT] = new shader("./shaders/text.vertex", "./shaders/text.fragment");

    game.objects[GAME_OBJECT_DEFAULT] = new object(OBJ_QUAD);
    game.objects[GAME_OBJECT_PARTICLE] = new object(OBJ_QUAD);
    game.objects[GAME_OBJECT_TILEMAP] = new object(OBJ_QUAD);
    game.objects[GAME_OBJECT_TEXT] = new object(OBJ_QUAD);

    double current_time = 0;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    world mainWorld; // lmao
    mainCam.cameraPosition = glm::dvec3(0.0, 0.0, 1.0);
    playerInit(game.characters[0], game, playerControllers[0]);
    game.characters[0].visual.SetColor(0.0, 0.0, 0.0, 1.0);
    prevState = WIN_SCREEN;

    ma_engine soundEngine;

    ma_result game_sound_result = ma_engine_init(nullptr, &soundEngine);
    if (game_sound_result != MA_SUCCESS)
    {
        std::cout << game_sound_result << " sound error\n";
    }

    loadFont("./fonts/rainyhearts.ttf");
    glm::mat4 textProjection = glm::ortho(0.0, static_cast<double>(window_width), 0.0, static_cast<double>(window_height));
    game.shaders[GAME_SHADER_TEXT]->use();
    game.shaders[GAME_SHADER_TEXT]->setUniformMat4("projection", textProjection);

    sprite transitionFade(game.shaders[GAME_SHADER_DEFAULT], game.objects[GAME_OBJECT_DEFAULT], "./img/fade.png", 1, 1);
    transitionFade.Scale(8.0, 4.0, 0.0);

    while (!glfwWindowShouldClose(window))
    {
        double past_time = current_time;
        current_time = glfwGetTime();
        delta_time = current_time - past_time;

        mouseUpdate(window);
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.2f, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mainCam.cameraPosition += mainCam.cameraVelocity;
        mainCam.update(0.2f);
        updateView(*game.shaders[GAME_SHADER_DEFAULT]);

        if (game.state == WON_LEVEL_STATE && game.levelincreasing)
        {
            prevState = WON_LEVEL_STATE;
            game.nextState = WORLD_SCREEN;
        }

        sceneInit(game, game.characters[0], mainWorld, soundEngine, transitionTimer);

        transitionFade.Put(mainCam.cameraPosition.x - 3.0, -1.0 + mainCam.cameraPosition.y - transitionTimer * 4.0, 0.0);
        // transitionFade.Put(-2.0, -2.0 - transitionTimer * 4.0, 0.0);
        transitionFade.SetColor(1.0, 1.0, 1.0, 1.0);
        transitionFade.Draw();
        gui_data.screenDraw(game, window, mainCam, mouseX, mouseY, delta_time, true);

        if (game.state == CHARACTER_CREATION_SCREEN && prevState == CHARACTER_CREATION_SCREEN)
        {
            for (int i = 0; i < playerCount; ++i)
            {
                game.characters[i].visual.Put(-1.4f + i * 0.22f, 0.0, 0.0);
                game.characters[i].visual.Draw();
            }
            renderText(*game.objects[GAME_OBJECT_TEXT], *game.shaders[GAME_SHADER_TEXT], "Character Select", 25.0, 625.0, 2.0, glm::vec4(0.0, 0.0, 0.0, 1.0));
        }
        if (game.state == MENU_SCREEN && prevState == MENU_SCREEN)
        {
            game.setParticles("./img/gfx/rain.png", 1, 1, 30, 1.5, 1.5, -1.8, 1.5, 1.8, 1.5, 52);
            if (game.particleByID(52) != nullptr)
            {
                game.particleByID(52)->setVariable(PV_PUSHMIN_Y, -15.0);
                game.particleByID(52)->setVariable(PV_PUSHMAX_Y, -15.0);
                game.particleByID(52)->setVariable(PV_RED, 1.0);
                game.particleByID(52)->setVariable(PV_GREEN, 1.0);
                game.particleByID(52)->setVariable(PV_BLUE, 1.0);
                game.particleByID(52)->setVariable(PV_ALPHA, 1.0);
                game.particleByID(52)->setVariable(PV_WIDTH, 0.02);
                game.particleByID(52)->setVariable(PV_HEIGHT, 0.08);
                game.particleByID(52)->setVariable(PV_SPAWN_X, -1.8);
                game.particleByID(52)->setVariable(PV_SPAWN_W, 1.5);
            }

            static double currentMusicVolume = 0, currentSoundVolume = 0;

            if (currentMusicVolume != game.music_volume)
            {
                for (int i = 0; i < sound_is_music_cutoff; ++i)
                {
                    if (game.game_sounds[i].pDataSource == nullptr || game.sound_paths[i].empty())
                        continue;

                    if (ma_sound_is_playing(&game.game_sounds[i]))
                    {
                        ma_sound_set_volume(&game.game_sounds[i], static_cast<double>(game.music_volume) / 100.0);
                    }
                }
            }
            if (currentSoundVolume != game.sound_volume)
            {
                for (int i = sound_is_music_cutoff; i < sound_limit; ++i)
                {
                    if (game.game_sounds[i].pDataSource == nullptr || game.sound_paths[i].empty())
                        continue;

                    if (ma_sound_is_playing(&game.game_sounds[i]))
                    {
                        ma_sound_set_volume(&game.game_sounds[i], static_cast<double>(game.sound_volume) / 100.0);
                    }
                }
            }
            currentMusicVolume = game.music_volume;
            currentSoundVolume = game.sound_volume;

            int gamepadButtonPressedLast = gamepadInputWatch();
            if (changingControl != control_limit && gamepadButtonPressedLast != -1 && game.characters[playerIDForControl].plControl != nullptr)
            {
                game.characters[playerIDForControl].plControl->gamepad_inputs[changingControl] = static_cast<GAMEPAD_MAP>(gamepadButtonPressedLast);
                changingControl = control_limit;
            }

            std::string playerIDTextStr = std::string("Player ") + std::to_string(playerIDForControl);
            if (playerIDForControlStrElementIndex != -1)
                gui_data.elements[playerIDForControlStrElementIndex].visual.texture_path = playerIDTextStr;

            for (int i = 0; i < control_limit; ++i)
            {
                if (game.characters[playerIDForControl].plControl == nullptr)
                {
                    gui_data.elements[uiElementForControlChangeIndex + i].visual.texture_path = "[No Player]";
                    gui_data.elements[uiElementForControlChangeIndex + i].utype = UI_TEXT;
                    continue;
                }

                gui_data.elements[uiElementForControlChangeIndex + i].utype = UI_CLICKABLE_TEXT;
                if (game.characters[playerIDForControl].plControl->gamepad_id <= -1)
                {
                    gui_data.elements[uiElementForControlChangeIndex + i].visual.texture_path =
                        KeyCodeToString(game.characters[playerIDForControl].plControl->inputs[i]);
                }
                else
                {
                    gui_data.elements[uiElementForControlChangeIndex + i].visual.texture_path =
                        gamepadInputStrings[game.characters[playerIDForControl].plControl->gamepad_inputs[i]];
                }
            }

            fullscreenChangeFunction(window);
        }
        game.particle_update(delta_time);
        if (game.state == WORLD_SCREEN && mainWorld.worldInitialized && prevState == WORLD_SCREEN)
        {
            game.update(mainWorld, delta_time);

            if (playerFacingRight && mainCam.offsetX < 0.2)
            {
                mainCam.offsetX += 2.0 * delta_time;
            }
            if (!playerFacingRight && mainCam.offsetX > -0.2)
            {
                mainCam.offsetX -= 2.0 * delta_time;
            }
            double xleft = mainCam.cameraPosition.x, xright = mainCam.cameraPosition.x;

            // set to player 0
            // check topmost and lowermost players that are on ground and put them in yup and ydown
            // if yup and ydown past holdyup and holdydown levels, update holdy vars
            // if yup and ydown less than players actual distance (inculding not onGround players) then don't lower holdy vars
            // dunno if I'll ever implement this

            for (int i = 0; i < playerCount; ++i)
            {
                if (game.characters[i].visual.x < xleft)
                {
                    xleft = game.characters[i].visual.x;
                }
                if (game.characters[i].visual.x > xright)
                {
                    xright = game.characters[i].visual.x;
                }
                if (!game.characters[i].onGround)
                    continue;
            }

            double distanceCamX = ((xright - xleft) + 8.0) * 9.0;
            if (distanceCamX > mainCam.default_fov * 0.01)
            {
                mainCam.fov = std::min(distanceCamX, mainCam.max_fov);
            }
            camCenterX = (xleft + xright) * 0.5;

            if (game.characters[0].onGround && game.characters[0].visual.y > -0.2 + lowestCamYLevel)
            {
                lowestCamYLevel += 2.0 * delta_time;
            }
            if (game.characters[0].onGround && game.characters[0].visual.y < lowestCamYLevel)
            {
                lowestCamYLevel -= 2.0 * delta_time;
            }
            if (game.characters[0].visual.y < lowestCamYLevel - 0.5)
            {
                lowestCamYLevel = game.characters[0].visual.y + 0.5;
            }
            if (game.characters[0].visual.y > lowestCamYLevel + 0.4)
            {
                lowestCamYLevel = game.characters[0].visual.y - 0.4;
            }
#ifdef COLLISION_DEBUG
            for (int i = 0; i < collision_box_limit; ++i)
            {
                if (mainWorld.collision_boxes[i].collisionID < 0)
                    continue;

                double mdXScale = mainWorld.collision_boxes[i].max_x - mainWorld.collision_boxes[i].min_x;
                double mdYScale = mainWorld.collision_boxes[i].max_y - mainWorld.collision_boxes[i].min_y;
                deCollision.Scale(mdXScale,
                                  mdYScale, 1.0);

                double newX = mainWorld.collision_boxes[i].min_x;
                double newY = mainWorld.collision_boxes[i].min_y;
                deCollision.Put(newX, newY, 0.0);
                deCollision.SetColor(0.5f, 0.5f, 0.5f, 0.5f);
                deCollision.Draw();
            }
#endif

#ifdef COLLISION_DEBUG
            double mpcXScale = game.characters[0].colliders[0].max_x - game.characters[0].colliders[0].min_x;
            double mpcYScale = game.characters[0].colliders[0].max_y - game.characters[0].colliders[0].min_y;
            dePl.Scale(mpcXScale, mpcYScale, 1.0);
            dePl.Put(game.characters[0].colliders[0].min_x, game.characters[0].colliders[0].min_y, 0.0);
            dePl.SetColor(0.5f, 0.5f, 0.5f, 0.5f);
            if (!dePl.empty)
                dePl.Draw();
#endif

            mainWorld.draw();

            for (int i = 0; i < game.characterCount; ++i)
            {
                game.characters[i].visual.Draw();

                if (game.characters[i].plControl == nullptr)
                    continue;

                playerControl(game, game.characters[i], window, &mainWorld);

                if (game.characters[i].hp <= 0 || game.characters[i].visual.y < -20.0)
                {
                    if (i == 0)
                    {
                        game.characters[i].visual.Put(mainWorld.spawnLocationX, -mainWorld.spawnLocationY, 0.0);
                    }
                    else
                    {
                        game.characters[i].visual.Put(game.characters[0].visual.x, game.characters[0].visual.y, 0.0);
                        game.characters[i].velocityY = 1.0f;
                        game.characters[i].velocityX = -1.0f;
                    }
                    game.setParticles("./img/gfx/spawn.png", 4, 1, 15, 4.0, 4.0, 0.0, 0.0, 0.2, 0.2, i + 30);
                    if (game.particleByID(i + 30) != nullptr)
                    {
                        game.particleByID(i + 30)->setVariable(PV_PUSHMIN_Y, -1.0);
                        game.particleByID(i + 30)->setVariable(PV_PUSHMAX_Y, 1.0);
                        game.particleByID(i + 30)->setVariable(PV_PUSHMIN_X, -1.0);
                        game.particleByID(i + 30)->setVariable(PV_PUSHMAX_X, 1.0);
                        game.particleByID(i + 30)->setVariable(PV_RED, game.characters[i].visual.colr);
                        game.particleByID(i + 30)->setVariable(PV_GREEN, game.characters[i].visual.colg);
                        game.particleByID(i + 30)->setVariable(PV_BLUE, game.characters[i].visual.colb);
                        game.particleByID(i + 30)->setVariable(PV_ALPHA, 1.0);
                        game.particleByID(i + 30)->setVariable(PV_WIDTH, 0.1);
                        game.particleByID(i + 30)->setVariable(PV_HEIGHT, 0.1);
                        game.particleByID(i + 30)->setVariable(PV_WIDTH_LIFE_FALLOFF, 0.2);
                        game.particleByID(i + 30)->setVariable(PV_HEIGHT_LIFE_FALLOFF, -0.2);
                        game.particleByID(i + 30)->setVariable(PV_ANIM_START, 0.0);
                        game.particleByID(i + 30)->setVariable(PV_ANIM_END, 4.0);
                        game.particleByID(i + 30)->setVariable(PV_ANIM_SPEED, 8.0);
                        game.particleByID(i + 30)->linkVariable(PV_SPAWN_X, &game.characters[i].visual.x);
                        game.particleByID(i + 30)->linkVariable(PV_SPAWN_W, &game.characters[i].visual.x);
                        game.particleByID(i + 30)->linkVariable(PV_SPAWN_Y, &game.characters[i].visual.y);
                        game.particleByID(i + 30)->linkVariable(PV_SPAWN_H, &game.characters[i].visual.y);
                    }
                    // player animation here???
                    game.characters[i].hp = game.characters[i].maxhp;
                }
            }

            if (game.levelincreasing)
            {
                game.nextState = WON_LEVEL_STATE;
            }
        }
        gui_data.screenDraw(game, window, mainCam, mouseX, mouseY, delta_time, false);

        glfwPollEvents();
        glfwSwapBuffers(window);
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

    double offsetX = xpos - mainCam.lastMouseX;
    double offsetY = mainCam.lastMouseY - ypos;

    const double sensitivity = 0.1f;
    glm::dvec3 direction = glm::dvec3(0.0);

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

        if (pixel_scale >= 1.0)
            break;

        {
            glm::dvec3 pixel_position(mainCam.cameraPosition);

            double inverted_pixel_scale = 1.0 / pixel_scale;
            pixel_position.x = std::round(mainCam.cameraPosition.x * inverted_pixel_scale) / inverted_pixel_scale;
            pixel_position.y = std::round(mainCam.cameraPosition.y * inverted_pixel_scale) / inverted_pixel_scale;
            pixel_position.z = std::round(mainCam.cameraPosition.z * inverted_pixel_scale) / inverted_pixel_scale;
            mainCam.cameraPosition = pixel_position;
        }
        break;
    default:
        mainCam.lastMouseX = xpos;
        mainCam.lastMouseY = ypos;

        offsetX *= sensitivity;
        offsetY *= sensitivity;

        mainCam.yaw += offsetX;
        mainCam.pitch += offsetY;

        if (mainCam.pitch > 89.0)
            mainCam.pitch = 89.0;
        if (mainCam.pitch < -89.0)
            mainCam.pitch = -89.0;

        direction.x = std::cos(glm::radians(mainCam.yaw)) * std::cos(glm::radians(mainCam.pitch));
        direction.y = std::sin(glm::radians(mainCam.pitch));
        direction.z = std::sin(glm::radians(mainCam.yaw)) * std::cos(glm::radians(mainCam.pitch));
        mainCam.cameraFront = glm::normalize(direction);
        direction = glm::dvec3(0.0);
        direction.x = std::cos(glm::radians(mainCam.yaw));
        direction.z = std::sin(glm::radians(mainCam.yaw));
        mainCam.cameraLockedFront = glm::normalize(direction);

        mainCam.cameraRight = glm::cross(mainCam.cameraUp, mainCam.cameraFront);
        mainCam.cameraLockedRight = glm::cross(mainCam.cameraUp, mainCam.cameraLockedFront);
        break;
    }
}
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (game.state == MENU_SCREEN && changingControl != control_limit && game.characters[playerIDForControl].plControl->gamepad_id <= -1)
    {
        game.characters[playerIDForControl].plControl->inputs[changingControl] = key;
        changingControl = control_limit;
    }
}

static game_state returnState;
void goMenuScreen(character *p, game_system *gs, world *w, int argv)
{
    returnState = game.state;
    game.nextState = MENU_SCREEN;
}
void leaveMenuScreen(character *p, game_system *gs, world *w, int argv)
{
    game.nextState = returnState;
}
void processInput(GLFWwindow *window)
{
    if (mainCam.type == CAMERA_3D)
    {
        mainCam.cameraVelocity.x = 0.0;
        mainCam.cameraVelocity.z = 0.0;

        if (mainCam.cameraPosition.y > 0.0)
            mainCam.cameraVelocity.y -= 2.0 * delta_time;
        if (mainCam.cameraPosition.y <= 0.0)
        {
            mainCam.cameraPosition.y = 0.0;
            mainCam.cameraVelocity.y = 0.0;
            mainCam.jumped = false;
        }

        if (glfwGetKey(window, GLFW_KEY_W))
        {
            mainCam.cameraVelocity += glm::dvec3(mainCam.cameraLockedFront.x * 20.0 * delta_time, mainCam.cameraLockedFront.y * 20.0 * delta_time, mainCam.cameraLockedFront.z * 20.0 * delta_time);
        }
        if (glfwGetKey(window, GLFW_KEY_S))
        {
            mainCam.cameraVelocity -= glm::dvec3(mainCam.cameraLockedFront.x * 20.0 * delta_time, mainCam.cameraLockedFront.y * 20.0 * delta_time, mainCam.cameraLockedFront.z * 20.0 * delta_time);
        }
        glm::dvec3 normVec = glm::normalize(glm::cross(mainCam.cameraFront, mainCam.cameraUp));
        if (glfwGetKey(window, GLFW_KEY_A))
        {
            mainCam.cameraVelocity -= glm::dvec3(normVec.x * 20.0 * delta_time, normVec.y * 20.0 * delta_time, normVec.z * 20.0 * delta_time);
        }
        if (glfwGetKey(window, GLFW_KEY_D))
        {
            mainCam.cameraVelocity += glm::dvec3(normVec.x * 20.0 * delta_time, normVec.y * 20.0 * delta_time, normVec.z * 20.0 * delta_time);
        }
        if (!mainCam.jumped && glfwGetKey(window, GLFW_KEY_SPACE))
        {
            mainCam.cameraVelocity.y = 2.0;
            mainCam.jumped = true;
        }
    }
    if (mainCam.type == CAMERA_2D_FOLLOW)
    {
        if (glfwGetKey(window, GLFW_KEY_W))
        {
            mainCam.cameraPosition.y += 10.0 * delta_time;
        }
        if (glfwGetKey(window, GLFW_KEY_S))
        {
            mainCam.cameraPosition.y -= 10.0 * delta_time;
        }
        if (glfwGetKey(window, GLFW_KEY_A))
        {
            mainCam.cameraPosition.x -= 10.0 * delta_time;
        }
        if (glfwGetKey(window, GLFW_KEY_D))
        {
            mainCam.cameraPosition.x += 10.0 * delta_time;
        }
    }

    static bool menuKeyHeld = false;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
    {
        menuKeyHeld = false;
    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && !menuKeyHeld && game.state != MENU_SCREEN)
    {
        returnState = game.state;
        game.nextState = MENU_SCREEN;
        menuKeyHeld = true;
    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && !menuKeyHeld && game.state == MENU_SCREEN)
    {
        game.nextState = returnState;
        menuKeyHeld = true;
    }

    if (glfwGetKey(window, GLFW_KEY_F1))
    {
        mainCam.current_fov = mainCam.zoomed_fov;
    }
    if (!glfwGetKey(window, GLFW_KEY_F1))
    {
        mainCam.current_fov = mainCam.fov;
    }

    if (!glfwGetKey(window, GLFW_KEY_F2) && mainCam.swappedFullscreen)
        mainCam.swappedFullscreen = false;

    if (glfwGetKey(window, GLFW_KEY_F2) && !mainCam.swappedFullscreen)
    {
        mainCam.fullscreen = !mainCam.fullscreen;
        if (mainCam.fullscreen)
            glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0.0, 0.0, window_width, window_height, GLFW_DONT_CARE);
        if (!mainCam.fullscreen)
            glfwSetWindowMonitor(window, NULL, 0.0, 0.0, window_width, window_height, GLFW_DONT_CARE);

        mainCam.swappedFullscreen = true;
    }
    if (glfwGetKey(window, GLFW_KEY_F4))
    {
        gui_data.quit = true;
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
}

void playerControl(game_system &game, character &p, GLFWwindow *window, world *floor)
{
    p.velocityX = 0.0;

    if (p.plControl->getInput(window, CONTROL_LEFT))
    {
        p.velocityX = -p.runSpeed;
        p.visual.Rotate(0.0, 180.0, 0.0);
        p.visual.xOffset = p.visual.trueW();
        if (p.onGround && p.animations[ANIM_ABILITY_0].finished && p.animations[ANIM_ABILITY_1].finished)
            p.PlayAnimation(ANIM_WALK, delta_time, true);
        p.walkingkeypressed = true;
        playerFacingRight = false;
    }
    if (p.plControl->getInput(window, CONTROL_RIGHT))
    {
        p.velocityX = p.runSpeed;
        p.visual.Rotate(0.0, 0.0, 0.0);
        p.visual.xOffset = 0.0;
        if (p.onGround && p.animations[ANIM_ABILITY_0].finished && p.animations[ANIM_ABILITY_1].finished)
            p.PlayAnimation(ANIM_WALK, delta_time, true);

        p.walkingkeypressed = true;
        playerFacingRight = true;
    }
    if (p.plControl->getInput(window, CONTROL_DOWN) && !p.onGround)
    {
        p.velocityY = 5.0 * -p.runSpeed;
    }
    if (p.playingAnim != ANIM_WALK || p.animations[ANIM_WALK].frame != 1)
    {
        p.stepsoundplayed = false;
    }
    if (p.playingAnim == ANIM_WALK && p.animations[ANIM_WALK].frame == 1 && !p.stepsoundplayed)
    {
        game.playSound(3, 0.15, true);
        p.stepsoundplayed = true;
    }
    if (!p.walkingkeypressed)
    {
        p.StopAnimation(ANIM_WALK);
    }
    if (p.onGround && !p.plControl->getInput(window, CONTROL_UP))
    {
        p.jumped = false;
    }
    if ((p.onGround || p.parrySuccess) && !p.jumped && p.plControl->getInput(window, CONTROL_UP))
    {
        p.velocityY = 1.8 * p.runSpeed;
        p.jumped = true;
        p.onGround = false;
        p.parrySuccess = false;
    }

    if (p.parryTimer > 0)
    {
        p.parryTimer -= 60.0f * delta_time;
    }
    if (p.parryTimer <= 0 && p.parrySuccess)
    {
        p.parrySuccess = false;
        p.velocityY = 1.0;
    }
    if (!p.plControl->getInput(window, CONTROL_SHIELD))
    {
        p.parryButtonPressed = false;
    }

    if (p.hp == 0 && p.parryTimer > (p.parryCooloff - p.parryWindow))
    {
        p.hp = p.maxhp;
        p.parrySuccess = true;
        p.jumped = false;
    }

    if (p.plControl->getInput(window, CONTROL_SHIELD) && !p.parryButtonPressed)
    {
        if (p.parryTimer <= 0)
        {
            p.PlayAnimation(ANIM_ABILITY_0, delta_time, false);
            p.parryTimer = p.parryCooloff;
        }
        p.parryButtonPressed = true;
    }

    if (p.strikeTimer >= 0.0)
    {
        if (p.strikeTimer > p.strikeCooloff - p.strikeWindow)
        {
            p.colliderOn(COLLIDER_STRIKE);
            p.striking = true;
        }
        p.strikeTimer -= 60.0 * delta_time;
    }
    if (p.strikeTimer <= 0.0 && p.strikeButtonPressed) // seems strange keep an eye on this?
    {
        p.colliderOff(COLLIDER_STRIKE);
        p.strikeButtonPressed = false;
    }
    if (p.plControl->getInput(window, CONTROL_SWORD) && !p.strikeButtonPressed)
    {
        p.PlayAnimation(ANIM_ABILITY_1, delta_time, false);
        p.strikeButtonPressed = true;
        p.strikeTimer = p.strikeCooloff;
    }
}

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

void playerInit(character &pl, game_system &game, player &controller)
{
    pl = character(game.shaders[GAME_SHADER_DEFAULT], game.objects[GAME_OBJECT_DEFAULT], "./img/char/knight.png", -120.0, -40.0, 4, 3, CH_PLAYER);
    pl.setCollider(COLLIDER_SOLID, aabb(0.0, 0.0, pl.visual.trueW() * 0.5, pl.visual.trueH() * 0.75), pl.visual.trueW() * 0.25, 0.0);
    pl.setCollider(COLLIDER_STRIKE,
                   aabb(pl.visual.x - pl.visual.trueW() * 0.5, pl.visual.y, pl.visual.x + pl.visual.trueW(), pl.visual.y + pl.visual.trueH() * 0.5), 0.0, 0.0);
    pl.colliderOn(COLLIDER_SOLID);

    if (glfwJoystickIsGamepad(playerGamepadCount + 1))
    {
        ++playerGamepadCount;
        controller.gamepad_id = playerGamepadCount;
    }
    pl.plControl = &controller;

    pl.SetAnimation(ANIM_IDLE, 0, 0, 0.0);
    pl.SetAnimation(ANIM_WALK, 0, 1, 100.0);
    pl.SetAnimation(ANIM_HURT, 6, 10, 250.0);
    pl.SetAnimation(ANIM_DEAD, 11, 11, 100.0);
    pl.SetAnimation(ANIM_ABILITY_2, 1, 1, 100.0);
    pl.SetAnimation(ANIM_ABILITY_0, 2, 2, 100.0);
    pl.SetAnimation(ANIM_ABILITY_1, 3, 3, 100.0);
    // pl.SetAnimation(ANIM_ABILITY_0, 2, 2, 0.0);
    // pl.SetAnimation(ANIM_ABILITY_1, 3, 3, 0.0);

    game.Add(pl);
}

void worldInit(game_system &game, world &dg, std::string tilePath, std::string levelPath, unsigned int fx, unsigned int fy)
{
    dg = world(tilePath.c_str(), fx, fy, game.objects[GAME_OBJECT_TILEMAP], game.shaders[GAME_SHADER_DEFAULT]);
    dg.readRoomFile(levelPath.c_str(), *game.objects[GAME_OBJECT_TILEMAP]);
}

// feh
void updateView(shader &_program, bool orthographic)
{
    glm::mat4 view = glm::mat4(1.0);
    glm::mat4 proj;

    _program.use();
    if (!orthographic)
    {
        view = glm::lookAt(mainCam.cameraPosition, mainCam.cameraPosition + mainCam.cameraFront, mainCam.cameraUp);
        proj = glm::perspective(glm::radians(mainCam.current_fov), static_cast<double>(window_width) / static_cast<double>(window_height), 0.01, 200.0);
    }
    if (orthographic)
        proj = glm::ortho(0.0, 1280.0, 0.0, 720.0, -1.0, 1.0);

    _program.setUniformMat4("projection", proj);

    if (!orthographic)
        _program.setUniformMat4("view", view);
}
void fullscreenChangeFunction(GLFWwindow *window)
{
    static bool fullscreenLast;
    if (mainCam.fullscreen == fullscreenLast)
        return;

    if (mainCam.fullscreen)
    {
        glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0.0, 0.0, window_width, window_height, GLFW_DONT_CARE);
    }
    if (!mainCam.fullscreen)
        glfwSetWindowMonitor(window, NULL, 0.0, 0.0, window_width, window_height, GLFW_DONT_CARE);

    fullscreenLast = mainCam.fullscreen;
}
void addPlayer(character *ch, game_system *gs, world *wo, int x)
{
    if (playerCount >= player_limit)
        return;

    playerInit(gs->characters[playerCount], *gs, playerControllers[playerCount]);
    gs->characters[playerCount].visual.Scale(0.48, 0.48, 1.0);
    gs->characters[playerCount].visual.SetColor(0.083333334 * static_cast<double>(playerCount + 1),
                                                0.083333334 * static_cast<double>(playerCount + 1),
                                                0.083333334 * static_cast<double>(playerCount + 1), 1.0);
    playerCount++;
}
void removePlayer(character *ch, game_system *gs, world *wo, int x)
{
    if (playerCount <= 1)
        return;

    playerCount--;
    gs->Remove(playerCount);
}

void changeControlFunc(character *ch, game_system *gs, world *wo, int x)
{
    changingControl = static_cast<controlset>(x);

    if (ch->plControl == nullptr)
        return;

    watchGamepadID = ch->plControl->gamepad_id; // will be -1 if no gamepad
}
void volumeChangeSoundPlayFunc(character *ch, game_system *gs, world *wo, int x)
{
    gs->playSound(6, 0.0, true);
}

int gamepadInputWatch()
{
    if (watchGamepadID <= -1)
        return -1;

    GLFWgamepadstate gState;

    if (!glfwGetGamepadState(GLFW_JOYSTICK_1 + watchGamepadID, &gState))
        return -1;

    for (int i = 0; i < GLFW_GAMEPAD_BUTTON_LAST + 1; ++i)
    {
        if (gState.buttons[i])
        {
            return i;
        }
    }

    double realSensitivity = gamepad_stick_sensitivity * 0.001f;
    if (gState.axes[0] < -realSensitivity)
    {
        return PAD_LSTICK_LEFT;
    }
    if (gState.axes[0] > realSensitivity)
    {
        return PAD_LSTICK_RIGHT;
    }
    if (gState.axes[1] < -realSensitivity)
    {
        return PAD_LSTICK_UP;
    }
    if (gState.axes[1] > realSensitivity)
    {
        return PAD_LSTICK_DOWN;
    }
    if (gState.axes[2] < -realSensitivity)
    {
        return PAD_RSTICK_LEFT;
    }
    if (gState.axes[2] > realSensitivity)
    {
        return PAD_RSTICK_RIGHT;
    }
    if (gState.axes[3] < -realSensitivity)
    {
        return PAD_RSTICK_UP;
    }
    if (gState.axes[3] > realSensitivity)
    {
        return PAD_RSTICK_DOWN;
    }
    if (gState.axes[4] > realSensitivity)
    {
        return PAD_TRIGGER_L;
    }
    if (gState.axes[5] > realSensitivity)
    {
        return PAD_TRIGGER_R;
    }
    return -1;
}
void increaseLevel(character *ch, game_system *gs, world *wo, int x)
{
    if (gs == nullptr)
        return;

    gs->levelincreasing = true;
}
void incrementPlayerIDForControlFunc(character *ch, game_system *gs, world *wo, int x)
{
    playerIDForControl += x;
    if (playerIDForControl < 0)
    {
        int cycleDifference = playerIDForControl;
        playerIDForControl = playerCount;
        playerIDForControl += cycleDifference;
    }
    if (playerIDForControl >= playerCount)
    {
        int cycleDifference = playerIDForControl - playerCount;
        playerIDForControl = cycleDifference;
    }
}
void fullScreenToggleFunc(character *ch, game_system *gs, world *wo, int x)
{
    mainCam.fullscreen = !mainCam.fullscreen;
}
void particleToggleFunc(character *ch, game_system *gs, world *wo, int x)
{
    gs->particlesenabled = !gs->particlesenabled;
    if (!gs->particlesenabled)
    {
        gs->killParticles();
    }
}

constexpr const char *const LUT44To96[]{"Comma",
                                        "Minus",
                                        "Period",
                                        "Slash",
                                        "Num0",
                                        "Num1",
                                        "Num2",
                                        "Num3",
                                        "Num4",
                                        "Num5",
                                        "Num6",
                                        "Num7",
                                        "Num8",
                                        "Num9",
                                        "Invalid",
                                        "Semicolon",
                                        "Invalid",
                                        "Equal",
                                        "Invalid",
                                        "Invalid",
                                        "Invalid",
                                        "A",
                                        "B",
                                        "C",
                                        "D",
                                        "E",
                                        "F",
                                        "G",
                                        "H",
                                        "I",
                                        "J",
                                        "K",
                                        "L",
                                        "M",
                                        "N",
                                        "O",
                                        "P",
                                        "Q",
                                        "R",
                                        "S",
                                        "T",
                                        "U",
                                        "V",
                                        "W",
                                        "X",
                                        "Y",
                                        "Z",
                                        "LeftBracket",
                                        "Backslash",
                                        "RightBracket",
                                        "Invalid",
                                        "Invalid",
                                        "GraveAccent"};

constexpr const char *const LUT256To348[]{"Escape",
                                          "Enter",
                                          "Tab",
                                          "Backspace",
                                          "Insert",
                                          "Delete",
                                          "Right",
                                          "Left",
                                          "Down",
                                          "Up",
                                          "PageUp",
                                          "PageDown",
                                          "Home",
                                          "End",
                                          "Invalid",
                                          "Invalid",
                                          "Invalid",
                                          "Invalid",
                                          "Invalid",
                                          "Invalid",
                                          "Invalid",
                                          "Invalid",
                                          "Invalid",
                                          "Invalid",
                                          "CapsLock",
                                          "ScrollLock",
                                          "NumLock",
                                          "PrintScreen",
                                          "Pause",
                                          "Invalid",
                                          "Invalid",
                                          "Invalid",
                                          "Invalid",
                                          "Invalid",
                                          "F1",
                                          "F2",
                                          "F3",
                                          "F4",
                                          "F5",
                                          "F6",
                                          "F7",
                                          "F8",
                                          "F9",
                                          "F10",
                                          "F11",
                                          "F12",
                                          "F13",
                                          "F14",
                                          "F15",
                                          "F16",
                                          "F17",
                                          "F18",
                                          "F19",
                                          "F20",
                                          "F21",
                                          "F22",
                                          "F23",
                                          "F24",
                                          "F25",
                                          "Invalid",
                                          "Invalid",
                                          "Invalid",
                                          "Invalid",
                                          "Invalid",
                                          "Keypad0",
                                          "Keypad1",
                                          "Keypad2",
                                          "Keypad3",
                                          "Keypad4",
                                          "Keypad5",
                                          "Keypad6",
                                          "Keypad7",
                                          "Keypad8",
                                          "Keypad9",
                                          "KeypadDecimal",
                                          "KeypadDivide",
                                          "KeypadMultiply",
                                          "KeypadSubtract",
                                          "KeypadAdd",
                                          "KeypadEnter",
                                          "KeypadEqual",
                                          "Invalid",
                                          "Invalid",
                                          "Invalid",
                                          "LeftShift",
                                          "LeftControl",
                                          "LeftAlt",
                                          "LeftSuper",
                                          "RightShift",
                                          "RightControl",
                                          "RightAlt",
                                          "RightSuper",
                                          "Menu"};

constexpr const char *KeyCodeToString(int keycode) noexcept
{
    if (keycode == 32)
    {
        return "Space";
    } // Common key, don't treat as an unlikely scenario

    if (keycode >= 44 && keycode <= 96) [[likely]]
    {
        return LUT44To96[keycode - 44];
    }

    if (keycode >= 256 && keycode <= 348) [[likely]]
    {
        return LUT256To348[keycode - 256];
    }

    // Unlikely scenario where the keycode didn't fall inside one of the two lookup tables
    switch (keycode)
    {
    case 39:
        return "Apostrophe";
    case 161:
        return "World1";
    case 162:
        return "Wordl2";
    default:
        return "Unknown";
    }
}