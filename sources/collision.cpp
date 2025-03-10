#include "../headers/collision.h"
#include <limits>

bool aabb::colliding(aabb &test)
{
    if (min_x >= test.max_x || min_y >= test.max_y ||
        test.min_x >= max_x || test.min_y >= max_y)
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
void aabb::Put(float x, float y, float w, float h)
{
    min_x = x;
    min_y = y;
    max_x = x + w;
    max_y = y + h;
}

// special thanks to the excellent book, "real time collision detection" by Christer Ericson,
// as well as the online tutorial -> https://gamedev.net/tutorials/programming/general-and-gameplay-programming/swept-aabb-collision-detection-and-response-r3084/ by BrendanL.K
float aabb::response(float xV, float yV, float xV2, float yV2, aabb &test, float &xNormal, float &yNormal)
{
    aabb boundingbox1;

    float velX = xV;
    float velY = yV;

    boundingbox1 = aabb(min_x + std::min(0.0f, velX * 2.0f), min_y + std::min(0.0f, velY * 2.0f), max_x + std::max(0.0f, velX * 2.0f), max_y + std::max(0.0f, velY * 2.0f));

    velX = xV;
    velY = yV;
    // if (xV != 0.0f && yV == 0.0f)
    //     boundingbox1 = aabb(min_x + std::min(0.0f, xV * 2.0f), min_y + 0.01f, max_x + std::max(0.0f, xV * 2.0f), max_y - 0.01f);
    // if (xV == 0.0f && yV != 0.0f)
    // boundingbox1 = aabb(min_x + 0.01f, min_y + std::min(0.0f, yV * 2.0f), max_x - 0.01f, max_y + std::max(0.0f, yV * 2.0f));
    // boundingbox1 = aabb(min_x + std::max)

    // aabb boundingbox1(min_x + 0.1f, min_y + 0.1f, max_x - 0.1f, max_y - 0.1f);
    // aabb boundingbox2(test.min_x + std::min(0.0f, xV2 * 2.0f), test.min_y + std::min(0.0f, yV2 * 2.0f), test.max_x + std::max(0.0f, xV2 * 2.0f), test.max_y + std::max(0.0f, yV2 * 2.0f));

    if (!boundingbox1.colliding(test))
    {
        xNormal = 0.0f;
        yNormal = 0.0f;
        return 1.0f;
    }

    // float velX = xV2 - xV;
    // float velY = yV2 - yV;

    float xInvEntry = 0.0f, yInvEntry = 0.0f;
    float xInvExit = 0.0f, yInvExit = 0.0f;

    if (velX > 0.0f)
    {
        xInvEntry = test.min_x - max_x;
        xInvExit = test.max_x - min_x;
    }
    else
    {
        xInvEntry = test.max_x - min_x;
        xInvExit = test.min_x - max_x;
    }
    if (velY > 0.0f)
    {
        yInvEntry = test.min_y - max_y;
        yInvExit = test.max_y - min_y;
    }
    else
    {
        yInvEntry = test.max_y - min_y;
        yInvExit = test.min_y - max_y;
    }

    float xEntry, yEntry;
    float xExit, yExit;

    if (velX == 0.0f)
    {
        xEntry = -std::numeric_limits<float>::infinity();
        xExit = std::numeric_limits<float>::infinity();
    }
    else
    {
        xEntry = xInvEntry / velX;
        xExit = xInvExit / velX;
    }
    if (velY == 0.0f)
    {
        yEntry = -std::numeric_limits<float>::infinity();
        yExit = std::numeric_limits<float>::infinity();
    }
    else
    {
        yEntry = yInvEntry / velY;
        yExit = yInvExit / velY;
    }

    float entryTime = std::max(xEntry, yEntry);
    float exitTime = std::min(xExit, yExit);

    if (entryTime > exitTime || xEntry < 0.0f && yEntry < 0.0f || xEntry > 1.0f || yEntry > 1.0f)
    {
        xNormal = 0.0f;
        yNormal = 0.0f;
        return 1.0f;
    }

    if (xEntry > yEntry)
    {
        if (xInvEntry < 0.0f)
        {
            xNormal = 1.0f;
            yNormal = 0.0f;
        }
        else
        {
            xNormal = -1.0f;
            yNormal = 0.0f;
        }
    }
    else if (xEntry < yEntry)
    {
        if (yInvEntry < 0.0f)
        {
            xNormal = 0.0f;
            yNormal = 1.0f;
        }
        else
        {
            xNormal = 0.0f;
            yNormal = -1.0f;
        }
    }
    if (xEntry == yEntry)
    {
        xNormal = 0.0f;
        yNormal = 0.0f;
        return 0.0f;
    }

    return entryTime;
}