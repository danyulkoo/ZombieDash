#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

void Penelope::doSomething()
{
    int ch;
    if (getWorld()->getKey(ch))
    {
        switch (ch)
        {
            case KEY_PRESS_UP:
                setDirection(up);
                moveTo(getX(), getY() + 4);
                break;
            case KEY_PRESS_DOWN:
                setDirection(down);
                moveTo(getX(), getY() - 4);
                break;
            case KEY_PRESS_LEFT:
                setDirection(left);
                // Check if Penelope can move
                if (getWorld()->isActorBlocked(*this))
                    break;
                moveTo(getX() - 4, getY());
                break;
            case KEY_PRESS_RIGHT:
                setDirection(right);
                moveTo(getX() + 4, getY());
                break;
        }
    }
}
