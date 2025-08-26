#ifndef COLLISION_H
#define COLLISION_H

#include <iostream>

enum COLLIDER_PARENT_TYPE
{
    NONE,
    CHARACTER,
    TILE,
    parent_type_limit
};

struct character;

// template <typename T>
struct aabb
{
    double min_x, min_y, max_x, max_y;
    double xOffsetFromParent = 0.0, yOffsetFromParent = 0.0;
    int colliderID = -1;
    int specialTileX = 0;
    int specialTileY = 0;

    bool collisionThisFrame = false;
    int collisionNormalX = 0, collisionNormalY = 0;
    double collisionDistanceToSide = 0.0, collisionSnapValue = 0.0;

    aabb *next_aabb = nullptr;
    character *parent_object = nullptr;
    // T *parent_object;
    // COLLIDER_PARENT_TYPE parent_type;

    aabb() : min_x(0.0), min_y(0.0), max_x(0.0), max_y(0.0) {}
    aabb(double x, double y, double x2, double y2, character *p = nullptr) : min_x(x), min_y(y), max_x(x2), max_y(y2), parent_object(p) {}

    bool colliding(aabb &test);

    void Set(double x, double y, double w, double h);
    void Put(double x, double y);
    void Scale(double w, double h);

    double response(double xV, double yV, double xV2, double yV2, aabb test, int &xNormal, int &yNormal, double &xPos, double &yPos, bool &insideCollision, double &distanceToSide);
    // double response(double xV, double yV, double xV2, double yV2, aabb test, double &tfirst, double &tlast, double &xNormal, double &yNormal,
    // double &xPos, double &yPos, bool &insideCollision);

    constexpr double centerX()
    {
        return (min_x + max_x) * 0.5;
    }
    constexpr double centerY()
    {
        return (min_y + max_y) * 0.5;
    }
};

#endif