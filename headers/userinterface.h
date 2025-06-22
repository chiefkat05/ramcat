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

    constexpr bool operator==(ui_element &element)
    {
        if (trueX == element.trueX && trueY == element.trueY && trueWidth == element.trueWidth &&
            trueHeight == element.trueHeight && posX == element.posX && posY == element.posY && width == element.width &&
            height == element.height && sliderPos == element.sliderPos && sliderLimit == element.sliderLimit && sliderMin == element.sliderMin &&
            background == element.background && visual == element.visual)
            return true;
        return false;
    }
};

struct textCharacter
{
    int letterID;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    unsigned int Advance;
};
struct gui
{
    std::vector<ui_element> elements;
    bool quit = false;

    unsigned int text_texture_id;
    std::map<char, textCharacter> textCharacters;
    FT_Library font_ft;
    FT_Face font_face;
    int loadFont(const char *path);
    int letterCount = 0;
    bool textChanged = false;

    void screenDraw(game_system &game, GLFWwindow *window, camera &mainCam, double mouseX, double mouseY, double delta_time, bool front);
    void textDraw(game_system &game);
    ui_element *mostRecentCreatedElement();
    void setText(game_system &game);
};

void changeScene(character *p, game_system *gs, world *w, int argv);

void quitGame(character *p, game_system *gs, world *w, int argv);

void connectToIp(character *p, game_system *gs, world *w, int argv);
void switchTyping(character *p, game_system *gs, world *w, int argv);

#endif