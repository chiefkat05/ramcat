#include "../headers/sprite.h"

float vertices[] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.5f, 0.5f, 0.0f,
    -0.5f, 0.5f, 0.0f};
unsigned int indices[] = {
    0, 1, 2,
    0, 3, 2};

sprite::sprite()
{
}
void sprite::Put(float _x, float _y)
{
    x = _x;
    y = _y;
}
void sprite::Move(float _xdist, float _ydist)
{
    x += _xdist;
    y += _ydist;
}

void sprite::Draw(shader &program, unsigned int VAO, unsigned int EBO)
{
    program.use();
    program.setUniformVec3("position", x, y, 0.0f);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

quad::quad()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
}
void quad::quadKill()
{

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}