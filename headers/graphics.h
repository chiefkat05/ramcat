#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

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

    void setUniformVec4(const char *name, double x, double y, double z, double w);
    void setUniformVec3(const char *name, double x, double y, double z);
    void setUniformVec2(const char *name, double x, double y);
    void setUniformDouble(const char *name, double x);
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
    glm::dvec3 cameraPosition;
    glm::dvec3 cameraTarget;
    glm::dvec3 cameraFront;
    glm::dvec3 cameraLockedFront;
    glm::dvec3 cameraDirection;
    glm::dvec3 worldUp;
    glm::dvec3 cameraRight;
    glm::dvec3 cameraLockedRight;
    glm::dvec3 cameraUp;
    double pitch = 0.0, yaw = 0.0;
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;
    bool firstMouse = true;
    bool fullscreen = false, swappedFullscreen = false;
    bool mouseCapture = true, swappedMouseCapture = false;
    glm::dvec3 cameraVelocity = glm::dvec3(0.0);
    bool jumped = false;
    double fov = 90.0, zoomed_fov = 20.0, current_fov = 90.0;
    simple_camera_types type;
    glm::dvec3 boundaryCorner, boundarySizeCorner;
    bool boundaryset = false;
    double *cameraLockX, *cameraLockY, *cameraLockZ;
    double offsetX = 0.0, offsetY = 0.0, offsetZ = 0.0;

    camera(simple_camera_types _type)
    {
        type = _type;

        switch (_type)
        {
        case CAMERA_STATIONARY:
            cameraPosition = glm::dvec3(0.0, 0.0, 3.0);
            cameraTarget = glm::dvec3(0.0, 0.0, 0.0);
            cameraFront = glm::dvec3(0.0, 0.0, -1.0);
            cameraLockedFront = glm::dvec3(0.0, 0.0, -1.0);
            cameraDirection = glm::normalize(cameraTarget - cameraPosition);
            worldUp = glm::dvec3(0.0, 1.0, 0.0);
            cameraRight = glm::normalize(glm::cross(worldUp, cameraDirection));
            cameraLockedRight = glm::dvec3(0.0);
            cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));
            break;
        case CAMERA_2D_FOLLOW:
            cameraPosition = glm::dvec3(0.0, 0.0, 3.0);
            cameraTarget = glm::dvec3(0.0, 0.0, 0.0);
            cameraFront = glm::dvec3(0.0, 0.0, -1.0);
            cameraLockedFront = glm::dvec3(0.0, 0.0, -1.0);
            cameraDirection = glm::normalize(cameraTarget - cameraPosition);
            worldUp = glm::dvec3(0.0, 1.0, 0.0);
            cameraRight = glm::normalize(glm::cross(worldUp, cameraDirection));
            cameraLockedRight = glm::dvec3(0.0);
            cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));
            break;
        case CAMERA_3D:
            cameraPosition = glm::dvec3(0.0, 0.0, 3.0);
            cameraTarget = glm::dvec3(0.0, 0.0, 0.0);
            cameraFront = glm::dvec3(0.0, 0.0, -1.0);
            cameraLockedFront = glm::dvec3(0.0, 0.0, -1.0);
            cameraDirection = glm::normalize(cameraTarget - cameraPosition);
            worldUp = glm::dvec3(0.0, 1.0, 0.0);
            cameraRight = glm::normalize(glm::cross(worldUp, cameraDirection));
            cameraLockedRight = glm::dvec3(0.0);
            cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));
            break;
        default:
            cameraPosition = glm::dvec3(0.0, 0.0, 3.0);
            cameraTarget = glm::dvec3(0.0, 0.0, 0.0);
            cameraFront = glm::dvec3(0.0, 0.0, -1.0);
            cameraLockedFront = glm::dvec3(0.0, 0.0, -1.0);
            cameraDirection = glm::normalize(cameraTarget - cameraPosition);
            worldUp = glm::dvec3(0.0, 1.0, 0.0);
            cameraRight = glm::normalize(glm::cross(worldUp, cameraDirection));
            cameraLockedRight = glm::dvec3(0.0);
            cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));
            break;
            break;
        }
    }

    void setBoundary(double x, double y, double z, double x2, double y2, double z2)
    {
        boundaryCorner = glm::dvec3(x, y, z);
        boundarySizeCorner = glm::dvec3(x2, y2, z2);

        boundaryset = true;
    }
    void lockTo(double *x, double *y = nullptr, double *z = nullptr)
    {
        cameraLockX = x;
        cameraLockY = y;
        cameraLockZ = z;
    }

    void update(double followSpeed)
    {
        // if (cameraLockX != nullptr)
        //     cameraPosition.x = *cameraLockX;
        // if (cameraLockY != nullptr)
        //     cameraPosition.y = *cameraLockY;
        // if (cameraLockZ != nullptr)
        //     cameraPosition.z = *cameraLockZ;
        if (cameraLockX != nullptr)
        {
            cameraPosition.x += followSpeed * (*cameraLockX - cameraPosition.x + offsetX); // I tried to use std::lerp but it refused to exist for some reason even with cmath include
        }
        if (cameraLockY != nullptr)
        {
            cameraPosition.y += followSpeed * (*cameraLockY - cameraPosition.y + offsetY);
        }
        if (cameraLockZ != nullptr)
        {
            cameraPosition.z += followSpeed * (*cameraLockZ - cameraPosition.z + offsetZ);
        }

        if (boundaryset)
        {
            if (cameraPosition.x < boundaryCorner.x)
                cameraPosition.x = boundaryCorner.x;
            if (cameraPosition.y < boundaryCorner.y)
                cameraPosition.y = boundaryCorner.y;
            if (cameraPosition.z < boundaryCorner.z)
                cameraPosition.z = boundaryCorner.z;

            if (cameraPosition.x > boundarySizeCorner.x)
                cameraPosition.x = boundarySizeCorner.x;
            if (cameraPosition.y > boundarySizeCorner.y)
                cameraPosition.y = boundarySizeCorner.y;
            if (cameraPosition.z > boundarySizeCorner.z)
                cameraPosition.z = boundarySizeCorner.z;
        }
    }

    void init()
    {
    }
};

#endif