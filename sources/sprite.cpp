#include "../headers/sprite.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"

float quad_vertices[] = {
    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.0f, 0.0f, 0.0f};
unsigned int quad_indices[] = {
    0, 1, 2,
    0, 3, 2};
float cube_vertices[] = {
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

    -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f};

sprite::sprite()
{
}
sprite::sprite(object *_obj, const char *path, unsigned int _fx, unsigned int _fy)
{
    sprite_object = _obj;
    textureWidth = 1.0f;
    textureHeight = 1.0f;

    texture_path = path;

    framesX = 1;
    framesY = 1;
    if (_fx > 1)
        framesX = _fx;
    if (_fy > 1)
        framesY = _fy;

    textureInit();

    // bool foundMatchingTexture = false;
    // for (unsigned int i = 0; i < tPile.count; ++i)
    // {
    //     if (tPile.paths[i] == _path)
    //     {
    //         img = i;
    //         foundMatchingTexture = true;
    //         break;
    //     }
    // }

    // if (!foundMatchingTexture && !tPile.list[tPile.count].loadFromFile(_path))
    // {
    //     std::cout << "error: image " << _path << " failed to load.\n";
    //     return;
    // }
    // tPile.list[tPile.count].setRepeated(true);

    // if (!foundMatchingTexture)
    // {
    //     tPile.paths[tPile.count] = _path;
    //     img = tPile.count;

    //     ++tPile.count;
    // }

    // rect = sf::Sprite(tPile.list[img]);
}
void sprite::textureInit()
{
    // stbi_set_flip_vertically_on_load(true);
    glGenTextures(1, &sprite_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sprite_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    float borderColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(texture_path, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "\n\terror " << texture_path << " failed to load\n";
    }

    stbi_image_free(data);

    spriteW = width / framesX;
    spriteH = height / framesY;

    textureWidth = static_cast<float>(spriteW) / width;
    textureHeight = static_cast<float>(spriteH) / height;

    empty = false;
}
void sprite::Put(float _x, float _y, float _z)
{
    x = _x;
    y = _y;
    z = _z;
}
void sprite::Move(float _xdist, float _ydist, float _zdist)
{
    x += _xdist;
    y += _ydist;
    z += _zdist;
}
void sprite::Scale(float _w, float _h, float _d)
{
    w = _w;
    h = _h;
    d = _d;
}
void sprite::Rotate(float _rx, float _ry, float _rz)
{
    rx = _rx;
    ry = _ry;
    rz = _rz;
}

void sprite::SetColor(float _r, float _g, float _b, float _a)
{
    colr = _r;
    colg = _g;
    colb = _b;
    cola = _a;
}

void sprite::Draw(shader &program)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, z));
    model = glm::rotate(model, glm::radians(rx), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(ry), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rz), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(w, h, d));
    program.use();
    program.setUniformInt("tex", 0);
    program.setUniformMat4("model", model);
    program.setUniformVec2("tex_offset", textureX, textureY);
    program.setUniformVec2("tex_scale", textureWidth, textureHeight);
    program.setUniformVec4("color", colr, colg, colb, cola);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sprite_texture);
    switch (sprite_object->obj_type)
    {
    case OBJ_QUAD:
        glBindVertexArray(sprite_object->VAO);
        glBindBuffer(1, sprite_object->EBO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        break;
    case OBJ_CUBE:
        glBindVertexArray(sprite_object->VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        break;
    case OBJ_NULL:
        break;
    default:
        break;
    }
}

object::object(object_type _obj)
{
    obj_type = _obj;

    switch (obj_type)
    {
    case OBJ_QUAD:
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        break;
    case OBJ_CUBE:
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        break;
    default:
        break;
    }
}
void object::objectKill()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}