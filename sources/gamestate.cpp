#include "../headers/gamestate.h"

gui gui_data;
game_state state;

const double pixel_divider = 36.0f;
const double windowAspectDivision = (static_cast<double>(window_width) / static_cast<double>(window_height));
int current_win_width = window_width, current_win_height = window_height;
double win_ratio_x = static_cast<double>(current_win_width) / static_cast<double>(window_width);
double win_ratio_y = static_cast<double>(current_win_height) / static_cast<double>(window_height);

extern bool buttonHovered;
extern bool mouseClicked;
extern bool mousePressed;
extern bool mouseReleased;
// connector host;
// bool typingInput;

// ui_element::ui_element(ui_element_type t, sprite *v, double x, double y, void func(character *, game_system *, dungeon *, int), bool bg,
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
ui_element::ui_element(ui_element_type t, const char *path, double x, double y, double w, double h, int frX, int frY,
                       void func(character *, game_system *, dungeon *, int), bool bg,
                       character *_func_p, game_system *_func_gs, dungeon *_func_d,
                       int _func_i, int *_linkValue)
    : visual(path, frX, frY, (t == UI_TEXT || t == UI_CLICKABLE_TEXT)), anim(&visual, 0, visual.framesX * visual.framesY, 1.0f)
{
    background = bg;
    utype = t;
    trueX = x;
    trueY = y;
    trueWidth = w;
    trueHeight = h;
    posX = window_width / 2 + (x * (window_width / 2));
    posY = window_height / 2 - (y * (window_height / 2));
    if (utype != UI_TEXT && utype != UI_CLICKABLE_TEXT)
    {
        visual.Put(x * windowAspectDivision, y, 0.0f);
    }
    else
    {
        visual.Put(((trueX + 1.0f) * 0.5f) * window_width, ((trueY + 1.0f) * 0.5f) * window_height, 0.0f);
    }
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
void ui_element::update(GLFWwindow *window, double mouseX, double mouseY, double delta_time)
{
    glfwGetFramebufferSize(window, &current_win_width, &current_win_height);

    // double trueMouseX = ((mouseX / current_win_width) * 2.0f - 1.0f) * windowAspectDivision;
    // double trueMouseY = (mouseY / window_height) * 2.0f - 1.0f;

    if (utype != UI_TEXT && utype != UI_CLICKABLE_TEXT)
    {
        posX = current_win_width / 2 + (trueX * (current_win_width / 2));
        posY = current_win_height / 2 - (visual.y * (current_win_height / 2));
        width = trueWidth / pixel_divider * (current_win_height / 2);
        height = trueHeight / pixel_divider * (current_win_height / 2);

        visual.Put(trueX * windowAspectDivision, trueY, 0.0f);
    }
    else
    {
        visual.Put(((trueX + 1.0f) * 0.5f) * window_width, ((trueY + 1.0f) * 0.5f) * window_height, 0.0f);
    }
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
        if (selected && !mousePressed)
        {
            function(func_p, func_gs, func_d, func_i);
            selected = false;
        }

        if (!selected && (mouseX < posX - width * 0.5f || mouseX > posX + width * 0.5f ||
                          mouseY < posY - height * 0.5f || mouseY > posY + height * 0.5f))
        {
            return;
        }
        if (mouseClicked)
        {
            selected = true;
        }

        if (selected)
        {
            double position = ((mouseX / current_win_width) * 2.0f - 1.0f);
            if (position > trueX + (visual.w / windowAspectDivision) * 0.5f)
                position = trueX + (visual.w / windowAspectDivision) * 0.5f;
            if (position < trueX - (visual.w / windowAspectDivision) * 0.5f)
                position = trueX - (visual.w / windowAspectDivision) * 0.5f;

            sliderPos = position * windowAspectDivision; // limit
            if (value != nullptr)
            {
                *value = (sliderPos - (visual.x - visual.w * 0.5f)) * 1.2f * sliderLimit;
            }
        }
        break;
    default:
        break;
    }
}
void ui_element::slider_values(double sP, int sL)
{
    sliderPos = visual.x + sP;

    if (sP > sL)
        sliderPos = visual.x + sL;

    sliderLimit = sL;

    if (value != nullptr)
    {
        sliderPos = static_cast<double>(*value) / (1.2f * static_cast<double>(sliderLimit)) + (visual.x - (visual.w * 0.5f));
    }
}

void gui::screenDraw(GLFWwindow *window, shader &program, shader &text_program, object &sprite_object, object &sprite_text_object, double mouseX, double mouseY, double delta_time, bool front)
{
    win_ratio_x = static_cast<double>(current_win_width) / static_cast<double>(window_width);
    win_ratio_y = static_cast<double>(current_win_height) / static_cast<double>(window_height);

    if (quit)
        glfwSetWindowShouldClose(window, true);

    for (int i = 0; i < elements.size(); ++i)
    {
        if (front && elements[i].background || !front && !elements[i].background)
            continue;

        elements[i].update(window, mouseX, mouseY, delta_time);
        if (elements[i].utype == UI_TEXT || elements[i].utype == UI_CLICKABLE_TEXT)
        {
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
            if (elements[i].utype == UI_SLIDER)
            {
                double holdPosition = elements[i].visual.x;
                elements[i].visual.x = elements[i].sliderPos;
                elements[i].visual.w *= 0.1f;
                elements[i].visual.Draw(program, sprite_object);
                elements[i].visual.x = holdPosition;
                elements[i].visual.w *= 10.0f;
            }
            elements[i].visual.Draw(program, sprite_object);
        }
    }
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