#include "../headers/userinterface.h"

gui gui_data;
game_state state;

const double pixel_divider = 36.0;
double windowAspectDivision = (static_cast<double>(window_width) / static_cast<double>(window_height));
int current_win_width = window_width, current_win_height = window_height;
double win_ratio_x = static_cast<double>(current_win_width) / static_cast<double>(window_width);
double win_ratio_y = static_cast<double>(current_win_height) / static_cast<double>(window_height);

extern bool buttonHovered;
extern bool mouseClicked;
extern bool mousePressed;
extern bool mouseReleased;
// connector host;
// bool typingInput;

// ui_element::ui_element(ui_element_type t, sprite *v, double x, double y, void func(character *, game_system *, world *, int), bool bg,
//                        character *_func_p, game_system *_func_gs, world *_func_d,
//                        int _func_i, int *_linkValue)
//     : visual(*v), anim(&visual, 0, visual.framesX * visual.framesY, 1.0)
// {
//     background = bg;
//     utype = t;
//     posX = x;
//     posY = y;
//     trueX = x;
//     trueY = y;
//     visual.Put(x, y, 0.0);
//     width = visual.spriteW;
//     height = visual.spriteH;
//     function = func;
//     func_p = _func_p;
//     func_gs = _func_gs;
//     func_d = _func_d;
//     func_i = _func_i;
//     value = _linkValue;
// }
ui_element::ui_element(game_system *game, ui_element_type t, const char *path, double x, double y, int frX, int frY,
                       void func(character *, game_system *, world *, int), bool bg,
                       character *_func_p, world *_func_d,
                       int _func_i, int *_linkValue)
    : visual(game->shaders[GAME_SHADER_DEFAULT], game->objects[GAME_OBJECT_DEFAULT],
             path, frX, frY, (t == UI_TEXT || t == UI_CLICKABLE_TEXT)),
      anim(&visual, 0, visual.framesX * visual.framesY, 1.0)
{
    background = bg;
    if (bg)
    {
        visual.z = -10.0;
    }
    else
    {
        visual.z = 2.0;
    }
    utype = t;
    trueX = x;
    trueY = y;
    trueWidth = visual.spriteW;
    trueHeight = visual.spriteH;
    posX = window_width / 2 + (x * (window_width / 2));
    posY = window_height / 2 - (y * (window_height / 2));
    if (utype != UI_TEXT && utype != UI_CLICKABLE_TEXT)
    {
        visual.Put(x * windowAspectDivision, y, visual.z);
    }
    else
    {
        visual.Put(((trueX + 1.0) * 0.5f) * window_width, ((trueY + 1.0) * 0.5f) * window_height, visual.z);
        visual.shaderP = game->shaders[GAME_SHADER_TEXT];
        visual.objectP = game->objects[GAME_OBJECT_TEXT];
    }
    width = trueWidth / pixel_divider * (window_height / 2);
    height = trueHeight / pixel_divider * (window_height / 2);
    visual.Scale(trueWidth / pixel_divider, trueHeight / pixel_divider, 1.0);
    function = func;
    func_p = _func_p;
    func_gs = game;
    func_d = _func_d;
    func_i = _func_i;
    value = _linkValue;
}

void ui_element::scale(double w, double h)
{
    trueWidth = w;
    trueHeight = h;
    width = trueWidth / pixel_divider * (window_height / 2);
    height = trueHeight / pixel_divider * (window_height / 2);
    visual.Scale(trueWidth / pixel_divider, trueHeight / pixel_divider, 1.0);
}
void ui_element::setScreenMode(ui_screen_mode new_screen_mode)
{
    screen_mode = new_screen_mode;
}

