#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "sprite.h"
// #include "network.h"
#include "system.h"
#include <vector>

enum ui_element_type
{
    UI_CLICKABLE,
    UI_VALUEISFRAME,
    UI_IMAGE,
    UI_TEXT,
    UI_CLICKABLE_TEXT,
    UI_SLIDER,
    UI_DROPDOWN,
    UI_TEXT_INPUT
};

void nullFunc(character *p, game_system *gs, world *w, int argv);

struct ui_element
{
    sprite visual;
    double trueX, trueY, trueWidth, trueHeight;
    double posX, posY, width, height, sliderPos = 0;
    int sliderLimit = 1, sliderMin = 0;
    int *value;
    bool selected = false;
    glm::vec4 color;

    void (*function)(character *, game_system *, world *, int);
    character *func_p;
    game_system *func_gs;
    world *func_d;
    int func_i;
    bool background = false;

    ui_element_type utype;
    animation anim;

    ui_element(ui_element_type t, const char *path, double x, double y, double w, double h, int frX, int frY,
               void func(character *, game_system *, world *, int) = nullFunc, bool bg = false,
               character *_func_p = nullptr, game_system *_func_gs = nullptr, world *_func_d = nullptr,
               int _func_i = 0, int *_linkValue = nullptr);
    void slider_values(int sM, int sL);

    void update(GLFWwindow *window, double mouseX, double mouseY, double delta_time);
};

enum game_state
{
    START_SCREEN,
    MENU_SCREEN,
    CHARACTER_CREATION_SCREEN,
    WORLD_SCREEN,
    WON_LEVEL_STATE,
    LOSE_SCREEN,
    WIN_SCREEN,
    state_total_count
};

struct gui
{
    std::vector<ui_element> elements;
    bool quit = false;

    void screenDraw(GLFWwindow *window, shader &program, shader &text_program, object &sprite_object, object &sprite_text_object, double mouseX, double mouseY, double delta_time, bool front);
    ui_element *mostRecentCreatedElement();
};

void startGame(character *p, game_system *gs, world *w, int argv);
void optionsTab(character *p, game_system *gs, world *w, int argv);

void quitGame(character *p, game_system *gs, world *w, int argv);

void connectToIp(character *p, game_system *gs, world *w, int argv);
void switchTyping(character *p, game_system *gs, world *w, int argv);

#endif