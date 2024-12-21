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
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void processInput(GLFWwindow *window);

float delta_time = 0.0f;

glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraLockedFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraDirection = glm::normalize(cameraTarget - cameraPosition);
glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::normalize(glm::cross(worldUp, cameraDirection));
glm::vec3 cameraLockedRight = glm::vec3(0.0f);
glm::vec3 cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));
float pitch = 0.0f, yaw = 0.0f;
float lastMouseX = window_width * 0.5f;
float lastMouseY = window_height * 0.5f;
bool firstMouse = true;
bool fullscreen = false, swappedFullscreen = false;
bool mouseCapture = true, swappedMouseCapture = false;
glm::vec3 cameraVelocity = glm::vec3(0.0f);
bool jumped = false;
float fov = 90.0f, zoomed_fov = 20.0f, current_fov = 90.0f;

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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "GLAD FAILED TO LOAD EVERYONE PANIC\n";
        glfwTerminate();
        return 0;
    }

    shader shaderProgram("./shaders/default.vertex", "./shaders/default.fragment");

    object spriteRect(OBJ_QUAD);
    object spriteCube(OBJ_CUBE);
    sprite bg(&spriteRect);
    sprite fren(&spriteCube);
    bg.setTexture("./test.png", bg.sprite_texture);
    fren.setTexture("./fren.png", fren.sprite_texture);
    shaderProgram.use();
    shaderProgram.setUniformInt("tex", 0);

    float current_time = 0;

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        float past_time = current_time;
        current_time = glfwGetTime();
        delta_time = current_time - past_time;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 proj;

        cameraPosition += cameraVelocity;

        view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
        proj = glm::perspective(glm::radians(current_fov), static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 200.0f);
        shaderProgram.setUniformMat4("projection", proj);
        shaderProgram.setUniformMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        for (int i = 0; i < 5; ++i)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(i * 1.2f - 2.0f, 0.0f, 0.0f));
            model = glm::rotate(model, static_cast<float>(glfwGetTime()) * glm::radians(50.0f) * i, glm::vec3(0.5f, 1.0f, 0.0f));
            shaderProgram.setUniformMat4("model", model);
            bg.Draw(shaderProgram, spriteRect.VAO, spriteRect.EBO);
        }
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(4.0f, 0.0f, 4.0f));
        shaderProgram.setUniformMat4("model", model);
        fren.Draw(shaderProgram, spriteCube.VAO, spriteCube.EBO);

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
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastMouseX = xpos;
        lastMouseY = ypos;
        firstMouse = false;
    }

    float offsetX = xpos - lastMouseX;
    float offsetY = lastMouseY - ypos;

    lastMouseX = xpos;
    lastMouseY = ypos;

    const float sensitivity = 0.1f;
    offsetX *= sensitivity;
    offsetY *= sensitivity;

    yaw += offsetX;
    pitch += offsetY;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction = glm::vec3(0.0f);
    direction.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    direction.y = std::sin(glm::radians(pitch));
    direction.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
    direction = glm::vec3(0.0f);
    direction.x = std::cos(glm::radians(yaw));
    direction.z = std::sin(glm::radians(yaw));
    cameraLockedFront = glm::normalize(direction);

    cameraRight = glm::cross(cameraUp, cameraFront);
    cameraLockedRight = glm::cross(cameraUp, cameraLockedFront);
}
void processInput(GLFWwindow *window)
{
    cameraVelocity.x = 0.0f;
    cameraVelocity.z = 0.0f;

    if (cameraPosition.y > 0.0f)
        cameraVelocity.y -= 2.0f * delta_time;
    if (cameraPosition.y <= 0.0f)
    {
        cameraPosition.y = 0.0f;
        cameraVelocity.y = 0.0f;
        jumped = false;
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W))
    {
        cameraVelocity += cameraLockedFront * 4.0f * delta_time;
    }
    if (glfwGetKey(window, GLFW_KEY_S))
    {
        cameraVelocity -= cameraLockedFront * 4.0f * delta_time;
    }
    if (glfwGetKey(window, GLFW_KEY_A))
    {
        cameraVelocity -= glm::normalize(glm::cross(cameraFront, cameraUp)) * 4.0f * delta_time;
    }
    if (glfwGetKey(window, GLFW_KEY_D))
    {
        cameraVelocity += glm::normalize(glm::cross(cameraFront, cameraUp)) * 4.0f * delta_time;
    }
    if (!jumped && glfwGetKey(window, GLFW_KEY_SPACE))
    {
        cameraVelocity.y = 0.4f;
        jumped = true;
    }

    if (glfwGetKey(window, GLFW_KEY_C))
    {
        current_fov = zoomed_fov;
    }
    if (!glfwGetKey(window, GLFW_KEY_C))
    {
        current_fov = fov;
    }

    if (!glfwGetKey(window, GLFW_KEY_F) && swappedFullscreen)
        swappedFullscreen = false;

    if (glfwGetKey(window, GLFW_KEY_F) && !swappedFullscreen)
    {
        fullscreen = !fullscreen;
        if (fullscreen)
            glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0.0f, 0.0f, window_width, window_height, GLFW_DONT_CARE);
        if (!fullscreen)
            glfwSetWindowMonitor(window, NULL, 0.0f, 0.0f, window_width, window_height, GLFW_DONT_CARE);

        swappedFullscreen = true;
    }

    if (!glfwGetKey(window, GLFW_KEY_TAB) && swappedMouseCapture)
        swappedMouseCapture = false;

    if (glfwGetKey(window, GLFW_KEY_TAB) && !swappedMouseCapture)
    {
        mouseCapture = !mouseCapture;
        if (mouseCapture)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (!mouseCapture)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        swappedMouseCapture = true;
    }
}