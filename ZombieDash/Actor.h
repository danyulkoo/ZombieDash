#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Actor: public GraphObject
{
public:
    // Constructor
    Actor(int imageID, double startX, double startY, int dir, int depth, double size, StudentWorld* world)
    : GraphObject(imageID, startX, startY, dir, depth, size), m_world(world), m_isAlive(true)
    {}
    
    // Destructor
    virtual ~Actor() {}
    
    // Simple const functions
    StudentWorld* getWorld() const  {return m_world;}
    bool isAlive() const            {return m_isAlive;}
    
    void setDead() {m_isAlive = false;}
    virtual void doSomething() = 0;
    
private:
    StudentWorld* m_world;
    bool m_isAlive;
};

class Wall: public Actor
{
public:
    Wall(double startX, double startY, StudentWorld* world)
    : Actor(IID_WALL, startX, startY, right, 0, 1, world)
    {}
    
    virtual ~Wall() {}
    virtual void doSomething()  {return;}
    
};

class Penelope: public Actor
{
public:
    Penelope(double startX, double startY, StudentWorld* world)
    : Actor(IID_PLAYER, startX, startY, right, 0, 1, world)
    {}
    
    virtual ~Penelope() {}
    
    virtual void doSomething();
    
    bool checkBlocking(const Wall& wall) const;
};


#endif // ACTOR_H_
