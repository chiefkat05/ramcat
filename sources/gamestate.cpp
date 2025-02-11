#include "../headers/gamestate.h"

gui gui_data;
game_state state;

const float pixel_divider = 36.0f;
// connector host;
// bool typingInput;

ui_element::ui_element(ui_element_type t, sprite *v, float x, float y, void func(character *, game_system *, dungeon *, int), bool bg,
                       character *_func_p, game_system *_func_gs, dungeon *_func_d,
                       int _func_i, int *_linkValue)
    : visual(*v), anim(&visual, 0, visual.framesX * visual.framesY, 1.0f)
{
    background = bg;
    utype = t;
    posX = x;
    posY = y;
    trueX = x;
    trueY = y;
    visual.Put(x, y, 0.0f);
    width = visual.spriteW;
    height = visual.spriteH;
    function = func;
    func_p = _func_p;
    func_gs = _func_gs;
    func_d = _func_d;
    func_i = _func_i;
    value = _linkValue;
}
ui_element::ui_element(ui_element_type t, object *obj, const char *path, float x, float y, float w, float h, int frX, int frY,
                       void func(character *, game_system *, dungeon *, int), bool bg,
                       character *_func_p, game_system *_func_gs, dungeon *_func_d,
                       int _func_i, int *_linkValue)
    : visual(obj, path, frX, frY), anim(&visual, 0, visual.framesX * visual.framesY, 1.0f)
{
    background = bg;
    utype = t;
    trueX = x;
    trueY = y;
    posX = window_width / 2 + (x * (window_width / 2));
    posY = window_height / 2 - (y * (window_height / 2));
    visual.Put(x * 1.7778f, y, 0.0f); // idk if this is necessary
    width = w / pixel_divider * (window_height / 2);
    height = h / pixel_divider * (window_height / 2);
    visual.Scale(w / pixel_divider, h / pixel_divider, 1.0f); // probably should be both the same division
    function = func;
    func_p = _func_p;
    func_gs = _func_gs;
    func_d = _func_d;
    func_i = _func_i;
    value = _linkValue;
}

// get ui element animations set up
void ui_element::update(GLFWwindow *window, float mouseX, float mouseY, bool mousePressed, bool mouseReleased, float delta_time)
{
    int win_width, win_height;
    glfwGetFramebufferSize(window, &win_width, &win_height);
    posX = win_width / 2 + (trueX * (win_width / 2));
    posY = win_height / 2 - (trueY * (win_height / 2));
    visual.Put(trueX * 1.7778f, trueY, 0.0f);
    switch (utype)
    {
    case UI_CLICKABLE:
        if (mouseX < posX - width * 0.5f || mouseX > posX + width * 0.5f ||
            mouseY < posY - height * 0.5f || mouseY > posY + height * 0.5f)
        {
            if (visual.colr < 1.0f)
            {
                visual.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
            }
            return;
        }
        buttonHovered = true;

        if (!mousePressed)
            visual.SetColor(0.5f, 0.5f, 0.5f, 0.5f);
        else
            visual.SetColor(0.7f, 0.7f, 0.7f, 0.7f);

        if (!mouseReleased)
            return;

        visual.SetColor(1.0f, 1.0f, 1.0f, 1.0f);

        function(func_p, func_gs, func_d, func_i);
        break;
    case UI_VALUEISFRAME:
        if (value == nullptr)
            break;
        anim._sprite = &visual; // this should only happen once or so    // sounds important pls look into
        anim.frame = *value;
        anim.timer = 1.0f;
        anim.run(delta_time, false);
        break;
    default:
        break;
    }
}

void gui::screenDraw(GLFWwindow *window, shader &program, float mouseX, float mouseY, bool mousePressed, bool mouseReleased, float delta_time, bool front)
{
    if (quit)
        glfwSetWindowShouldClose(window, true);

    for (int i = 0; i < elements.size(); ++i)
    {
        if (front && elements[i].background || !front && !elements[i].background)
            continue;

        elements[i].update(window, mouseX, mouseY, mousePressed, mouseReleased, delta_time);
        elements[i].visual.Draw(program);
    }
    // background.Draw(program);
    // if (bgAnim._sprite != nullptr)
    // {
    //     bgAnim.run(delta_time, true);
    // }
}

void startGame(character *p, game_system *gs, dungeon *d, int argv)
{
    if (state > CHARACTER_CREATION_SCREEN)
        return;

    state = static_cast<game_state>(argv);
}
void optionsTab(character *p, game_system *gs, dungeon *d, int argv)
{
    state = MENU_SCREEN;
    // gs->game_music.stop();
    // if (!gs->game_music.openFromFile("../snd/mus/options.mp3"))
    // {
    //     std::cout << "failed to load audio file ../snd/mus/options.mp3\n";
    //     return;
    // }
    // gs->game_music.play();
}
void nullFunc(character *p, game_system *gs, dungeon *d, int argv) {}

void quitGame(character *p, game_system *gs, dungeon *d, int argv)
{
    gui_data.quit = true;
}

// void connectToIp(character *p, game_system *gs, dungeon *d, int argv)
// {
//     host.host_ip = "127.0.0.1";
//     host.port = 4444;
//     host.bind(4444);
// }

// void switchTyping(character *p, game_system *gs, dungeon *d, int argv)
// {
//     typingInput = !typingInput;
// }