#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include "GraphObject.h"
#include "Level.h"

#include <cmath>
#include <algorithm>
#include <vector>

#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), m_Penelope(nullptr), m_levelCompleted(false), m_numCitizens(0), m_ticks(0)
{
}

int StudentWorld::init()
{
    // Reset levelCompleted to false
    resetLevel();
    
    Level lev(assetPath());
    stringstream levelFile;
    levelFile << "level" << getLevel()/10 << getLevel()%10 << ".txt";

    Level::LoadResult result = lev.loadLevel(levelFile.str());
    if (result == Level::load_fail_file_not_found)
        return GWSTATUS_PLAYER_WON;
    else if (result == Level::load_fail_bad_format)
        return GWSTATUS_LEVEL_ERROR;
    else if (result == Level::load_success)
    {
        cerr << "Successfully loaded level" << endl;
        
        Level::MazeEntry ge;
        for (int i = 0; i < LEVEL_WIDTH; i++)
        {
            for (int j = 0; j < LEVEL_HEIGHT; j++)
            {
                ge = lev.getContentsOf(i, j);
                switch (ge)
                {
                    case Level::empty:
                        break;
                    case Level::player:
                        m_Penelope = new Penelope(SPRITE_WIDTH * i, SPRITE_HEIGHT * j, this);
                        break;
                    case Level::wall:
                        addActor(new Wall(SPRITE_WIDTH * i, SPRITE_HEIGHT * j, this));
                        break;
                    case Level::exit:
                        addActor(new Exit(SPRITE_WIDTH * i, SPRITE_HEIGHT * j, this));
                        break;
                    case Level::pit:
                        addActor(new Pit(SPRITE_WIDTH * i, SPRITE_HEIGHT * j, this));
                        break;
                    case Level::citizen:
                        addActor(new Citizen(SPRITE_WIDTH * i, SPRITE_HEIGHT * j, this));
                        m_numCitizens++;
                        break;
                    case Level::dumb_zombie:
                        addActor(new DumbZombie(SPRITE_WIDTH * i, SPRITE_HEIGHT * j, this));
                        break;
                    case Level::smart_zombie:
                        addActor(new SmartZombie(SPRITE_WIDTH * i, SPRITE_HEIGHT * j, this));
                        break;
                    case Level::vaccine_goodie:
                        addActor(new VaccineGoodie(SPRITE_WIDTH * i, SPRITE_HEIGHT * j, this));
                        break;
                    case Level::gas_can_goodie:
                        addActor(new GasCanGoodie(SPRITE_WIDTH * i, SPRITE_HEIGHT * j, this));
                        break;
                    case Level::landmine_goodie:
                        addActor(new LandmineGoodie(SPRITE_WIDTH * i, SPRITE_HEIGHT * j, this));
                        break;
                    default:
                        break;
                }
            }
            
        }
    }

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    
    // Increase tick count
    m_ticks++;
    
    // make Penelope doSomething
    m_Penelope->doSomething();
    
    // make Actors doSomething
    list<Actor*>::iterator itrActor;
    for (itrActor = liActor.begin(); itrActor != liActor.end(); itrActor++)
    {
        if ((*itrActor)->isAlive())
            (*itrActor)->doSomething();
        
        // If Penelope completed the Level
        if (m_levelCompleted)
        {
            playSound(SOUND_LEVEL_FINISHED);
            return GWSTATUS_FINISHED_LEVEL;
        }
        
        if (didPenelopeFailLevel())
        {
            decLives();
            playSound(SOUND_PLAYER_DIE);
            removeDeadActors();
            return GWSTATUS_PLAYER_DIED;
        }
    }
    
    // get rid of dead actors
    removeDeadActors();
    
    // update game display text
    DisplayText();
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    if (m_Penelope != nullptr)
    {
        delete m_Penelope;
        m_Penelope = nullptr;
    }
    
    list<Actor*>::iterator itrActor;
    
    // Free all Actors
    itrActor = liActor.begin();
    while (itrActor != liActor.end())
    {
        delete *itrActor;
        itrActor = liActor.erase(itrActor);
    }
    
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}


