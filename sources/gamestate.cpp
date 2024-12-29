#include "../headers/gamestate.h"

gui gui_data;
game_state state;
// connector host;
// bool typingInput;

ui_element::ui_element(ui_element_type t, sprite *v, float x, float y, void func(character *, game_system *, dungeon *, int),
                       character *_func_p, game_system *_func_gs, dungeon *_func_d,
                       int _func_i, int *_linkValue)
    : visual(*v), anim(&visual, 0, visual.framesX * visual.framesY, 1.0f)
{
    utype = t;
    posX = x;
    posY = y;
    width = visual.spriteW;
    height = visual.spriteH;
    function = func;
    func_p = _func_p;
    func_gs = _func_gs;
    func_d = _func_d;
    func_i = _func_i;
    value = _linkValue;
}
// linkvalue apparently doesn nothing, get rid of it please
ui_element::ui_element(ui_element_type t, object *obj, shader *shad, const char *path, float x, float y, int frX, int frY,
                       void func(character *, game_system *, dungeon *, int),
                       character *_func_p, game_system *_func_gs, dungeon *_func_d,
                       int _func_i, int *_linkValue)
    : visual(obj, shad), anim(&visual, 0, visual.framesX * visual.framesY, 1.0f)
{
    visual.setTexture(path, frX, frY);
    utype = t;
    posX = x;
    posY = y;
    width = visual.spriteW;
    height = visual.spriteH;
    function = func;
    func_p = _func_p;
    func_gs = _func_gs;
    func_d = _func_d;
    func_i = _func_i;
    value = _linkValue;
}

void ui_element::update(float mouseX, float mouseY, bool mousePressed, bool mouseReleased, float delta_time)
{
    switch (utype)
    {
    case UI_CLICKABLE:
        if (mouseX < posX || mouseX > posX + width ||
            mouseY < posY || mouseY > posY + height)
        {
            // if (visual.rect.getColor().r < 255)
            // {
            //     visual.rect.setColor(sf::Color(255, 255, 255, 255));
            // }
            return;
        }
        buttonHovered = true;

        // if (!mousePressed)
        //     visual.rect.setColor(sf::Color(150, 150, 150, 255));
        // else
        //     visual.rect.setColor(sf::Color(70, 70, 70, 255));

        if (!mouseReleased)
            return;

        // visual.rect.setColor(sf::Color(255, 255, 255, 255));

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

void gui::screenDraw(GLFWwindow *window, float mouseX, float mouseY, bool mousePressed, bool mouseReleased, float delta_time)
{
    if (quit)
        glfwSetWindowShouldClose(window, true);

    for (int i = 0; i < elements.size(); ++i)
    {
        elements[i].update(mouseX, mouseY, mousePressed, mouseReleased, delta_time);
        if (elements[i].anim._sprite != nullptr)
        {
            elements[i].anim.run(delta_time, true);
        }
        // window->draw(elements[i].visual.rect);
        elements[i].visual.Draw();
    }
    if (bgAnim._sprite != nullptr)
    {
        bgAnim.run(delta_time, true);
    }
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