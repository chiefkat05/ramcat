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

camera mainCam(CAMERA_2D_FOLLOW);
// mainCam.LockTo(&player);
// mainCam.SetBoundary(&leftWall, &rightWall);

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
    sprite floor(&spriteRect);
    sprite fren(&spriteCube);
    bg.setTexture("./test.png", bg.sprite_texture);
    fren.setTexture("./fren.png", fren.sprite_texture);
    floor.setTexture("./img/soot.png", floor.sprite_texture);

    // declare 'sprites' or game objects here
    // sprite yoursprite(&[spriteRect | spriteCube]);
    // yoursprite.setTexture([path], yoursprite.sprite_texture);
    // yoursprite.Put(10.0f, 10.0f, 0.0f);
    // yoursprite.Scale(2.0f, 2.0f, 2.0f);
    // yoursprite.Rotate(0.0f, 0.0f, 45.0f);
    // yoursprite.Draw(shader, [spriteRect | spriteCube].VAO, [spriteRect | spriteCube].EBO);

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

        mainCam.cameraPosition += mainCam.cameraVelocity;

        view = glm::lookAt(mainCam.cameraPosition, mainCam.cameraPosition + mainCam.cameraFront, mainCam.cameraUp);
        proj = glm::perspective(glm::radians(mainCam.current_fov), static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 200.0f);
        shaderProgram.setUniformMat4("projection", proj);
        shaderProgram.setUniformMat4("view", view);

        for (int i = 0; i < 5; ++i)
        {
            bg.Put(i * 10.0f, 0.0f, 0.0f);
            bg.Scale(10.0f, 10.0f, 10.0f);
            bg.Draw(shaderProgram, spriteRect.VAO, spriteRect.EBO);
        }
        fren.Put(4.0f, -1.0f, -14.0f);
        fren.Draw(shaderProgram, spriteCube.VAO, spriteCube.EBO);

        floor.Put(0.0f, -5.0f, 0.0f);
        floor.Scale(400.0f, 400.0f, 400.0f);
        floor.Rotate(90.0f, 0.0f, 0.0f);
        floor.Draw(shaderProgram, spriteRect.VAO, spriteRect.EBO);

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
    if (mainCam.firstMouse)
    {
        mainCam.lastMouseX = xpos;
        mainCam.lastMouseY = ypos;
        mainCam.firstMouse = false;
    }

    float offsetX = xpos - mainCam.lastMouseX;
    float offsetY = mainCam.lastMouseY - ypos;

    const float sensitivity = 0.1f;
    glm::vec3 direction = glm::vec3(0.0f);

    switch (mainCam.type)
    {
    case CAMERA_STATIONARY:
        break;
    case CAMERA_2D_FOLLOW:
        if (mainCam.firstMouse)
        {
            mainCam.lastMouseX = xpos;
            mainCam.lastMouseY = ypos;
            mainCam.firstMouse = false;
        }

        mainCam.lastMouseX = xpos;
        mainCam.lastMouseY = ypos;

        offsetX *= sensitivity;
        offsetY *= sensitivity;

        mainCam.cameraPosition.x += offsetX;
        mainCam.cameraPosition.y += offsetY;
        break;
    default:

        mainCam.lastMouseX = xpos;
        mainCam.lastMouseY = ypos;

        offsetX *= sensitivity;
        offsetY *= sensitivity;

        mainCam.yaw += offsetX;
        mainCam.pitch += offsetY;

        if (mainCam.pitch > 89.0f)
            mainCam.pitch = 89.0f;
        if (mainCam.pitch < -89.0f)
            mainCam.pitch = -89.0f;

        direction.x = std::cos(glm::radians(mainCam.yaw)) * std::cos(glm::radians(mainCam.pitch));
        direction.y = std::sin(glm::radians(mainCam.pitch));
        direction.z = std::sin(glm::radians(mainCam.yaw)) * std::cos(glm::radians(mainCam.pitch));
        mainCam.cameraFront = glm::normalize(direction);
        direction = glm::vec3(0.0f);
        direction.x = std::cos(glm::radians(mainCam.yaw));
        direction.z = std::sin(glm::radians(mainCam.yaw));
        mainCam.cameraLockedFront = glm::normalize(direction);

        mainCam.cameraRight = glm::cross(mainCam.cameraUp, mainCam.cameraFront);
        mainCam.cameraLockedRight = glm::cross(mainCam.cameraUp, mainCam.cameraLockedFront);
        break;
    }
}
void processInput(GLFWwindow *window)
{
    if (mainCam.type == CAMERA_3D)
    {
        mainCam.cameraVelocity.x = 0.0f;
        mainCam.cameraVelocity.z = 0.0f;

        if (mainCam.cameraPosition.y > 0.0f)
            mainCam.cameraVelocity.y -= 2.0f * delta_time;
        if (mainCam.cameraPosition.y <= 0.0f)
        {
            mainCam.cameraPosition.y = 0.0f;
            mainCam.cameraVelocity.y = 0.0f;
            mainCam.jumped = false;
        }

        if (glfwGetKey(window, GLFW_KEY_W))
        {
            mainCam.cameraVelocity += mainCam.cameraLockedFront * 4.0f * delta_time;
        }
        if (glfwGetKey(window, GLFW_KEY_S))
        {
            mainCam.cameraVelocity -= mainCam.cameraLockedFront * 4.0f * delta_time;
        }
        if (glfwGetKey(window, GLFW_KEY_A))
        {
            mainCam.cameraVelocity -= glm::normalize(glm::cross(mainCam.cameraFront, mainCam.cameraUp)) * 4.0f * delta_time;
        }
        if (glfwGetKey(window, GLFW_KEY_D))
        {
            mainCam.cameraVelocity += glm::normalize(glm::cross(mainCam.cameraFront, mainCam.cameraUp)) * 4.0f * delta_time;
        }
        if (!mainCam.jumped && glfwGetKey(window, GLFW_KEY_SPACE))
        {
            mainCam.cameraVelocity.y = 20.0f * delta_time;
            mainCam.jumped = true;
        }
    }
    if (mainCam.type == CAMERA_2D_FOLLOW)
    {
        if (glfwGetKey(window, GLFW_KEY_W))
        {
            mainCam.cameraPosition.y += 10.0f * delta_time;
        }
        if (glfwGetKey(window, GLFW_KEY_S))
        {
            mainCam.cameraPosition.y -= 10.0f * delta_time;
        }
        if (glfwGetKey(window, GLFW_KEY_A))
        {
            mainCam.cameraPosition.x -= 10.0f * delta_time;
        }
        if (glfwGetKey(window, GLFW_KEY_D))
        {
            mainCam.cameraPosition.x += 10.0f * delta_time;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_C))
    {
        mainCam.current_fov = mainCam.zoomed_fov;
    }
    if (!glfwGetKey(window, GLFW_KEY_C))
    {
        mainCam.current_fov = mainCam.fov;
    }

    if (!glfwGetKey(window, GLFW_KEY_F) && mainCam.swappedFullscreen)
        mainCam.swappedFullscreen = false;

    if (glfwGetKey(window, GLFW_KEY_F) && !mainCam.swappedFullscreen)
    {
        mainCam.fullscreen = !mainCam.fullscreen;
        if (mainCam.fullscreen)
            glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0.0f, 0.0f, window_width, window_height, GLFW_DONT_CARE);
        if (!mainCam.fullscreen)
            glfwSetWindowMonitor(window, NULL, 0.0f, 0.0f, window_width, window_height, GLFW_DONT_CARE);

        mainCam.swappedFullscreen = true;
    }

    if (!glfwGetKey(window, GLFW_KEY_TAB) && mainCam.swappedMouseCapture)
        mainCam.swappedMouseCapture = false;

    if (glfwGetKey(window, GLFW_KEY_TAB) && !mainCam.swappedMouseCapture)
    {
        mainCam.mouseCapture = !mainCam.mouseCapture;
        if (mainCam.mouseCapture)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (!mainCam.mouseCapture)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        mainCam.swappedMouseCapture = true;
    }
}