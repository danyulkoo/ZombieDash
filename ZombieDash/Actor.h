#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;
class Mover;
class Person;
class Penelope;
class Goodie;
class Landmine;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Actor: public GraphObject
{
public:
    // Constructor
    Actor(int imageID, double startX, double startY, int dir, int depth, StudentWorld* world)
    : GraphObject(imageID, startX, startY, dir, depth), m_world(world), m_isAlive(true)
    {}
    
    // Destructor
    virtual ~Actor() {}
    
    // Simple const functions
    StudentWorld* getWorld() const  {return m_world;}
    bool isAlive()           const  {return m_isAlive;}
    
    // Virtual functions
    virtual void doSomething() = 0;
    virtual bool doesBlockMovers()  const {return false;}
    virtual bool blocksFlames()     const {return false;}
    virtual bool isCitizen()        const {return false;}
    virtual bool isZombie()         const {return false;}
    virtual bool dropsVaccine()     const {return false;}
    
    // Activateable functions
    virtual void kill() {m_isAlive = false;}
    virtual void useExitIfAppropriate() {}
    virtual void pickUpGoodieIfAppropriate(Goodie* g) {}
    virtual void triggerLandmineIfAppropriate(Landmine* l) {}
    virtual void getDamaged() {}
    virtual void getInfectedByVomitIfAppropriate() {}
    
private:
    StudentWorld* m_world;
    bool m_isAlive;
};

/* WALL DECLARATION */
class Wall: public Actor
{
public:
    Wall(double startX, double startY, StudentWorld* world)
    : Actor(IID_WALL, startX, startY, right, 0, world)
    {}
    
    virtual ~Wall() {}
    
    virtual bool doesBlockMovers()  const {return true;}
    virtual bool blocksFlames() const {return true;}
    virtual void doSomething()  {}
    
    // Can't kill a wall
    virtual void kill() {}
};

/* ACTIVATEABLE DECLARATION */
class Activateable: public Actor
{
public:
    Activateable(int imageID, double startX, double startY, int dir, int depth, StudentWorld* world)
    : Actor(imageID, startX, startY, dir, depth, world)
    {}
    
    virtual ~Activateable() {}

    virtual void doSomething();
    virtual void activate(Actor* a) = 0;
};

/* EXIT DECLARATION */
class Exit: public Activateable
{
public:
    // Constructor
    Exit(double startX, double startY, StudentWorld* world)
    : Activateable(IID_EXIT, startX, startY, right, 1, world)
    {}
    
    // Destructor
    virtual ~Exit() {}
    
    virtual bool blocksFlames() const {return true;}
    
    virtual void activate(Actor* a);
    // Can't kill an exit
    virtual void kill() {}
};

/* DAMAGER DECLARATION */
class Damager: public Activateable
{
public:
    Damager(int imageID, double startX, double startY, int dir, int depth, StudentWorld* world)
    : Activateable(imageID, startX, startY, dir, depth, world)
    {}
    
    virtual ~Damager() {}
    
    virtual void activate(Actor* a);
};

/* PROJECTILE IMPLEMENTATION */
class Projectile: public Damager
{
public:
    Projectile(int imageID, double startX, double startY, int dir, StudentWorld* world)
    : Damager(imageID, startX, startY, dir, 0, world), m_currTick(0)
    {}
    
    virtual ~Projectile() {}
    
    virtual void doSomething();
    bool checkAliveAndTicks();
    
private:
    int m_currTick;
};

class Flame: public Projectile
{
public:
    Flame(double startX, double startY, int dir, StudentWorld* world)
    : Projectile(IID_FLAME, startX, startY, dir, world)
    {}
    
    virtual ~Flame() {}
    
};

class Vomit: public Projectile
{
public:
    Vomit(double startX, double startY, int dir, StudentWorld* world)
    : Projectile(IID_VOMIT, startX, startY, dir ,world)
    {}
    
    virtual ~Vomit() {}
    
    virtual void activate(Actor* a);

};

/* PIT DECLARATION */
class Pit: public Damager
{
public:
    Pit(double startX, double startY, StudentWorld* world)
    : Damager(IID_PIT, startX, startY, right, 0, world)
    {}
    
    virtual ~Pit() {}
    
};

/* GOODIE DECLARATION */
class Goodie: public Activateable
{
public:
    Goodie(int imageID, double startX, double startY, StudentWorld* world)
    : Activateable(imageID, startX, startY, right, 1, world)
    {}
    
    virtual ~Goodie() {}
    
    virtual void getDamaged();
    virtual void activate(Actor* a);
    virtual void grantPenelopeSpecificGoodie(Penelope* p) = 0;
};

class VaccineGoodie: public Goodie
{
public:
    VaccineGoodie(double startX, double startY, StudentWorld* world)
    : Goodie(IID_VACCINE_GOODIE, startX, startY, world)
    {}
    
    virtual ~VaccineGoodie() {}
    virtual void grantPenelopeSpecificGoodie(Penelope* p);
};

