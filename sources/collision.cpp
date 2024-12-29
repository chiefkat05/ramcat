#include "../headers/collision.h"
#include <limits>

bool aabb::colliding(aabb &test)
{
    if (min_x > test.max_x || min_y > test.max_y ||
        test.min_x > max_x || test.min_y > max_y)
        return false;

    return true;
}

void aabb::moveCenterToPoint(float xPos, float yPos)
{
    float width = max_x - min_x;
    float height = max_y - min_y;
    min_x = xPos - width * 0.5f;
    min_y = yPos - height * 0.5f;
    max_x = xPos + width * 0.5f;
    max_y = yPos + height * 0.5f;
}
// special thanks to the excellent book, "real time collision detection" by Christer Ericson,
// as well as the online tutorial -> https://gamedev.net/tutorials/programming/general-and-gameplay-programming/swept-aabb-collision-detection-and-response-r3084/ by BrendanL.K
float aabb::response(float xVelocity, float yVelocity, aabb &test, float &xNormal, float &yNormal)
{
    aabb boundingbox = aabb(min_x + std::min(xVelocity, 0.0f), min_y + std::min(yVelocity, 0.0f),
                            max_x + std::max(xVelocity, 0.0f), max_y + std::max(yVelocity, 0.0f));
    if (!test.colliding(boundingbox))
        return 1.0f;

    float tFirst = 0.0f;
    float tLast = 1.0f;
    float xEntry = 0.0f, yEntry = 0.0f, xInvEntry = 0.0f, yInvEntry = 0.0f;

    if (xVelocity < 0.0f)
        xInvEntry = (min_x - test.max_x) / xVelocity;
    if (xVelocity > 0.0f)
        xInvEntry = (test.min_x - max_x) / xVelocity;
    if (yVelocity < 0.0f)
        yInvEntry = (min_y - test.max_y) / yVelocity;
    if (yVelocity > 0.0f)
        yInvEntry = (test.min_y - max_y) / yVelocity;

    // if (std::abs(xInvEntry) < std::abs(yInvEntry))
    // {
    //     xNormal = 1.0f;
    // }
    // else
    // {
    //     yNormal = 1.0f;
    // }

    if (xVelocity == 0.0f)
    {
        xEntry = -std::numeric_limits<float>::infinity();
    }
    if (xVelocity != 0.0f)
    {
        xEntry = xInvEntry / xVelocity;
    }
    if (yVelocity == 0.0f)
    {
        yEntry = -std::numeric_limits<float>::infinity();
    }
    if (yVelocity != 0.0f)
    {
        yEntry = yInvEntry / yVelocity;
    }

    if (xEntry > yEntry)
    {
        if (xInvEntry < 0.0f)
        {
            xNormal = 1.0f;
        }
        else
        {
            xNormal = -1.0f;
        }
    }
    else
    {
        if (yInvEntry < 0.0f)
        {
            yNormal = 1.0f;
        }
        else
        {
            yNormal = -1.0f;
        }
    }

    if (colliding(test))
    {
        return 0.0f;
    }

    if (xVelocity < 0.0f)
    {
        if (test.max_x < min_x)
            return 0.0f;
        if (max_x < test.min_x)
            tFirst = std::max((max_x - test.min_x) / xVelocity, tFirst);
        if (test.max_x > min_x)
            tLast = std::min((min_x - test.max_x) / xVelocity, tLast);
    }
    if (xVelocity > 0.0f)
    {
        if (test.min_x > max_x)
            return 0.0f;
        if (test.max_x < min_x)
            tFirst = std::max((min_x - test.max_x) / xVelocity, tFirst);
        if (max_x > test.min_x)
            tLast = std::min((max_x - test.min_x) / xVelocity, tLast);
    }
    if (yVelocity < 0.0f)
    {
        if (test.max_y < min_y)
            return 0.0f;
        if (max_y < test.min_y)
            tFirst = std::max((max_y - test.min_y) / yVelocity, tFirst);
        if (test.max_y > min_y)
            tLast = std::min((min_y - test.max_y) / yVelocity, tLast);
    }
    if (yVelocity > 0.0f)
    {
        if (test.min_y > max_y)
            return 0.0f;
        if (test.max_y < min_y)
            tFirst = std::max((min_y - test.max_y) / yVelocity, tFirst);
        if (max_y > test.min_y)
            tLast = std::min((max_y - test.min_y) / yVelocity, tLast);
    }

    if (tFirst > tLast)
        return 0.0f;

    return tFirst;
}