/////////////////////////////
// PUBLIC MEMBER FUNCTIONS //
/////////////////////////////
// Check if Activateable is overlapping with a Mover/Person
void StudentWorld::activateIfOverlapping(Activateable *a)
{
    if (isOverlapping(a, m_Penelope))
        a->activate(m_Penelope);
    
    list<Actor*>::iterator itr = liActor.begin();
    
    for (itr = liActor.begin(); itr != liActor.end(); itr++)
    {
        // If activateable overlaps with a mover, activate
        if (isOverlapping(a, (*itr)))
            a->activate(*itr);
    }
}

bool StudentWorld::isZombieTriggerAt(double x, double y) const
{
    if (x == px() && y == py())
        return true;
    
    list<Actor*>::const_iterator itr;
    for (itr = liActor.begin(); itr != liActor.end(); itr++)
        if ((*itr)->getX() == x && (*itr)->getY())
            return true;
    
    return false;
}

bool StudentWorld::locateNearestVomitTrigger(double x, double y, double& otherX, double& otherY, double& distance)
{
    bool thereIsLivingHuman = m_Penelope->isAlive();
    double nearestX = px();
    double nearestY = py();
    double nearestDist = getEucledianDistCoord(x, y, px(), py());
    
    list<Actor*>::iterator itr;
    for (itr = liActor.begin(); itr != liActor.end(); itr++)
    {
        if ((*itr)->isAlive() && (*itr)->isCitizen())
        {
            thereIsLivingHuman = true;
            double tmp = getEucledianDistCoord(x, y, (*itr)->getX(), (*itr)->getY());
            if (tmp < nearestDist)
            {
                nearestDist = tmp;
                nearestX = (*itr)->getX();
                nearestY = (*itr)->getY();
            }
        }
    }
    
    if (thereIsLivingHuman)
    {
        otherX = nearestX;
        otherY = nearestY;
        distance = nearestDist;
        return true;
    }
    
    return false;
}

bool StudentWorld::locateNearestCitizenTrigger(double x, double y, double& otherX, double& otherY, double& distance, bool& isThreat) const
{
    bool thereIsLivingPenOrZombie = m_Penelope->isAlive();
    bool triggerIsThreat = false;
    double nearestX = px();
    double nearestY = py();
    double nearestDist = getEucledianDistCoord(x, y, px(), py());
    
    list<Actor*>::const_iterator itr;
    for (itr = liActor.begin(); itr != liActor.end(); itr++)
    {
        if ((*itr)->isZombie() && (*itr)->isAlive())
        {
            thereIsLivingPenOrZombie = true;
            double tmp = getEucledianDistCoord(x, y, (*itr)->getX(), (*itr)->getY());
            if (tmp < nearestDist)
            {
                triggerIsThreat = true;
                nearestDist = tmp;
                nearestX = (*itr)->getX();
                nearestY = (*itr)->getY();
            }
        }
    }
    
    if (thereIsLivingPenOrZombie)
    {
        otherX = nearestX;
        otherY = nearestY;
        distance = nearestDist;
        isThreat = triggerIsThreat;
        return true;
    }
    
    return false;
}

bool StudentWorld::locateNearestCitizenThreat(double x, double y, double& otherX, double& otherY, double& distance) const
{
    bool thereIsLivingZombie = false;
    double nearestX = 0;
    double nearestY = 0;
    double nearestDist = 0;
    
    list<Actor*>::const_iterator itr;
    for (itr = liActor.begin(); itr != liActor.end(); itr++)
    {
        if ((*itr)->isZombie() && (*itr)->isAlive())
        {
            thereIsLivingZombie = true;
            nearestX = (*itr)->getX();
            nearestY = (*itr)->getY();
            nearestDist = getEucledianDistCoord(x, y, (*itr)->getX(), (*itr)->getY());
            break;
        }
    }
    
    while (itr != liActor.end())
    {
        if ((*itr)->isZombie() && (*itr)->isAlive())
        {
            thereIsLivingZombie = true;
            double tmp = getEucledianDistCoord(x, y, (*itr)->getX(), (*itr)->getY());
            if (tmp < nearestDist)
                nearestDist = tmp;
        }
        itr++;
    }
    
    if (thereIsLivingZombie)
    {
        otherX = nearestX;
        otherY = nearestY;
        distance = nearestDist;
        return true;
    }
    
    return false;
}
//////////////////////////////
///// AUXILARY FUNCTIONS /////
//////////////////////////////

