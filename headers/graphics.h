#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>

std::string readFileToString(std::string path);

class shader
{
public:
    unsigned int ID;

    void use();
    shader(const char *vPath, const char *fPath);

    void setUniformVec4(const char *name, float x, float y, float z, float w);
    void setUniformVec3(const char *name, float x, float y, float z);
    void setUniformVec2(const char *name, float x, float y);
    void setUniformFloat(const char *name, float x);
    void setUniformInt(const char *name, int x);
    void setUniformBool(const char *name, bool x);
    void setUniformMat4(const char *name, glm::mat4 matrix);
};

enum simple_camera_types
{
    CAMERA_STATIONARY,
    CAMERA_2D_FOLLOW,
    CAMERA_3D,
};

struct camera
{
    glm::vec3 cameraPosition;
    glm::vec3 cameraTarget;
    glm::vec3 cameraFront;
    glm::vec3 cameraLockedFront;
    glm::vec3 cameraDirection;
    glm::vec3 worldUp;
    glm::vec3 cameraRight;
    glm::vec3 cameraLockedRight;
    glm::vec3 cameraUp;
    float pitch = 0.0f, yaw = 0.0f;
    float lastMouseX = 0.0f;
    float lastMouseY = 0.0f;
    bool firstMouse = true;
    bool fullscreen = false, swappedFullscreen = false;
    bool mouseCapture = true, swappedMouseCapture = false;
    glm::vec3 cameraVelocity = glm::vec3(0.0f);
    bool jumped = false;
    float fov = 90.0f, zoomed_fov = 20.0f, current_fov = 90.0f;
    simple_camera_types type;

    camera(simple_camera_types _type)
    {
        type = _type;

        switch (_type)
        {
        case CAMERA_STATIONARY:
            cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
            cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
            cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
            cameraLockedFront = glm::vec3(0.0f, 0.0f, -1.0f);
            cameraDirection = glm::normalize(cameraTarget - cameraPosition);
            worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
            cameraRight = glm::normalize(glm::cross(worldUp, cameraDirection));
            cameraLockedRight = glm::vec3(0.0f);
            cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));
            break;
        case CAMERA_2D_FOLLOW:
            cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
            cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
            cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
            cameraLockedFront = glm::vec3(0.0f, 0.0f, -1.0f);
            cameraDirection = glm::normalize(cameraTarget - cameraPosition);
            worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
            cameraRight = glm::normalize(glm::cross(worldUp, cameraDirection));
            cameraLockedRight = glm::vec3(0.0f);
            cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));
            break;
        case CAMERA_3D:
            cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
            cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
            cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
            cameraLockedFront = glm::vec3(0.0f, 0.0f, -1.0f);
            cameraDirection = glm::normalize(cameraTarget - cameraPosition);
            worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
            cameraRight = glm::normalize(glm::cross(worldUp, cameraDirection));
            cameraLockedRight = glm::vec3(0.0f);
            cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));
            break;
        default:
            cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
            cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
            cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
            cameraLockedFront = glm::vec3(0.0f, 0.0f, -1.0f);
            cameraDirection = glm::normalize(cameraTarget - cameraPosition);
            worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
            cameraRight = glm::normalize(glm::cross(worldUp, cameraDirection));
            cameraLockedRight = glm::vec3(0.0f);
            cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));
            break;
            break;
        }
    }

    void init()
    {
    }
};

#endif