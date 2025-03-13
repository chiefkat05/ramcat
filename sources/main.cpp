// animation commands should be inside sprite class and have the following public functions:
//  sprite.runFrames(x, y, spd)
//  sprite.saveFrames(id, x, y, spd)
//  sprite.runFrames(id)

// clean this up and put all functions at the bottom so menuData and main can be easiest to access

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
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

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
// std::string GAMEPAD_MAP_STRINGS[];

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
        game.playSound(1, 0.15f, 0);
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

bool pauseKeyHeld = false, uiKeyHeld = false, showUI = true;
player playerControllers[player_limit];
character players[player_limit];

std::string controlsetstrings[] = {
    "Up", "Left", "Right", "Down", "Shield", "Sword", "Bubble", "Spawn"};

// special thanks to https://gist.github.com/0xD34DC0DE/910855d41786b962127ae401da2a3441 for the strings so I don't have to do this myself
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

const char *gamepadInputStrings[] = {"PAD_BUTTON_A", "PAD_BUTTON_B", "PAD_BUTTON_X", "PAD_BUTTON_Y", "PAD_BUTTON_LBUTTON",
                                     "PAD_BUTTON_RBUTTON", "PAD_BUTTON_BACK", "PAD_BUTTON_START", "PAD_BUTTON_HOME", "PAD_BUTTON_LSTICK",
                                     "PAD_BUTTON_RSTICK", "PAD_DPAD_UP", "PAD_DPAD_RIGHT", "PAD_DPAD_DOWN", "PAD_DPAD_LEFT", "how did you do this",
                                     "PAD_LSTICK_LEFT", "PAD_LSTICK_RIGHT", "PAD_LSTICK_UP", "PAD_LSTICK_DOWN", "PAD_RSTICK_LEFT", "PAD_RSTICK_RIGHT",
                                     "PAD_RSTICK_UP", "PAD_RSTICK_DOWN", "PAD_LTRIGGER", "PAD_RTRIGGER"};

void playerInit(character &pl, game_system &game, player &controller)
{
    pl = character("./img/char/knight.png", -120.0f, -40.0f, 4, 3, CH_PLAYER);
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

void dungeonInit(game_system &game, dungeon &dg, std::string tilePath, std::string levelPath, unsigned int fx, unsigned int fy)
{
    dg = dungeon(tilePath.c_str(), fx, fy);
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

    playerInit(players[playerCount], *gs, playerControllers[playerCount]);
    players[playerCount].visual.Scale(0.48f, 0.48f, 1.0f);
    playerCount++;
}
void removePlayer(character *ch, game_system *gs, dungeon *dg, int x)
{
    if (playerCount <= 1)
        return;

    // delete (&players[playerCount]);
    playerCount--;
}
/**
    CONTROL_UP,
    CONTROL_LEFT,
    CONTROL_RIGHT,
    CONTROL_DOWN,
    CONTROL_SHIELD,
    CONTROL_SWORD,
    CONTROL_BUBBLE,
    CONTROL_SPAWN_PLAYER,
    control_limit **/
controlset changingControl = control_limit;
int uiElementForControlChangeIndex = 0;
int playerIDForControl = 0;
int watchGamepadID = -1;
void changeControlFunc(character *ch, game_system *gs, dungeon *dg, int x)
{
    changingControl = static_cast<controlset>(x);

    if (ch->plControl == nullptr)
        return;

    watchGamepadID = ch->plControl->gamepad_id; // will be -1 if no gamepad
}

extern float gamepad_stick_sensitivity;
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
            break;
        }
    }
    if (gState.axes[0] < -gamepad_stick_sensitivity)
    {
        return PAD_LSTICK_LEFT;
    }
    if (gState.axes[0] > gamepad_stick_sensitivity)
    {
        return PAD_LSTICK_RIGHT;
    }
    if (gState.axes[1] < -gamepad_stick_sensitivity)
    {
        return PAD_LSTICK_UP;
    }
    if (gState.axes[1] > gamepad_stick_sensitivity)
    {
        return PAD_LSTICK_DOWN;
    }
    if (gState.axes[2] < -gamepad_stick_sensitivity)
    {
        return PAD_RSTICK_LEFT;
    }
    if (gState.axes[2] > gamepad_stick_sensitivity)
    {
        return PAD_RSTICK_RIGHT;
    }
    if (gState.axes[3] < -gamepad_stick_sensitivity)
    {
        return PAD_RSTICK_UP;
    }
    if (gState.axes[3] > gamepad_stick_sensitivity)
    {
        return PAD_RSTICK_DOWN;
    }
    if (gState.axes[4] > gamepad_stick_sensitivity)
    {
        return PAD_TRIGGER_L;
    }
    if (gState.axes[5] > gamepad_stick_sensitivity)
    {
        return PAD_TRIGGER_R;
    }
    return -1;
    // for (int i = 0; i < GLFW_GAMEPAD_AXIS_LAST; ++i)
    // {

    // }
}
void incrementPlayerIDForControlFunc(character *ch, game_system *gs, dungeon *dg, int x)
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
void fullScreenToggleFunc(character *ch, game_system *gs, dungeon *dg, int x)
{
    mainCam.fullscreen = !mainCam.fullscreen;
}
void goMenuScreen(character *p, game_system *gs, dungeon *d, int argv);
void leaveMenuScreen(character *p, game_system *gs, dungeon *d, int argv);

