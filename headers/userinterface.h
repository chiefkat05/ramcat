#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

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

enum ui_screen_mode
{
    UI_LOCKED,
    UI_BOUND,
    UI_UNLOCKED
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
    ui_screen_mode screen_mode = UI_LOCKED;

    ui_element_type utype;
    animation anim;

    ui_element(game_system *game, ui_element_type t, const char *path, double x, double y, int frX, int frY,
               void func(character *, game_system *, world *, int) = nullFunc, bool bg = false,
               character *_func_p = nullptr, world *_func_d = nullptr,
               int _func_i = 0, int *_linkValue = nullptr);
    void slider_values(int sM, int sL);
    void scale(double w, double h);
    void setScreenMode(ui_screen_mode new_screen_mode);

    void update(GLFWwindow *window, double mouseX, double mouseY, camera &mainCam, double delta_time);
};

struct transition
{
    double destination = 0.0, speed = 0.0;
    double *valueD = nullptr;
    float *valueF = nullptr;
    int *valueI = nullptr;

    void (*function)(character *, game_system *, world *, int);
    character *func_p;
    game_system *func_gs;
    world *func_d;
    int func_i;

    bool ready = true, finished = false;

    transition(double spd, double destination, double *v, void func(character *, game_system *, world *, int) = nullFunc,
               character *_func_p = nullptr, game_system *_func_gs = nullptr, world *_func_d = nullptr,
               int _func_i = 0, bool r = true)
    {
        speed = spd;
        function = func;
        ready = r;
        finished = false;
        valueD = v;
    }
    transition(double spd, float destination, float *v, void func(character *, game_system *, world *, int) = nullFunc,
               character *_func_p = nullptr, game_system *_func_gs = nullptr, world *_func_d = nullptr,
               int _func_i = 0, bool r = true)
    {
        speed = spd;
        function = func;
        ready = r;
        finished = false;
        valueF = v;
    }
    transition(double spd, int destination, int *v, void func(character *, game_system *, world *, int) = nullFunc,
               character *_func_p = nullptr, game_system *_func_gs = nullptr, world *_func_d = nullptr,
               int _func_i = 0, bool r = true)
    {
        speed = spd;
        function = func;
        ready = r;
        finished = false;
        valueI = v;
    }
    transition()
    {
        speed = 0.0;
        function = nullptr;
        ready = false;
        finished = true;
    }

    void trigger()
    {
        ready = true;
    }
    void runD(float delta_time)
    {
        if (*valueD < destination)
        {
            *valueD += speed * delta_time;
            if (*valueD >= destination)
            {
                *valueD = destination;
                function(func_p, func_gs, func_d, func_i);
                finished = true;
            }
        }
        if (*valueD > destination)
        {
            *valueD -= speed * delta_time;
            if (*valueD <= destination)
            {
                *valueD = destination;
                function(func_p, func_gs, func_d, func_i);
                finished = true;
            }
        }
        if (*valueD == destination)
        {
            function(func_p, func_gs, func_d, func_i);
            finished = true;
        }
    }
    void runF(float delta_time)
    {
        if (*valueF < destination)
        {
            *valueF += speed * delta_time;
            if (*valueF >= destination)
            {
                *valueF = destination;
                function(func_p, func_gs, func_d, func_i);
                finished = true;
            }
        }
        if (*valueF > destination)
        {
            *valueF -= speed * delta_time;
            if (*valueF <= destination)
            {
                *valueF = destination;
                function(func_p, func_gs, func_d, func_i);
                finished = true;
            }
        }
        if (*valueF == destination)
        {
            function(func_p, func_gs, func_d, func_i);
            finished = true;
        }
    }
    void runI(float delta_time)
    {
        if (*valueI < destination)
        {
            *valueI += speed * delta_time;
            if (*valueI >= destination)
            {
                *valueI = destination;
                function(func_p, func_gs, func_d, func_i);
                finished = true;
            }
        }
        if (*valueI > destination)
        {
            *valueI -= speed * delta_time;
            if (*valueI <= destination)
            {
                *valueI = destination;
                function(func_p, func_gs, func_d, func_i);
                finished = true;
            }
        }
        if (*valueI == destination)
        {
            function(func_p, func_gs, func_d, func_i);
            finished = true;
        }
    }
    void run(float delta_time)
    {
        if (!ready || finished)
            return;

        if (valueD != nullptr)
        {
            runD(delta_time);
        }
        if (valueF != nullptr)
        {
            runF(delta_time);
        }
        if (valueI != nullptr)
        {
            runI(delta_time);
        }
    }
};

struct gui
{
    std::vector<ui_element> elements;
    bool quit = false;

    void screenDraw(game_system &game, GLFWwindow *window, camera &mainCam, double mouseX, double mouseY, double delta_time, bool front);
    ui_element *mostRecentCreatedElement();
};

void changeScene(character *p, game_system *gs, world *w, int argv);

void quitGame(character *p, game_system *gs, world *w, int argv);

void connectToIp(character *p, game_system *gs, world *w, int argv);
void switchTyping(character *p, game_system *gs, world *w, int argv);

#endif