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
double aabb::response(double xV, double yV, double xV2, double yV2, aabb test, double &xNormal, double &yNormal, double &xPos, double &yPos, bool &insideCollision)
{
    aabb boundingbox1, boundingbox2;

    double velX = xV;
    double velY = yV;

    // velX = xV;
    // velY = yV;
    // velX = xV - xV2;
    // velY = yV - yV2;
    // velX = xV2 - xV;
    // velY = yV2 - yV;

    boundingbox1 = aabb(min_x + std::min(0.0, xV), min_y + std::min(0.0, yV), max_x + std::max(0.0, xV), max_y + std::max(0.0, yV));
    boundingbox2 = aabb(test.min_x + std::min(0.0, xV2), test.min_y + std::min(0.0, yV2), test.max_x + std::max(0.0, xV2), test.max_y + std::max(0.0, yV2));

    if (!boundingbox1.colliding(boundingbox2))
    {
        xNormal = 0.0;
        yNormal = 0.0;
        return 1.0;
    }
    // if (yV2 != 0.0)
    //     std::cout << " hoawiefjqoeifjqoeirf\n";

    // Kinda proud of this one too, inside-block-collisions was very fun to solve and I feel like this is very compact and simple enough that there's
    // unlikely to be any major bugs resulting from normal usage of it
    // boundingbox1 = aabb(min_x + xV - xV2, min_y + yV - yV2, max_x + xV - xV2, max_y + yV - yV2);
    if (colliding(test))
    {
        // std::cout << velX << ", xV = " << xV << ", xV2 = " << xV2 << " COLLISION COLLISION\n";
        // for future me that's probably a little thick in the head, let me explain real quick
        // it's negative (minus) velocity for all the values below because it needs to be the opposite of wherever the
        // player is moving. If the player is going to the left (-0.5) then it needs to check to the right of the player (0.5)
        // if the player is moving right (0.5) then it needs to check to the left of the player (-0.5)
        // thank you for understanding, have a great day and keep working hard :)

        // Also, why does using the current frame's velocity work? I thought I would need to use the last frame's velocity but
        // somehow this doesn't lead to any floating point errors
        double lastPositionTop = max_y - yV;
        double lastPositionBottom = min_y - yV;
        double lastPositionLeft = min_x - xV;
        double lastPositionRight = max_x - xV;

        // also, I just noticed a potential bug in this system, where if the hitbox is smaller than the player's hitbox
        // then he will always just move to the right or down when he's inside of it since all of the badLastPosition variables
        // are correctly outside the hitbox boundaries and it will trigger only based off player movement...
        // this won't likely be a problem in any of the games I make, and it's not a big deal in general, and in every other way
        // this solution is pretty much perfect, so I'm going to leave it like this. But I will also document this in case it ever comes up.

        // well, I think I ironed out most of the major issues with moving objects colliding with moving objects... but I feel like I've not seen the last of them.

        // one last note because I just spent a long time not understanding a basic function of this code
        // lastPositionBottom >= test.max_y - yV2 IS CORRECT
        // because the if statement is checking whether the player was OUTSIDE the block, NOT INSIDE
        // WHETHER THE PLAYER IS INSIDE THE BLOCK IS HANDLED BY THE ABOVE IF(COLLIDING(TEST)) STATEMENT, YOU SPONGE-BRAINED MONGREL
        // thank you for understanding, have a great day and work hard :)
        float returnY = yPos;
        if (yV < 0.0 && lastPositionBottom >= test.max_y - yV2)
        // if (yV < 0.0 && lastPositionBottom >= test.max_y + yV2)
        {
            double yoffset = yPos - min_y;
            returnY = test.max_y + yoffset; // sigh

            yNormal = 1.0;
            xNormal = 0.0;
            insideCollision = true;

            return returnY;
        }
        if (yV > 0.0 && lastPositionTop <= test.min_y - yV2)
        // if (yV > 0.0 && lastPositionTop <= test.min_y + yV2)
        {
            double yoffset = max_y - yPos;
            returnY = test.min_y - yoffset;

            yNormal = -1.0;
            xNormal = 0.0;
            insideCollision = true;

            return returnY;
        }
        float returnX = xPos;
        if (xV < 0.0 && lastPositionLeft >= test.max_x - xV2)
        // if (xV < 0.0 && lastPositionLeft >= test.max_x + xV2)
        {
            double xoffset = xPos - min_x;
            returnX = test.max_x + xoffset;

            xNormal = -1.0;
            yNormal = 0.0;
            insideCollision = true;

            return returnX;
        }
        if (xV > 0.0 && lastPositionRight <= test.min_x - xV2)
        // if (xV > 0.0 && lastPositionRight <= test.min_x + xV2)
        {
            double xoffset = max_x - xPos;
            returnX = test.min_x - xoffset;

            xNormal = 1.0;
            yNormal = 0.0;
            insideCollision = true;

            return returnX;
        }
    }

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