int playerIDForControlStrElementIndex = 0;
int prevState = -1;
#ifdef COLLISION_DEBUG
sprite delots[20];
sprite dePl;
#endif
// edit all guis here
void menuData(game_system &mainG, character &p1, dungeon &floor, ma_engine &s_engine)
{
    if (state == prevState)
        return;

    gui_data.elements.clear();

    std::string playerIDForControlStr = "Player " + std::to_string(playerIDForControl);

    // sound channels system
    // for (int i = 0; i < sound_limit; ++i) // sounds need to stop but also play lmao // maybe also need fade out or stuff // maybe fade out implementation will fix the other problem try it! // just use sound channels to overwrite on new state
    // {
    //     mainG.stopSound(i);
    // }
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

        mainG.initSound("./snd/mus/fellowtheme.mp3", 0, &s_engine);
        mainG.playSound(0, 1, 0);
        gui_data.elements.push_back(ui_element(UI_IMAGE, "./img/menu.png", 0.0f, 0.0f, 128.0f, 64.0f, 3, 1, nullFunc, true));
        gui_data.elements.push_back(ui_element(UI_CLICKABLE, "./img/quit.png", -0.1f, -0.5f, 9.0f, 10.0f, 1, 1, quitGame));
        gui_data.elements.push_back(ui_element(UI_CLICKABLE, "./img/options.png", -0.3f, -0.5f, 9.0f, 10.0f, 1, 1, goMenuScreen));
        gui_data.elements.push_back(ui_element(UI_CLICKABLE, "./img/play.png", -0.5f, -0.5f, 9.0f, 10.0f, 1, 1, startGame, false, nullptr, nullptr, nullptr, CHARACTER_CREATION_SCREEN));
        mainG.level = 0;
        mainG.levelincreasing = false;
        break;
    case MENU_SCREEN:
        mainCam.setBoundary(0.0f, -0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
        mainCam.lockTo(nullptr, nullptr, nullptr);
        mainCam.cameraPosition = glm::vec3(0.0f, 0.0f, 1.0f);
        mainCam.offsetX = 0.0f;
        mainCam.offsetY = 0.0f;
        mainCam.offsetZ = 0.0f;
        gui_data.elements.push_back(ui_element(UI_IMAGE, "./img/menu.png", 0.0f, 0.0f, 128.0f, 64.0f, 3, 1, nullFunc, true));
        gui_data.elements.push_back(ui_element(UI_CLICKABLE, "./img/back.png", 0.8f, -0.2f, 9.0f, 10.0f, 1, 1, leaveMenuScreen));
        gui_data.elements.push_back(ui_element(UI_CLICKABLE, "./img/home.png", 0.8f, -0.5f, 9.0f, 10.0f, 1, 1, startGame, false, nullptr, nullptr, nullptr, START_SCREEN));
        gui_data.elements.push_back(ui_element(UI_CLICKABLE, "./img/quit.png", 0.8f, -0.8f, 9.0f, 10.0f, 1, 1, quitGame));
        mainG.initSound("./snd/mus/fellowtheme.mp3", 0, &s_engine);
        mainG.playSound(0, 1, 0);

        gui_data.elements.push_back(ui_element(UI_TEXT, "Settings", 25.0f, 625.0f, 64.0f, 0.0f, 1, 1));
        gui_data.elements.push_back(ui_element(UI_TEXT, "Controls", 100.0f, 575.0f, 1.6f * 32.0f, 0.0f, 1, 1));
        gui_data.elements.push_back(ui_element(UI_TEXT, playerIDForControlStr.c_str(), 125.0f, 500.0f, 1.2f * 32.0f, 0.0f, 1, 1));
        playerIDForControlStrElementIndex = gui_data.elements.size() - 1;
        gui_data.elements.push_back(ui_element(UI_CLICKABLE, "./img/arrow_left.png", -0.8f, 0.45f, 10.0f, 10.0f, 1, 1, incrementPlayerIDForControlFunc, false, nullptr, nullptr, nullptr, -1));
        gui_data.elements.push_back(ui_element(UI_CLICKABLE, "./img/arrow_right.png", -0.2f, 0.45f, 10.0f, 10.0f, 1, 1, incrementPlayerIDForControlFunc, false, nullptr, nullptr, nullptr, 1));
        for (int i = 0; i < control_limit; ++i)
        {
            gui_data.elements.push_back(ui_element(UI_TEXT, controlsetstrings[i].c_str(), 50.0f, 425.0f - i * 50.0f, 1.6f * 32.0f, 0.0f, 1, 1));
        }
        if (playerIDForControl >= playerCount)
            playerIDForControl = playerCount - 1;

        uiElementForControlChangeIndex = gui_data.elements.size();
        for (int i = 0; i < control_limit; ++i)
        {
            // control buttons here
            if (players[playerIDForControl].plControl == nullptr)
            {
                continue;
            }
            if (players[playerIDForControl].plControl->gamepad_id <= -1)
            {
                gui_data.elements.push_back(ui_element(UI_CLICKABLE_TEXT, KeyCodeToString(players[playerIDForControl].plControl->inputs[i]), 250.0f, 430.0f - i * 50.0f, 50.0f, 1.0f,
                                                       1, 1, changeControlFunc, false, &players[playerIDForControl], &mainG, &floor, i));
            }
            else
            {
                gui_data.elements.push_back(ui_element(UI_TEXT, gamepadInputStrings[players[playerIDForControl].plControl->gamepad_inputs[i]], 250.0f, 430.0f - i * 50.0f, 50.0f, 1.0f,
                                                       1, 1, changeControlFunc, false, &players[playerIDForControl], &mainG, &floor, i));
            }
        }

        break;
    case CHARACTER_CREATION_SCREEN:
        for (int i = 0; i < playerCount; ++i)
        {
            players[i].visual.Scale(0.48f, 0.48f, 1.0f);
        }
        gui_data.elements.push_back(ui_element(UI_IMAGE, "./img/menu-char.png", 0.0f, 0.0f, 128.0f, 64.0f, 1, 1, nullFunc, true));
        gui_data.elements.push_back(ui_element(UI_CLICKABLE, "./img/add_player.png", 0.2f, 0.4f, 16.0f, 16.0f, 1, 1, addPlayer, false, &p1, &mainG, &floor));
        gui_data.elements.push_back(ui_element(UI_CLICKABLE, "./img/add_player.png", 0.4f, 0.4f, 16.0f, 16.0f, 1, 1, removePlayer, false, &p1, &mainG, &floor));
        gui_data.elements.push_back(ui_element(UI_CLICKABLE, "./img/play.png", -0.5f, -0.5f, 9.0f, 10.0f, 1, 1, startGame, false, nullptr, nullptr, nullptr, DUNGEON_SCREEN));
        break;
    case DUNGEON_SCREEN:
        for (int i = 0; i < playerCount; ++i)
        {
            players[i].visual.Scale(0.32f, 0.32f, 1.0f);
        }
        mainCam.lockTo(&p1.visual.x, &lowestCamYLevel);
        mainG.initSound("./snd/mus/castle-1.mp3", 0, &s_engine);
        mainG.initSound("./snd/fx/kstep.wav", 1, &s_engine);
        mainG.playSound(0, 1.5f, 0);
        gui_data.elements.push_back(ui_element(UI_IMAGE, "./img/bg/01.png", 0.0f, 0.0f, 1280.0f, 520.0f, 3, 1, nullFunc, true));
        if (mainG.levelincreasing)
        {
            mainG.levelincreasing = false;
            mainG.level++;
        }
        switch (mainG.level)
        {
        case 0:
            dungeonInit(mainG, floor, "./img/tiles.png", "./levels/01.lvl", 4, 4);
            break;
        case 1:
            dungeonInit(mainG, floor, "./img/tiles.png", "./levels/02.lvl", 4, 4);
            break;
        case 2:
            dungeonInit(mainG, floor, "./img/tiles.png", "./levels/03.lvl", 4, 4);
            break;
            // break;
        case 3:
            dungeonInit(mainG, floor, "./img/tiles.png", "./levels/04.lvl", 4, 4);
            break;
        case 4:
            state = START_SCREEN;
            // dungeonInit(mainG, floor, "./img/tiles.png", "./levels/B.lvl", 4, 4);
            return;
        case 5:
            dungeonInit(mainG, floor, "./img/tiles.png", "./levels/W.lvl", 4, 4);
            break;
        case 6:
            // state = MENU_SCREEN;
            state = START_SCREEN;
            return;
        default:
            std::cout << ":megamind: no level?\n";
            dungeonInit(mainG, floor, "./img/tiles.png", "./levels/01.lvl", 4, 2);
            break;
        }
        mainCam.setBoundary(1.9f, -0.0f, -1.0f, floor.roomWidth * 0.16f, 50.0f, 1.0f);

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
        for (int i = 0; i < playerCount; ++i)
        {
            if (players[i].plControl == nullptr)
                continue;

            players[i].visual.Put(floor.spawnLocationX, -floor.spawnLocationY + playerSpawnDist, 0.0f);
            playerSpawnDist += players[i].visual.h + 0.02f;
        }
        lowestCamYLevel = p1.visual.y;

        break;
    case WON_LEVEL_STATE:
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

    shader shaderProgram("./shaders/default.vertex", "./shaders/default.fragment");
    shader textShaderProgram("./shaders/text.vertex", "./shaders/text.fragment");

    object spriteRect(OBJ_QUAD);
    object spriteCube(OBJ_CUBE);
    object spriteText(OBJ_TEXT);

    float current_time = 0;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    dungeon mainDungeon("./img/tiles.png", 4, 2); // lmao
    mainDungeon.readRoomFile("./levels/01.lvl");
    mainCam.cameraPosition = glm::vec3(0.0f, 0.0f, 1.0f);
    playerInit(players[0], game, playerControllers[0]);
    prevState = WIN_SCREEN;

    ma_engine soundEngine;

    ma_result game_sound_result = ma_engine_init(nullptr, &soundEngine);
    if (game_sound_result != MA_SUCCESS)
    {
        std::cout << game_sound_result << " sound error\n";
    }

    loadFont("./stb/rainyhearts.ttf");
    glm::mat4 textProjection = glm::ortho(0.0f, static_cast<float>(window_width), 0.0f, static_cast<float>(window_height));
    textShaderProgram.use();
    textShaderProgram.setUniformMat4("projection", textProjection);

    while (!glfwWindowShouldClose(window)) // now what // screen resolution options, fullscreen toggle, volume sliders // or just volume buttons no pressure
    {
        float past_time = current_time;
        current_time = glfwGetTime();
        delta_time = current_time - past_time;

        mouseUpdate(window);
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        updateView(shaderProgram);

        if (state == WON_LEVEL_STATE && game.levelincreasing)
        {
            prevState = WON_LEVEL_STATE;
            state = DUNGEON_SCREEN;
        }
        menuData(game, players[0], mainDungeon, soundEngine);
        gui_data.screenDraw(window, shaderProgram, textShaderProgram, spriteRect, spriteText, mouseX, mouseY, mousePressed, mouseReleased, delta_time, true);
        for (int i = 0; i < game.particlesystemcount; ++i)
        {
            game.particles[i]->draw(window, shaderProgram, spriteRect, delta_time);
        }

        if (state == CHARACTER_CREATION_SCREEN)
        {
            for (int i = 0; i < playerCount; ++i)
            {
                players[i].visual.Put(-1.4f + i * 0.22f, 0.0f, 0.0f);
                players[i].visual.Draw(shaderProgram, spriteRect);
            }
            renderText(spriteText, textShaderProgram, "Character Select", 25.0f, 625.0f, 2.0f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        }
        if (state == MENU_SCREEN)
        {
            int gamepadButtonPressedLast = gamepadInputWatch();
            if (changingControl != control_limit && gamepadButtonPressedLast != -1 && players[playerIDForControl].plControl != nullptr)
            {
                players[playerIDForControl].plControl->gamepad_inputs[changingControl] = static_cast<GAMEPAD_MAP>(gamepadButtonPressedLast);
                changingControl = control_limit;
            }

            std::string playerIDTextStr = std::string("Player ") + std::to_string(playerIDForControl);
            gui_data.elements[playerIDForControlStrElementIndex].visual.texture_path = playerIDTextStr.c_str();

            for (int i = 0; i < control_limit; ++i)
            {
                if (players[playerIDForControl].plControl == nullptr)
                {
                    gui_data.elements[uiElementForControlChangeIndex + i].visual.texture_path = "[No Player]";
                    gui_data.elements[uiElementForControlChangeIndex + i].utype = UI_TEXT;
                    continue;
                }

                gui_data.elements[uiElementForControlChangeIndex + i].utype = UI_CLICKABLE_TEXT;
                if (players[playerIDForControl].plControl->gamepad_id <= -1)
                {
                    gui_data.elements[uiElementForControlChangeIndex + i].visual.texture_path =
                        KeyCodeToString(players[playerIDForControl].plControl->inputs[i]);
                }
                else
                {
                    gui_data.elements[uiElementForControlChangeIndex + i].visual.texture_path =
                        gamepadInputStrings[players[playerIDForControl].plControl->gamepad_inputs[i]];
                }
            }

            gui_data.elements.push_back(ui_element(UI_CLICKABLE_TEXT, "Toggle Fullscreen", 500.0f, 700.0f, 32.0f, 0.0f, 1, 1, fullScreenToggleFunc));
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

            mainDungeon.draw(window, shaderProgram, spriteRect);

            for (int i = 0; i < game.characterCount; ++i)
            {
                if (game.characters[i]->visual.empty)
                    continue;

                game.characters[i]->visual.Draw(shaderProgram, spriteRect);

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
        gui_data.screenDraw(window, shaderProgram, textShaderProgram, spriteRect, spriteText, mouseX, mouseY, mousePressed, mouseReleased, delta_time, false);

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
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (state == MENU_SCREEN && changingControl != control_limit && players[playerIDForControl].plControl->gamepad_id <= -1)
    {
        players[playerIDForControl].plControl->inputs[changingControl] = key;
        changingControl = control_limit;
    }
}

static game_state returnState;
void goMenuScreen(character *p, game_system *gs, dungeon *d, int argv)
{
    returnState = state;
    state = MENU_SCREEN;
}
void leaveMenuScreen(character *p, game_system *gs, dungeon *d, int argv)
{
    state = returnState;
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

    static bool menuKeyHeld = false;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
    {
        menuKeyHeld = false;
    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && !menuKeyHeld && state != MENU_SCREEN)
    {
        returnState = state;
        state = MENU_SCREEN;
        menuKeyHeld = true;
    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && !menuKeyHeld && state == MENU_SCREEN)
    {
        state = returnState;
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