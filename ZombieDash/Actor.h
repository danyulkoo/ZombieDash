#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Actor: public GraphObject
{
public:
    // Constructor
    Actor(int imageID, double startX, double startY, int dir, int depth, double size, StudentWorld* world, bool doesBlock)
    : GraphObject(imageID, startX, startY, dir, depth, size), m_world(world), m_isAlive(true), m_doesBlock(doesBlock)
    {}
    
    // Destructor
    virtual ~Actor() {}
    
    // Simple const functions
    StudentWorld* getWorld() const  {return m_world;}
    bool isAlive()           const  {return m_isAlive;}
    bool doesBlock()         const  {return m_doesBlock;}
    void setDead() {m_isAlive = false;}
    virtual void doSomething() = 0;
    
private:
    StudentWorld* m_world;
    bool m_isAlive;
    bool m_doesBlock;
};

class Wall: public Actor
{
public:
    Wall(double startX, double startY, StudentWorld* world)
    : Actor(IID_WALL, startX, startY, right, 0, 1, world, true)
    {}
    
    virtual ~Wall() {}
    virtual void doSomething()  {return;}
    
};

class Penelope: public Actor
{
public:
    Penelope(double startX, double startY, StudentWorld* world)
    : Actor(IID_PLAYER, startX, startY, right, 0, 1, world, true)
    {}
    
    virtual ~Penelope() {}
    
    virtual void doSomething();
    
};


#endif // ACTOR_H_
