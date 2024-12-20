// animation commands should be inside sprite class and have the following public functions:
//  sprite.runFrames(x, y, spd)
//  sprite.saveFrames(id, x, y, spd)
//  sprite.runFrames(id)

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../headers/sprite.h"

const unsigned int window_width = 1280;
const unsigned int window_height = 720;

float texCoords[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f};

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

float delta_time = 0.0f;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(window_width, window_height, "Rambunctious- \"Cat edition\"", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "no window :(\n";
        glfwTerminate();
        return 0;
    }
    glfwMakeContextCurrent(window);
    // glViewport(0, 0, 1980, 1080);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "GLAD FAILED TO LOAD EVERYONE PANIC\n";
        glfwTerminate();
        return 0;
    }

    shader shaderProgram("./shaders/translation.vertex", "./shaders/texture-2.fragment");

    quad spriteRect;
    sprite bg;
    bg.setTexture("./test.png", bg.sprite_texture);
    bg.setTexture("./test-2.png", bg.sprite_texture2);
    shaderProgram.use();
    shaderProgram.setUniformInt("tex", 0);
    shaderProgram.setUniformInt("tex_2", 1);
    // int left = 1;

    float current_time = 0;

    glEnable(GL_DEPTH_TEST);

    // glm::mat4 orthographic = glm::ortho(0.0f, static_cast<float>(window_width), 0.0f, static_cast<float>(window_height), 0.1f, 200.0f);

    while (!glfwWindowShouldClose(window))
    {
        float past_time = current_time;
        current_time = glfwGetTime();
        delta_time = current_time - past_time;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // player.Move(0.1f * delta_time * left, 0.0f);
        // if (player.x > 1.0f)
        //     left = -1;
        // if (player.x < -1.0f)
        //     left = 1;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            bg.Move(2.0f * delta_time, 0.0f);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            bg.Move(-2.0f * delta_time, 0.0f);
        }
        glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
        // glm::mat4 trans = glm::mat4(1.0f);
        // trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
        // trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        // trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));
        // shaderProgram.setUniformMat4("transform", trans);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 proj;
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        proj = glm::perspective(glm::radians(90.0f), static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 200.0f);
        shaderProgram.setUniformMat4("projection", proj);
        shaderProgram.setUniformMat4("view", view);
        for (int i = 0; i < 5; ++i)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(i * 1.2f - 2.0f, 0.0f, 0.0f));
            model = glm::rotate(model, static_cast<float>(glfwGetTime()) * glm::radians(50.0f) * i, glm::vec3(0.5f, 1.0f, 0.0f));
            shaderProgram.setUniformMat4("model", model);
            bg.Draw(shaderProgram, spriteRect.VAO, spriteRect.EBO);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // std::cout << "you thought you could resize lmao\n";
    glViewport(0, 0, width, height);
}
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}