bool StudentWorld::isMoverBlocked(const Actor* actor, double dist, int a_dir) const
{
    double a_bot    = actor->getY();
    double a_top    = actor->getY() + SPRITE_HEIGHT - 1;
    double a_left   = actor->getX();
    double a_right  = actor->getX() + SPRITE_WIDTH - 1;
    
    double b_bot;
    double b_top;
    double b_left;
    double b_right;
    
    // Check Penelope
    if (actor != m_Penelope)
    {
        b_bot   = m_Penelope->getY();
        b_top   = m_Penelope->getY() + SPRITE_HEIGHT - 1;
        b_left  = m_Penelope->getX();
        b_right = m_Penelope->getX() + SPRITE_WIDTH - 1;
        
        if (a_dir == actor->left)
        {
            // Ignore if Penelope is on the actor's right
            if (b_right < a_left)
            {
                // if actor would move past blocking object's right AND actor is within the height
                // of blocking object
                if (a_left - dist <= b_right && isWithinHeight(a_bot, a_top, b_bot, b_top))
                    return true;
            }
        }
        else if (a_dir == actor->right)
        {
            // Ignore if blocking object is on the actor's left
            if (b_left > a_right)
            {
                // if actor would move past blocking object's left AND actor is within the height
                // of blocking object
                if (a_right + dist >= b_left && isWithinHeight(a_bot, a_top, b_bot, b_top))
                    return true;
            }
        }
        else if (a_dir == actor->up)
        {
            if (b_bot > a_top)
            {
                if (a_top + dist >= b_bot && isWithinWidth(a_left, a_right, b_left, b_right))
                    return true;
            }
        }
        else if (a_dir == actor->down)
        {
            if (b_top < a_bot)
            {
                if (a_bot - dist <= b_top && isWithinWidth(a_left, a_right, b_left, b_right))
                    return true;
            }
        }
    }
    
    // Check other Actors
    list<Actor*>::const_iterator itr;
    for (itr = liActor.begin(); itr != liActor.end(); itr++)
    {
        if ((*itr)->doesBlockMovers())
        {
            b_bot    = (*itr)->getY();
            b_top    = (*itr)->getY() + SPRITE_HEIGHT - 1;
            b_left   = (*itr)->getX();
            b_right  = (*itr)->getX() + SPRITE_WIDTH - 1;
            
            if (a_dir == actor->left)
            {
                // Ignore if blocking object is on the actor's right
                if (b_right >= a_left)
                    continue;
                // if actor would move past blocking object's right AND actor is within the height
                // of blocking object
                if (a_left - dist <= b_right && isWithinHeight(a_bot, a_top, b_bot, b_top))
                    return true;
            }
            else if (a_dir == actor->right)
            {
                // Ignore if blocking object is on the actor's left
                if (b_left <= a_right)
                    continue;
                // if actor would move past blocking object's left AND actor is within the height
                // of blocking object
                if (a_right + dist >= b_left && isWithinHeight(a_bot, a_top, b_bot, b_top))
                    return true;
            }
            else if (a_dir == actor->up)
            {
                if (b_bot <= a_top)
                    continue;
                if (a_top + dist >= b_bot && isWithinWidth(a_left, a_right, b_left, b_right))
                    return true;
            }
            else if (a_dir == actor->down)
            {
                if (b_top >= a_bot)
                    continue;
                if (a_bot - dist <= b_top && isWithinWidth(a_left, a_right, b_left, b_right))
                    return true;
                
            }
        }
    }
    
    return false;
}

bool StudentWorld::willFlameOverlapWallExit(double fx, double fy) const
{
    list<Actor*>::const_iterator itr;
    for (itr = liActor.begin(); itr != liActor.end(); itr++)
    {
        if (!(*itr)->blocksFlames())
            continue;
        
        double ix = (*itr)->getX();
        double iy = (*itr)->getY();
        
        if (isOverlappingCoord(fx, fy, ix, iy))
            return true;
    }
    
    return false;
    
}

void StudentWorld::exitPenelope()
{
    completeLevel();
}

void StudentWorld::recordCitizenOrZombieRemoved(Mover* m)
{
    if (m->isZombie())
    {
        int pts = 2000;
        if (m->dropsVaccine())
        {
            pts = 1000;
            if (randInt(1, 10) == 1)
                dumbZombieThrowGoodie(m);
            
        }
        increaseScore(pts);
    }
    
    if (m->isCitizen())
    {
        
        int pts = 500;
        if (!m->exitedSafely())
            pts = -1000;
        
        increaseScore(pts);
        m_numCitizens--;
    }
}

