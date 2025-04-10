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
double aabb::response(double xV, double yV, double xV2, double yV2, aabb &test, double &xNormal, double &yNormal, double &xPos, double &yPos, bool &insideCollision)
{
    aabb boundingbox1;

    double velX = xV;
    double velY = yV;

    boundingbox1 = aabb(min_x + std::min(0.0, velX), min_y + std::min(0.0, velY), max_x + std::max(0.0, velX), max_y + std::max(0.0, velY));

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

    // Kinda proud of this one too, inside-block-collisions was very fun to solve and I feel like this is very compact and simple enough that there's
    // unlikely to be any major bugs resulting from normal usage of it
    insideCollision = false;
    if (colliding(test))
    {
        // for future me that's probably a little thick in the head, let me explain real quick
        // it's negative (minus) velocity for all the values below because it needs to be the opposite of wherever the
        // player is moving. If the player is going to the left (-0.5) then it needs to check to the right of the player (0.5)
        // if the player is moving right (0.5) then it needs to check to the left of the player (-0.5)
        // thank you for understanding, have a great day and keep working hard :)

        // Also, why does using the current frame's velocity work? I thought I would need to use the last frame's velocity but
        // somehow this doesn't lead to any floating point errors
        double badLastYPositionUnder = max_y - velY;
        double badLastYPositionAbove = min_y - velY;
        double badLastYPositionLeft = min_x - velX;
        double badLastYPositionRight = max_x - velX;

        // also, I just noticed a potential bug in this system, where if the hitbox is smaller than the player's hitbox
        // then he will always just move to the right or down when he's inside of it since all of the badLastPosition variables
        // are correctly outside the hitbox boundaries and it will trigger only based off player movement...
        // this won't likely be a problem in any of the games I make, and it's not a big deal in general, and in every other way
        // this solution is pretty much perfect, so I'm going to leave it like this. But I will also document this in case it ever comes up.
        if (velY != 0.0)
        {
            float returnY = yPos;
            if (velY < 0.0 && badLastYPositionAbove >= test.max_y)
            {
                double yoffset = yPos - min_y;
                returnY = test.max_y + ((max_y - min_y) - yoffset) * 0.5;
                yNormal = 1.0;
            }
            if (velY > 0.0 && badLastYPositionUnder <= test.min_y)
            {
                double yoffset = max_y - yPos;
                returnY = test.min_y - ((max_y - min_y) + yoffset) * 0.5;
                yNormal = -1.0;
            }
            insideCollision = true;
            xNormal = 0.0;
            return returnY;
        }
        if (velX != 0.0)
        {
            float returnX = xPos;
            xNormal = 0.0;
            if (velX > 0.0 && badLastYPositionLeft <= test.min_x)
            {
                double xoffset = max_x - xPos;
                returnX = test.min_x - ((max_x - min_x) + xoffset) * 0.5;

                xNormal = -1.0;
            }
            if (velX < 0.0 && badLastYPositionRight >= test.max_x)
            {
                double xoffset = xPos - min_x;
                returnX = test.max_x + ((max_x - min_x) - xoffset) * 0.5;
                yNormal = 1.0;
            }
            insideCollision = true;
            yNormal = 0.0;
            return returnX;
        }
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