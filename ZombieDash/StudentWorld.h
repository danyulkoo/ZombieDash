#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <list>
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class Actor;
class Activateable;
class Mover;
class Penelope;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    // Push back a new Actor object into the StudentWorld's list<Actor*>
    void addActor(Actor* a);
    
    // Simple Const return functions
    int  numCitizens()    const {return m_numCitizens;}
    int  ticks()          const {return m_ticks;}
    
    /* GAME FUNCTIONS ACTORS CAN USE */
    
    // If an Activateable is overlapping with an actor, activate
    void activateIfOverlapping(Activateable* a);
    
    // Is there a human that will trigger Zombie at (x,y)
    bool isZombieTriggerAt(double x, double y) const;
    
    // Return true if there is a living human, otherwise false.  If true,
    // otherX, otherY, and distance will be set to the location and distance
    // of the human nearest to (x,y).
    bool locateNearestVomitTrigger(double x, double y, double& otherX, double& otherY, double& distance);
    
    // Return true if there is a living zombie or Penelope, otherwise false.
    // If true, otherX, otherY, and distance will be set to the location and
    // distance of the one nearest to (x,y), and isThreat will be set to true
    // if it's a zombie, false if a Penelope.
    bool locateNearestCitizenTrigger(double x, double y, double& otherX, double& otherY, double& distance, bool& isThreat) const;
    
    // Return true if there is a living zombie, false otherwise.  If true,
    // otherX, otherY and distance will be set to the location and distance
    // of the one nearest to (x,y).
    bool locateNearestCitizenThreat(double x, double y, double& otherX, double& otherY, double& distance) const;
    
    // Auxilary functions
    bool isOverlappingCoord(double x1, double y1, double x2, double y2) const;
    double getEucledianDistCoord(double x1, double y1, double x2, double y2) const;
    
    // Checks if an Actor is blocked
    bool isMoverBlocked(const Actor* actor, double dist, int a_dir) const;
    
    // Checks if flame will overlap with a Wall or Exit
    bool willFlameOverlapWallExit(double fx, double fy) const;
    
    // Adds or takes points based on event
    void exitPenelope();
    void recordCitizenOrZombieRemoved(Mover* m);
    

private:
    /* MEMBER DATA */
    Penelope* m_Penelope;
    std::list<Actor*> liActor;
    bool m_levelCompleted;
    int  m_numCitizens;
    int  m_ticks;
    
    /* MEMBER FUNCTIONS*/
    double px() const;
    double py() const;
    int p_dir() const;
    
    bool didPenelopeFailLevel() const;
    void dumbZombieThrowGoodie(Mover* z);
    bool isOverlapping(const Actor* a, const Actor* b) const;
    bool isWithinHeight(double a_bot, double a_top, double b_bot, double b_top) const;
    bool isWithinWidth(double a_left, double a_right, double b_left, double b_right) const;
    
    // Game functions
    void resetLevel() {m_levelCompleted = false;}
    void completeLevel() {m_levelCompleted = true;}
    void removeDeadActors();
    void DisplayText();
    
};
#endif // STUDENTWORLD_H_