class GasCanGoodie: public Goodie
{
public:
    GasCanGoodie(double startX, double startY, StudentWorld* world)
    : Goodie(IID_GAS_CAN_GOODIE, startX, startY, world)
    {}
    
    virtual ~GasCanGoodie() {}
    virtual void grantPenelopeSpecificGoodie(Penelope* p);
};

class LandmineGoodie: public Goodie
{
public:
    LandmineGoodie(double startX, double startY, StudentWorld* world)
    : Goodie(IID_LANDMINE_GOODIE, startX, startY, world)
    {}
    
    virtual ~LandmineGoodie() {}
    virtual void grantPenelopeSpecificGoodie(Penelope* p);
};

/* LANDMINE DECLARATION */
class Landmine: public Activateable
{
public:
    Landmine(double startX, double startY, StudentWorld* world)
    : Activateable(IID_LANDMINE, startX, startY, right, 1, world), m_safetyTicks(30), m_isLandmineActive(false)
    {}
    
    virtual ~Landmine() {}
    
    virtual void getDamaged();
    virtual void activate(Actor* a);
    
    void explode(double lx, double ly);
    
private:
    int m_safetyTicks;
    bool m_isLandmineActive;
};

/* MOVER DECLARATION */
class Mover: public Actor
{
public:
    Mover(int imageID, double startX, double startY, int dir, int depth, StudentWorld* world)
    : Actor(imageID, startX, startY, dir, depth, world)
    {}
    
    virtual ~Mover() {}
    virtual bool isFullyInfected() const {return false;}
    virtual bool doesBlockMovers() const {return true;}
    virtual bool exitedSafely()    const {return false;}
    
    virtual void triggerLandmineIfAppropriate(Landmine* l);
    virtual void getDamaged();
    
    bool executeMovementPlan(int dist);
};

/* ZOMBIE DECLARATION */
class Zombie: public Mover
{
public:
    Zombie(double startX, double startY, StudentWorld* world)
    : Mover(IID_ZOMBIE, startX, startY, right, 0, world), m_movementPlanDist(0)
    {}
    
    virtual ~Zombie() {}
    
    virtual void doSomething();
    virtual bool isZombie() const {return true;}
    virtual void chooseNewMovementPlan() = 0;
    
    int movementPlan() const {return m_movementPlanDist;}
    
private:
    bool checkAliveAndParalysis();
    int m_movementPlanDist;
};

class DumbZombie: public Zombie
{
public:
    DumbZombie(double startX, double startY, StudentWorld* world)
    : Zombie(startX, startY, world)
    {}
    
    virtual ~DumbZombie() {}
    
    virtual bool dropsVaccine() const {return true;}
    virtual void chooseNewMovementPlan();
    
};

class SmartZombie: public Zombie
{
public:
    SmartZombie(double startX, double startY, StudentWorld* world)
    : Zombie(startX, startY, world)
    {}
    
    virtual ~SmartZombie() {}
    
    virtual void chooseNewMovementPlan();
    
private:
    void setZombieTowardsPerson(double px, double py, double dist);
};

/* PERSON DECLARATION */
class Person: public Mover
{
public:
    Person(int imageID, double startX, double startY, StudentWorld* world)
    : Mover(imageID, startX, startY, right, 0, world), m_infectionCount(0), m_isInfected(false)
    {}
    
    virtual ~Person() {}
    
    virtual void getInfectedByVomitIfAppropriate();
    virtual void vaccinate();
    
    int  infectionCount() const {return m_infectionCount;}
    bool isInfected()     const {return m_isInfected;}
    bool isFullyInfected() const;
    void raiseInfectionCountIfInfected();
    
private:
    int  m_infectionCount;
    bool m_isInfected;
};

/* PENELOPE DECLARATION*/
class Penelope: public Person
{
public:
    Penelope(double startX, double startY, StudentWorld* world)
    : Person(IID_PLAYER, startX, startY, world), m_landmines(0), m_flameCharges(0), m_vaccines(0)
    {}
    
    virtual ~Penelope() {}
    
    virtual void doSomething();
    virtual void useExitIfAppropriate();
    virtual void pickUpGoodieIfAppropriate(Goodie* g);
    
    int landmines()     const {return m_landmines;}
    int flameCharges()  const {return m_flameCharges;}
    int vaccines()      const {return m_vaccines;}
    
    void addLandmine();
    void addFlameCharges();
    void addVaccine();
private:
    int m_landmines;
    int m_flameCharges;
    int m_vaccines;
    
    void useFlamethrower();
    void dropNewLandmine();
};

/* CITIZEN DECLARATION */
class Citizen: public Person
{
public:
    Citizen(double startX, double startY, StudentWorld* world)
    : Person(IID_CITIZEN, startX, startY, world), m_exitedSafely(false)
    {}
    
    virtual ~Citizen() {}
    
    virtual void doSomething();
    virtual bool isCitizen() const {return true;}
    virtual bool exitedSafely() const {return m_exitedSafely;}
    virtual void useExitIfAppropriate();
    virtual void vaccinate() {}
    
    
private:
    bool m_exitedSafely;
    bool setDirTowardsPenelope(double px, double py);
};

#endif // ACTOR_H_