//////////////////////////////
// PRIVATE MEMBER FUNCTIONS //
//////////////////////////////
double StudentWorld::px() const
{
    if (m_Penelope != nullptr)
        return m_Penelope->getX();
    else
        return -1;
}

double StudentWorld::py() const
{
    if (m_Penelope != nullptr)
        return m_Penelope->getY();
    else
        return -1;
}

int StudentWorld::p_dir() const
{
    if (m_Penelope != nullptr)
        return m_Penelope->getDirection();
    else
        return -1;
}

void StudentWorld::addActor(Actor* a)
{
    liActor.push_back(a);
}

void StudentWorld::removeDeadActors()
{
    if (m_Penelope != nullptr && !m_Penelope->isAlive())
    {
        delete m_Penelope;
        m_Penelope = nullptr;
    }
    
    // Free all dead Actors
    list<Actor*>::iterator itr = liActor.begin();
    itr = liActor.begin();
    
    while (itr != liActor.end())
    {
        if ((*itr) != nullptr && !(*itr)->isAlive())
        {
            delete *itr;
            itr = liActor.erase(itr);
        }
        else
            itr++;
    }
    
}

bool StudentWorld::didPenelopeFailLevel() const
{
    return (!m_Penelope->isAlive() || m_Penelope->isFullyInfected());
}

void StudentWorld::dumbZombieThrowGoodie(Mover *z)
{
    int n = randInt(0, 3);
    
    double x = z->getX();
    double y = z->getY();
    
    switch (n)
    {
        case 0:
            y += SPRITE_HEIGHT;
            break;
        case 1:
            x += SPRITE_WIDTH;
            break;
        case 2:
            y -= SPRITE_HEIGHT;
            break;
        case 3:
            x -= SPRITE_WIDTH;
            break;
    }
    
    list<Actor*>::iterator itr;
    for (itr = liActor.begin(); itr != liActor.end(); itr++)
        if (isOverlappingCoord(x, y, (*itr)->getX(), (*itr)->getY()))
            return;
    
    addActor(new VaccineGoodie(x, y, this));
}

bool StudentWorld::isOverlapping(const Actor* a, const Actor* b) const
{
    if (a == b)
        return false;
    
    double dist = getEucledianDistCoord(a->getX(), a->getY(), b->getX(), b->getY());
    
    return (dist >= 0 && dist <= 10);
}

bool StudentWorld::isOverlappingCoord(double x1, double y1, double x2, double y2) const
{
    double dist = getEucledianDistCoord(x1, y1, x2, y2);
    
    return (dist >= 0 && dist <= 10);
}

double StudentWorld::getEucledianDistCoord(double x1, double y1, double x2, double y2) const
{
    // Check Actor 1 center coordinates
    x1 = x1 + (SPRITE_WIDTH/2);
    y1 = y1 + (SPRITE_HEIGHT/2);
    // Check Actor 2 center coordinates
    x2 = x2 + (SPRITE_WIDTH/2);
    y2 = y2 + (SPRITE_HEIGHT/2);
    
    return sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
}

bool StudentWorld::isWithinHeight(double a_bot, double a_top, double b_bot, double b_top) const
{
    return ((a_bot >= b_bot && a_bot <= b_top) || (a_top >= b_bot && a_top <= b_top));
}

bool StudentWorld::isWithinWidth(double a_left, double a_right, double b_left, double b_right) const
{
    return ((a_left >= b_left && a_left <= b_right) || (a_right >= b_left && a_right <= b_right));
}

void StudentWorld::DisplayText()
{
    if (m_Penelope == nullptr)
        return;
    
    ostringstream oss;
    
    oss << "Score: ";
    oss << getScore() << "  ";
    
    oss << "Level: ";
    oss << getLevel() << "  ";
    
    oss << "Lives: ";
    oss << getLives() << "  ";
    
    oss << "Vaccines: ";
    oss << m_Penelope->vaccines() << "  ";
    
    oss << "Flames: ";
    oss << m_Penelope->flameCharges() << "  ";
    
    oss << "Mines: ";
    oss << m_Penelope->landmines() << "  ";
    
    oss << "Infected: ";
    oss << m_Penelope->infectionCount() << "  ";
    
    setGameStatText(oss.str());
}