// get ui element animations set up
void ui_element::update(GLFWwindow *window, double mouseX, double mouseY, camera &mainCam, double delta_time)
{
    glfwGetFramebufferSize(window, &current_win_width, &current_win_height);

    // double trueMouseX = ((mouseX / current_win_width) * 2.0 - 1.0) * windowAspectDivision;
    // double trueMouseY = (mouseY / window_height) * 2.0 - 1.0;

    if (utype != UI_TEXT && utype != UI_CLICKABLE_TEXT)
    {
        posX = current_win_width / 2 + (trueX * (current_win_width / 2));
        posY = current_win_height / 2 - (visual.y * (current_win_height / 2));
        width = trueWidth / pixel_divider * (current_win_height / 2);
        height = trueHeight / pixel_divider * (current_win_height / 2);

        visual.Put(mainCam.cameraPosition.x + trueX * windowAspectDivision, mainCam.cameraPosition.y + trueY, visual.z);
    }
    else
    {
        visual.Put(mainCam.cameraPosition.x + ((trueX + 1.0) * 0.5f) * window_width, mainCam.cameraPosition.y + ((trueY + 1.0) * 0.5f) * window_height, visual.z);
    }
    switch (utype)
    {
    case UI_CLICKABLE:
        if (mouseX < posX || mouseX > posX + width ||
            mouseY < posY - height || mouseY > posY)
        {
            if (visual.colr < 1.0)
            {
                visual.SetColor(1.0, 1.0, 1.0, 1.0);
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

        visual.SetColor(1.0, 1.0, 1.0, 1.0);

        function(func_p, func_gs, func_d, func_i);
        break;
    case UI_VALUEISFRAME:
        if (value == nullptr)
            break;
        anim._sprite = &visual; // this should only happen once or so    // sounds important pls look into
        anim.frame = *value;
        anim.timer = 1.0;
        anim.run(delta_time, false);
        break;
    case UI_TEXT:
        break;
    case UI_CLICKABLE_TEXT:
        if (mouseX < posX || mouseX > width ||
            mouseY > posY || mouseY < height)
        {
            if (visual.colr < 1.0)
            {
                visual.SetColor(1.0, 1.0, 1.0, 1.0);
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

        visual.SetColor(1.0, 1.0, 1.0, 1.0);

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
            double position = ((mouseX / current_win_width) * 2.0 - 1.0);
            if (position > trueX + (visual.w / windowAspectDivision) * 0.5)
                position = trueX + (visual.w / windowAspectDivision) * 0.5;
            if (position < trueX - (visual.w / windowAspectDivision) * 0.5)
                position = trueX - (visual.w / windowAspectDivision) * 0.5;

            sliderPos = position * windowAspectDivision; // limit
            if (value != nullptr)
            {
                *value = sliderMin + ((sliderPos - (visual.x - visual.w * 0.5)) * 1.2 * sliderLimit);
            }
        }
        break;
    default:
        break;
    }
}

// v = m + (p - l) * 1.2 * s
// v / 1.2s - m/1.2s + l = p
// p = v / 1.2s - m/1.2s + l
// sliderPos = *value / 1.2*sliderLimit - sliderMin / 1.2*sliderLimit + (visual.x - visual.w * 0.5)

void ui_element::slider_values(int sM, int sL)
{
    sliderMin = sM;
    sliderLimit = sL - sM;

    if (value != nullptr)
    {
        sliderPos = static_cast<double>(*value) / (1.2 * static_cast<double>(sliderLimit)) + (visual.x - (visual.w * 0.5f)) -
                    (static_cast<double>(sliderMin) / (1.2 * static_cast<double>(sliderLimit))); // do this math please :)
    }
}

int gui::loadFont(const char *path)
{
    if (FT_Init_FreeType(&font_ft))
    {
        std::cout << "\n\tError: Freetype not initiated properly\n";
        return -1;
    }
    if (FT_New_Face(font_ft, path, 0, &font_face))
    {
        std::cout << "\n\tError: Freetype failed to load font\n";
        return -1;
    }
    FT_Set_Pixel_Sizes(font_face, 256, 256);

    if (FT_Load_Char(font_face, 'X', FT_LOAD_RENDER))
    {
        std::cout << "\n\tError: Freetype failed to load glyph\n";
        return -1;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &text_texture_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, text_texture_id);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_R8, 256, 256, 128, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

    for (unsigned char c; c < 128; ++c)
    {
        if (FT_Load_Char(font_face, c, FT_LOAD_RENDER))
        {
            std::cout << "/n/tError: Fretype failed to load glyph\n";
            continue;
        }

        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, (int)c, font_face->glyph->bitmap.width,
                        font_face->glyph->bitmap.rows, 1, GL_RED, GL_UNSIGNED_BYTE, font_face->glyph->bitmap.buffer);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        textCharacter textChar = {
            (int)c,
            glm::ivec2(font_face->glyph->bitmap.width, font_face->glyph->bitmap.rows),
            glm::ivec2(font_face->glyph->bitmap_left, font_face->glyph->bitmap_top),
            static_cast<unsigned int>(font_face->glyph->advance.x)};
        textCharacters.insert(std::pair<char, textCharacter>(c, textChar));
    }
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    FT_Done_Face(font_face);
    FT_Done_FreeType(font_ft);

    return 0;
}
void gui::setText(game_system &game)
{
    letterCount = 0;
    for (int i = 0; i < elements.size(); ++i)
    {
        if (elements[i].utype != UI_TEXT && elements[i].utype != UI_CLICKABLE_TEXT)
            continue;

        for (int j = 0; j < elements[i].visual.texture_path.size(); ++j)
        {
            if (elements[i].visual.texture_path[j] == ' ' || elements[i].visual.texture_path[j] == '\n')
                continue;

            ++letterCount;
        }
    }
    glm::mat4 transforms[letterCount];
    int textureIDs[letterCount];
    letterCount = 0;

    std::string::const_iterator c;
    for (int i = 0; i < elements.size(); ++i)
    {
        if (elements[i].utype != UI_TEXT && elements[i].utype != UI_CLICKABLE_TEXT)
            continue;

        elements[i].visual.w = 0.2;
        std::cout << i << ", huh " << elements[i].visual.w << " wow\n"; // here size is broken

        double x = elements[i].visual.x;
        double y = elements[i].visual.y;
        double newlineX = x;

        for (c = elements[i].visual.texture_path.begin(); c != elements[i].visual.texture_path.end(); c++)
        {
            game.shaders[GAME_SHADER_TEXT]->use();
            textCharacter ch = textCharacters[*c];

            if (*c == '\n')
            {
                y -= ((ch.Size.y)) * 1.3 * elements[i].visual.w;
                x = newlineX;
                continue;
            }
            if (*c == ' ')
            {
                x += (ch.Advance >> 6) * elements[i].visual.w;
                continue;
            }

            double xpos = x + ch.Bearing.x * elements[i].visual.w;
            double ypos = y - (256.0 - ch.Bearing.y) * elements[i].visual.w;
            double w = 256.0 * elements[i].visual.w;
            double h = 256.0 * elements[i].visual.w;

            transforms[letterCount] = glm::translate(glm::mat4(1.0), glm::vec3(xpos, ypos, 0.0)) * glm::scale(glm::mat4(1.0), glm::vec3(w, h, 0.0)); // make z pos equal to gui zpos
            textureIDs[letterCount] = ch.letterID;

            unsigned int testAdvance = ch.Advance;

            x += (ch.Advance >> 6) * elements[i].visual.w;

            // if (h > returnVec.w)
            //     returnVec.w = h;

            ++letterCount;
        }
    }
    game.objects[GAME_OBJECT_TEXT]->setInstances(letterCount, transforms, nullptr, nullptr, textureIDs);
}

void gui::screenDraw(game_system &game, GLFWwindow *window, camera &mainCam, double mouseX, double mouseY, double delta_time, bool front)
{
    win_ratio_x = static_cast<double>(current_win_width) / static_cast<double>(window_width);
    win_ratio_y = static_cast<double>(current_win_height) / static_cast<double>(window_height);

    if (quit)
        glfwSetWindowShouldClose(window, true);

    // int text_letter_count = 0;
    for (int i = 0; i < elements.size(); ++i)
    {
        if (front && elements[i].background || !front && !elements[i].background)
            continue;

        elements[i].update(window, mouseX, mouseY, mainCam, delta_time);
        if (elements[i].utype == UI_TEXT || elements[i].utype == UI_CLICKABLE_TEXT)
        {
            // for (int j = 0; j < elements[i].visual.texture_path.size(); ++j)
            // {
            //     if (elements[i].visual.texture_path[j] == ' ' || elements[i].visual.texture_path[j] == '\n')
            //         continue;

            //     ++text_letter_count;
            // }
            // glm::vec4 boundingbox = renderText(*game.objects[GAME_OBJECT_TEXT], *game.shaders[GAME_SHADER_TEXT], elements[i].visual.texture_path,
            //                                    elements[i].visual.x, elements[i].visual.y, elements[i].visual.w,
            //                                    glm::vec4(elements[i].visual.colr, elements[i].visual.colg, elements[i].visual.colb, elements[i].visual.cola));
            // glm::vec4 boundingbox = renderText(*game.objects[GAME_OBJECT_TEXT], *game.shaders[GAME_SHADER_TEXT], elements[i].visual.texture_path,
            //                                    elements[i].visual.x, elements[i].visual.y, 2.0,
            //                                    glm::vec4(elements[i].visual.colr, elements[i].visual.colg, elements[i].visual.colb, elements[i].visual.cola));

            // boundingbox.y = window_height - boundingbox.y;
            // boundingbox.w = window_height - boundingbox.w;

            // elements[i].posX = boundingbox.x * win_ratio_x;
            // elements[i].posY = boundingbox.y * win_ratio_y;
            // elements[i].width = boundingbox.z * win_ratio_x;
            // elements[i].height = boundingbox.w * win_ratio_y;
            // glActiveTexture(GL_TEXTURE0);
            // glBindTexture(GL_TEXTURE_2D_ARRAY, elements[i].visual.sprite_texture);
            // glBindVertexArray(game.objects[GAME_OBJECT_TEXT]->VAO);

            // glBindBuffer(GL_ARRAY_BUFFER, game.objects[GAME_OBJECT_TEXT]->VBO);
            // glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, );
        }
        else
        {
            if (elements[i].utype == UI_SLIDER)
            {
                double holdPosition = elements[i].visual.x;
                elements[i].visual.x = elements[i].sliderPos;
                elements[i].visual.w *= 0.1f;
                elements[i].visual.Draw();
                elements[i].visual.x = holdPosition;
                elements[i].visual.w *= 10.0;
            }
            elements[i].visual.Draw();
        }
    }

    game.shaders[GAME_SHADER_TEXT]->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, text_texture_id);
    glBindVertexArray(game.objects[GAME_OBJECT_TEXT]->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, game.objects[GAME_OBJECT_TEXT]->VBO);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, letterCount);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    game.shaders[GAME_SHADER_DEFAULT]->use();
}
ui_element *gui::mostRecentCreatedElement()
{
    return &elements[elements.size() - 1];
}

void changeScene(character *p, game_system *gs, world *w, int argv)
{
    gs->nextState = static_cast<game_state>(argv);
}
void nullFunc(character *p, game_system *gs, world *w, int argv) {}

void quitGame(character *p, game_system *gs, world *w, int argv)
{
    gui_data.quit = true;
}

// void connectToIp(character *p, game_system *gs, world *w, int argv)
// {
//     host.host_ip = "127.0.0.1";
//     host.port = 4444;
//     host.bind(4444);
// }

// void switchTyping(character *p, game_system *gs, world *w, int argv)
// {
//     typingInput = !typingInput;
// }