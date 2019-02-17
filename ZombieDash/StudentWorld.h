#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <list>
#include <string>

using namespace std;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class Actor;
class Penelope;
class Wall;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    // Checks if
    bool isActorBlocked(const Actor& actor);
private:
    Penelope* m_Penelope;
    list<Actor*> liActor;
};

// Helper functions
bool isWithinHeight(double a_bot, double a_top, double b_bot, double b_top);
bool isWithinWidth(double a_left, double a_right, double b_left, double b_right);
#endif // STUDENTWORLD_H_
