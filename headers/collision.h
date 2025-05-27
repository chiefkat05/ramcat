#ifndef COLLISION_H
#define COLLISION_H

#include <iostream>

struct aabb
{
    double min_x, min_y, max_x, max_y;
    int collisionID = -1;
    int specialTileX = 0;
    int specialTileY = 0;

    aabb() : min_x(0.0), min_y(0.0), max_x(0.0), max_y(0.0) {}
    aabb(double x, double y, double x2, double y2) : min_x(x), min_y(y), max_x(x2), max_y(y2) {}

    bool colliding(aabb &test);

    void Set(double x, double y, double w, double h);
    void Put(double x, double y);
    void Scale(double w, double h);

    double response(double xV, double yV, double xV2, double yV2, aabb test, double &xNormal, double &yNormal, double &xPos, double &yPos, bool &insideCollision, double &distanceToSide);
    // double response(double xV, double yV, double xV2, double yV2, aabb test, double &tfirst, double &tlast, double &xNormal, double &yNormal,
    // double &xPos, double &yPos, bool &insideCollision);
};

#endif