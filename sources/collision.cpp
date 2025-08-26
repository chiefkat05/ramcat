#include "../headers/collision.h"
#include <limits>

bool aabb::colliding(aabb &test)
{
    if (min_x >= test.max_x || min_y >= test.max_y ||
        test.min_x >= max_x || test.min_y >= max_y)
        return false;

    return true;
}

void aabb::Set(double x, double y, double w, double h)
{
    min_x = x;
    min_y = y;
    max_x = x + w;
    max_y = y + h;
}
void aabb::Put(double x, double y)
{
    double width = max_x - min_x;
    double height = max_y - min_y;
    min_x = x;
    min_y = y;
    max_x = x + width;
    max_y = y + height;
}
void aabb::Scale(double w, double h)
{
    max_x = min_x + w;
    max_y = min_y + h;
}

// special thanks to the excellent book, "real time collision detection" by Christer Ericson,
// as well as the online tutorial -> https://gamedev.net/tutorials/programming/general-and-gameplay-programming/swept-aabb-collision-detection-and-response-r3084/ by BrendanL.K
double aabb::response(double xV, double yV, double xV2, double yV2, aabb test, int &xNormal, int &yNormal, double &xPos, double &yPos, bool &insideCollision,
                      double &distanceToSide)
{
    aabb boundingA(min_x + std::min(xV, 0.0), min_y + std::min(yV, 0.0), max_x + std::max(xV, 0.0), max_y + std::max(yV, 0.0));
    aabb boundingB(test.min_x + std::min(xV2, 0.0), test.min_y + std::min(yV2, 0.0), test.max_x + std::max(xV2, 0.0), test.max_y + std::max(yV2, 0.0));
    if (!boundingA.colliding(boundingB)) // next we need to make the collision boxes correct number-wise and then see what the trouble is with playing the strike animation for gulk.
    {                                    // Also we need to get gulk to move and try getting it to turn around and see what other collision things you can do to test
        insideCollision = false;
        return 0.0;
    }
    insideCollision = true;

    double velX = xV - xV2;
    double velY = yV - yV2;

    aabb testBoxExpanded = aabb(test.min_x - (max_x - xPos),
                                test.min_y - (max_y - yPos),
                                test.max_x + (xPos - min_x),
                                test.max_y + (yPos - min_y));

    double AxNext = xPos + velX;
    double AyNext = yPos + velY;

    double maxDistance = -std::numeric_limits<double>::infinity();
    if (maxDistance <= testBoxExpanded.min_x - xPos)
    {
        maxDistance = testBoxExpanded.min_x - xPos;
        xNormal = -1;
        yNormal = 0;
    }
    if (maxDistance <= xPos - testBoxExpanded.max_x)
    {
        maxDistance = xPos - testBoxExpanded.max_x;
        xNormal = 1;
        yNormal = 0;
    }
    if (maxDistance <= testBoxExpanded.min_y - yPos)
    {
        maxDistance = testBoxExpanded.min_y - yPos;
        xNormal = 0;
        yNormal = -1;
    }
    if (maxDistance <= yPos - testBoxExpanded.max_y)
    {
        maxDistance = yPos - testBoxExpanded.max_y;
        xNormal = 0;
        yNormal = 1;
    }
    distanceToSide = maxDistance;

    if (xNormal == -1 && xV > 0.0)
    {
        return testBoxExpanded.min_x;
    }
    if (xNormal == 1 && xV < 0.0)
    {
        return testBoxExpanded.max_x;
    }
    if (yNormal == -1 && yV > 0.0)
    {
        return testBoxExpanded.min_y;
    }
    if (yNormal == 1 && yV < 0.0)
    {
        return testBoxExpanded.max_y;
    }

    if (xNormal != 0.0)
        return xPos;
    if (yNormal != 0.0)
        return yPos;

    insideCollision = false;
    // here to avoid compiler complaints
    return 0.0;
}