#ifndef COLLISION_H
#define COLLISION_H

#include <iostream>

struct aabb
{
    float min_x, min_y, max_x, max_y;
    int collisionID = 0;

    aabb() : min_x(0.0f), min_y(0.0f), max_x(0.0f), max_y(0.0f) {}
    aabb(float x, float y, float x2, float y2) : min_x(x), min_y(y), max_x(x2), max_y(y2) {}

    bool colliding(aabb &test);

    void moveCenterToPoint(float xPos, float yPos);

    float response(float xVelocity, float yVelocity, aabb &test, float &xNormal, float &yNormal);
};

const unsigned int quadtree_object_limit = 5;
struct quadtree
{
    aabb bounds;
    quadtree *bottomleft, *bottomright, *topleft, *topright;

    aabb objects[quadtree_object_limit];
};

#endif