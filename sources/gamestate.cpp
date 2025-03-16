#include "../headers/gamestate.h"

gui gui_data;
game_state state;

const float pixel_divider = 36.0f;
// connector host;
// bool typingInput;

// ui_element::ui_element(ui_element_type t, sprite *v, float x, float y, void func(character *, game_system *, dungeon *, int), bool bg,
//                        character *_func_p, game_system *_func_gs, dungeon *_func_d,
//                        int _func_i, int *_linkValue)
//     : visual(*v), anim(&visual, 0, visual.framesX * visual.framesY, 1.0f)
// {
//     background = bg;
//     utype = t;
//     posX = x;
//     posY = y;
//     trueX = x;
//     trueY = y;
//     visual.Put(x, y, 0.0f);
//     width = visual.spriteW;
//     height = visual.spriteH;
//     function = func;
//     func_p = _func_p;
//     func_gs = _func_gs;
//     func_d = _func_d;
//     func_i = _func_i;
//     value = _linkValue;
// }
ui_element::ui_element(ui_element_type t, const char *path, float x, float y, float w, float h, int frX, int frY,
                       void func(character *, game_system *, dungeon *, int), bool bg,
                       character *_func_p, game_system *_func_gs, dungeon *_func_d,
                       int _func_i, int *_linkValue)
    : visual(path, frX, frY, (t == UI_TEXT || t == UI_CLICKABLE_TEXT)), anim(&visual, 0, visual.framesX * visual.framesY, 1.0f)
{
    background = bg;
    utype = t;
    trueX = x;
    trueY = y;
    posX = window_width / 2 + (x * (window_width / 2));
    posY = window_height / 2 - (y * (window_height / 2));
    visual.Put(x * (static_cast<float>(window_width) / static_cast<float>(window_height)), y, 0.0f);
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
int current_win_width, current_win_height;
void ui_element::update(GLFWwindow *window, float mouseX, float mouseY, bool mousePressed, bool mouseReleased, float delta_time)
{
    glfwGetFramebufferSize(window, &current_win_width, &current_win_height);
    if (utype != UI_TEXT && utype != UI_CLICKABLE_TEXT)
    {
        posX = current_win_width / 2 + (trueX * (current_win_width / 2));
        posY = current_win_height / 2 - (trueY * (current_win_height / 2));
    }
    visual.Put(trueX * (static_cast<float>(window_width) / static_cast<float>(window_height)), trueY, 0.0f);
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
        // std::cout << *value << " is value\n";
        // visual.textureX = *value % visual.framesX;
        // visual.textureY = *value / visual.framesX;
        break;
    case UI_TEXT:
        break;
    case UI_CLICKABLE_TEXT:
        if (mouseX < posX || mouseX > width ||
            mouseY > posY || mouseY < height)
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
    case UI_SLIDER:
        if (mouseX < posX - width * 0.5f || mouseX > posX + width * 0.5f ||
            mouseY < posY - height * 0.5f || mouseY > posY + height * 0.5f)
        {
            return;
        }

        if (mousePressed)
        {
            // another valuefor slidervalue
            sliderPos = ((mouseX - (posX - width * 0.5f)) / 300.0f); // limit // figure out what's special about 300 - is it simply width * 3?
            std::cout << (mouseX - (posX - width * 0.5f)) << ", " << width << ", " << sliderPos << " hmm\n";
            // *value = sliderPos * sliderLimit;
        }
        break;
    default:
        break;
    }
}

void gui::screenDraw(GLFWwindow *window, shader &program, shader &text_program, object &sprite_object, object &sprite_text_object, float mouseX, float mouseY, bool mousePressed, bool mouseReleased, float delta_time, bool front)
{
    float win_ratio_x = static_cast<float>(current_win_width) / static_cast<float>(window_width);
    float win_ratio_y = static_cast<float>(current_win_height) / static_cast<float>(window_height);

    if (quit)
        glfwSetWindowShouldClose(window, true);

    for (int i = 0; i < elements.size(); ++i)
    {
        if (front && elements[i].background || !front && !elements[i].background)
            continue;

        elements[i].update(window, mouseX, mouseY, mousePressed, mouseReleased, delta_time);
        if (elements[i].utype == UI_TEXT || elements[i].utype == UI_CLICKABLE_TEXT)
        {
            // elements[i].visual.Draw(text_program, sprite_text_object);
            glm::vec4 boundingbox = renderText(sprite_text_object, text_program, elements[i].visual.texture_path,
                                               elements[i].visual.x, elements[i].visual.y, elements[i].visual.w,
                                               glm::vec4(elements[i].visual.colr, elements[i].visual.colg, elements[i].visual.colb, elements[i].visual.cola));

            boundingbox.y = window_height - boundingbox.y;
            boundingbox.w = window_height - boundingbox.w;

            elements[i].posX = boundingbox.x * win_ratio_x;
            elements[i].posY = boundingbox.y * win_ratio_y;
            elements[i].width = boundingbox.z * win_ratio_x;
            elements[i].height = boundingbox.w * win_ratio_y;
        }
        else
        {
            elements[i].visual.Draw(program, sprite_object);
            if (elements[i].utype == UI_SLIDER)
            {
                float holdPosition = elements[i].visual.x;
                elements[i].visual.x = elements[i].sliderPos;
                elements[i].visual.w *= 0.1f;
                elements[i].visual.Draw(program, sprite_object);
                elements[i].visual.x = holdPosition;
                elements[i].visual.w *= 10.0f;
            }
        }
    }

    // background.Draw(program);
    // if (bgAnim._sprite != nullptr)
    // {
    //     bgAnim.run(delta_time, true);
    // }
}
ui_element *gui::mostRecentCreatedElement()
{
    return &elements[elements.size() - 1];
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