#include "../headers/collision.h"
#include <limits>

bool aabb::colliding(aabb &test)
{
    if (min_x >= test.max_x || min_y >= test.max_y ||
        test.min_x >= max_x || test.min_y >= max_y)
        return false;

    return true;
}

void aabb::moveCenterToPoint(double xPos, double yPos)
{
    double width = max_x - min_x;
    double height = max_y - min_y;
    min_x = xPos - width * 0.5f;
    min_y = yPos - height * 0.5f;
    max_x = xPos + width * 0.5f;
    max_y = yPos + height * 0.5f;
}
void aabb::Put(double x, double y, double w, double h)
{
    min_x = x;
    min_y = y;
    max_x = x + w;
    max_y = y + h;
}

// special thanks to the excellent book, "real time collision detection" by Christer Ericson,
// as well as the online tutorial -> https://gamedev.net/tutorials/programming/general-and-gameplay-programming/swept-aabb-collision-detection-and-response-r3084/ by BrendanL.K
double aabb::response(double xV, double yV, double xV2, double yV2, aabb &test, double &xNormal, double &yNormal)
{
    aabb boundingbox1;

    double velX = xV;
    double velY = yV;

    boundingbox1 = aabb(min_x + std::min(0.0, velX * 2.0), min_y + std::min(0.0, velY * 2.0), max_x + std::max(0.0, velX * 2.0), max_y + std::max(0.0, velY * 2.0));

    velX = xV;
    velY = yV;
    // if (xV != 0.0 && yV == 0.0)
    //     boundingbox1 = aabb(min_x + std::min(0.0, xV * 2.0), min_y + 0.01f, max_x + std::max(0.0, xV * 2.0), max_y - 0.01f);
    // if (xV == 0.0 && yV != 0.0)
    // boundingbox1 = aabb(min_x + 0.01f, min_y + std::min(0.0, yV * 2.0), max_x - 0.01f, max_y + std::max(0.0, yV * 2.0));
    // boundingbox1 = aabb(min_x + std::max)

    // aabb boundingbox1(min_x + 0.1f, min_y + 0.1f, max_x - 0.1f, max_y - 0.1f);
    // aabb boundingbox2(test.min_x + std::min(0.0, xV2 * 2.0), test.min_y + std::min(0.0, yV2 * 2.0), test.max_x + std::max(0.0, xV2 * 2.0), test.max_y + std::max(0.0, yV2 * 2.0));

    if (!boundingbox1.colliding(test))
    {
        xNormal = 0.0;
        yNormal = 0.0;
        return 1.0;
    }

    // double velX = xV2 - xV;
    // double velY = yV2 - yV;

    double xInvEntry = 0.0, yInvEntry = 0.0;
    double xInvExit = 0.0, yInvExit = 0.0;

    if (velX > 0.0)
    {
        xInvEntry = test.min_x - max_x;
        xInvExit = test.max_x - min_x;
    }
    else
    {
        xInvEntry = test.max_x - min_x;
        xInvExit = test.min_x - max_x;
    }
    if (velY > 0.0)
    {
        yInvEntry = test.min_y - max_y;
        yInvExit = test.max_y - min_y;
    }
    else
    {
        yInvEntry = test.max_y - min_y;
        yInvExit = test.min_y - max_y;
    }

    double xEntry, yEntry;
    double xExit, yExit;

    if (velX == 0.0)
    {
        xEntry = -std::numeric_limits<double>::infinity();
        xExit = std::numeric_limits<double>::infinity();
    }
    else
    {
        xEntry = xInvEntry / velX;
        xExit = xInvExit / velX;
    }
    if (velY == 0.0)
    {
        yEntry = -std::numeric_limits<double>::infinity();
        yExit = std::numeric_limits<double>::infinity();
    }
    else
    {
        yEntry = yInvEntry / velY;
        yExit = yInvExit / velY;
    }

    double entryTime = std::max(xEntry, yEntry);
    double exitTime = std::min(xExit, yExit);

    if (entryTime > exitTime || xEntry < 0.0 && yEntry < 0.0 || xEntry > 1.0 || yEntry > 1.0)
    {
        xNormal = 0.0;
        yNormal = 0.0;
        return 1.0;
    }

    if (xEntry > yEntry)
    {
        if (xInvEntry < 0.0 || xInvEntry == 0.0 && velX < 0.0)
        {
            xNormal = 1.0;
            yNormal = 0.0;
        }
        if (xInvEntry > 0.0 || xInvEntry == 0.0 && velX > 0.0)
        {
            xNormal = -1.0;
            yNormal = 0.0;
        }
    }
    if (xEntry < yEntry)
    {
        if (yInvEntry < 0.0 || yInvEntry == 0.0 && velY < 0.0)
        {
            xNormal = 0.0;
            yNormal = 1.0;
        }
        if (yInvEntry > 0.0 || yInvEntry == 0.0 && velY > 0.0)
        {
            xNormal = 0.0;
            yNormal = -1.0;
        }
    }
    if (xEntry == yEntry)
    {
        xNormal = 0.0;
        yNormal = 0.0;
        return 1.0f;
    }

    return entryTime;
}