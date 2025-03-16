#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "sprite.h"
// #include "network.h"
#include "system.h"
#include <vector>

extern bool buttonHovered;

enum ui_element_type
{
    UI_CLICKABLE,
    UI_VALUEISFRAME,
    UI_IMAGE,
    UI_TEXT,
    UI_CLICKABLE_TEXT,
    UI_SLIDER,    // can use value for slider position?
    UI_DROPDOWN,  // can use value for index number?
    UI_TEXT_INPUT // can use visual.texture_path for text
};

void nullFunc(character *p, game_system *gs, dungeon *d, int argv);

struct ui_element
{
    sprite visual;
    float trueX, trueY;
    float posX, posY, width, height, sliderPos = 0.0f, sliderLimit = 1.0f;
    int *value;

    void (*function)(character *, game_system *, dungeon *, int);
    character *func_p;
    game_system *func_gs;
    dungeon *func_d;
    int func_i;
    bool background = false;

    ui_element_type utype;
    animation anim;

    // ui_element(ui_element_type t, sprite *v, float x, float y, void func(character *, game_system *, dungeon *, int), bool bg = false,
    //            character *_func_p = nullptr, game_system *_func_gs = nullptr, dungeon *_func_d = nullptr,
    //            int _func_i = 0, int *_linkValue = nullptr);
    ui_element(ui_element_type t, const char *path, float x, float y, float w, float h, int frX, int frY,
               void func(character *, game_system *, dungeon *, int) = nullFunc, bool bg = false,
               character *_func_p = nullptr, game_system *_func_gs = nullptr, dungeon *_func_d = nullptr,
               int _func_i = 0, int *_linkValue = nullptr);

    void update(GLFWwindow *window, float mouseX, float mouseY, bool mousePressed, bool mouseReleased, float delta_time);
};

enum game_state
{
    START_SCREEN,
    MENU_SCREEN,
    CHARACTER_CREATION_SCREEN,
    DUNGEON_SCREEN,
    WON_LEVEL_STATE,
    LOSE_SCREEN,
    WIN_SCREEN,
    state_total_count
};

struct gui
{
    std::vector<ui_element> elements;
    bool quit = false;

    void screenDraw(GLFWwindow *window, shader &program, shader &text_program, object &sprite_object, object &sprite_text_object, float mouseX, float mouseY, bool mousePressed, bool mouseReleased, float delta_time, bool front);
    ui_element *mostRecentCreatedElement();
};

void startGame(character *p, game_system *gs, dungeon *d, int argv);
void optionsTab(character *p, game_system *gs, dungeon *d, int argv);

void quitGame(character *p, game_system *gs, dungeon *d, int argv);

void connectToIp(character *p, game_system *gs, dungeon *d, int argv);
void switchTyping(character *p, game_system *gs, dungeon *d, int argv);

#endif