#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "sprite.h"
#include "network.h"
#include "system.h"
#include <vector>

extern bool buttonHovered;

enum ui_element_type
{
    UI_CLICKABLE,
    UI_VALUEISFRAME,
    UI_IMAGE
};
struct ui_element
{
    sprite visual;
    float posX, posY, width, height;
    int *value;

    void (*function)(character *, game_system *, dungeon *, int);
    character *func_p;
    game_system *func_gs;
    dungeon *func_d;
    int func_i;

    ui_element_type utype;
    animation anim;

    ui_element(ui_element_type t, sprite *v, float x, float y, void func(character *, game_system *, dungeon *, int),
               character *_func_p = nullptr, game_system *_func_gs = nullptr, dungeon *_func_d = nullptr,
               int _func_i = 0, int *_linkValue = nullptr);
    ui_element(ui_element_type t, const char *path, float x, float y, int frX, int frY, void func(character *, game_system *, dungeon *, int),
               character *_func_p = nullptr, game_system *_func_gs = nullptr, dungeon *_func_d = nullptr,
               int _func_i = 0, int *_linkValue = nullptr);

    void update(float mouseX, float mouseY, bool mousePressed, bool mouseReleased, float delta_time);
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
    sprite background;
    animation bgAnim;
    bool quit = false;

    void screenDraw(sf::RenderWindow *window, float mouseX, float mouseY, bool mousePressed, bool mouseReleased, float delta_time);
};

void nullFunc(character *p, game_system *gs, dungeon *d, int argv);

void startGame(character *p, game_system *gs, dungeon *d, int argv);
void optionsTab(character *p, game_system *gs, dungeon *d, int argv);

void quitGame(character *p, game_system *gs, dungeon *d, int argv);

void connectToIp(character *p, game_system *gs, dungeon *d, int argv);
void switchTyping(character *p, game_system *gs, dungeon *d, int argv);

#endif