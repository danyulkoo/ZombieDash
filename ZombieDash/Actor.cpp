#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

///////////////////////////////////
/// ACITVATEABLE Implementation ///
///////////////////////////////////
void Activateable::doSomething()
{
    if (!isAlive())
        return;
    
    getWorld()->activateIfOverlapping(this);
}

/////////////////////////////////
/////  EXIT Implementation  /////
/////////////////////////////////
void Exit::activate(Actor* a)
{
    a->useExitIfAppropriate();
}

/////////////////////////////////
////  DAMAGER Implementation  ///
/////////////////////////////////
// This will be for flames and pit
void Damager::activate(Actor *a)
{
    a->getDamaged();
}

/////////////////////////////////
/// PROJECTILE Implementation ///
/////////////////////////////////
void Projectile::doSomething()
{
    if (!checkAliveAndTicks())
        return;

    Activateable::doSomething();
}

bool Projectile::checkAliveAndTicks()
{
    m_currTick++;
    
    if (!isAlive())
        return false;
    
    if (m_currTick == 2)
    {
        kill();
        return false;
    }
    
    return true;
}

/////////////////////////////////
/////  VOMIT Implementation  ////
/////////////////////////////////

// Unlike pit and flame, vomit does not kill
// It only infects people (so do not infect zombies)
void Vomit::activate(Actor *a)
{
    a->getInfectedByVomitIfAppropriate();
}

//////////////////////////////////
///// GOODIE Implementation //////
//////////////////////////////////
void Goodie::activate(Actor *a)
{
    a->pickUpGoodieIfAppropriate(this);
}

void Goodie::getDamaged()
{
    kill();
}

void VaccineGoodie::grantPenelopeSpecificGoodie(Penelope *p)
{
    p->addVaccine();
}

void GasCanGoodie::grantPenelopeSpecificGoodie(Penelope *p)
{
    p->addFlameCharges();
}

void LandmineGoodie::grantPenelopeSpecificGoodie(Penelope *p)
{
    p->addLandmine();
}

//////////////////////////////////
///// LANDMINE Implementation ////
//////////////////////////////////
void Landmine::activate(Actor *a)
{
    if (!m_isLandmineActive)
    {
        m_safetyTicks--;
        if (m_safetyTicks == 0)
            m_isLandmineActive = true;
        return;
    }
    
    a->triggerLandmineIfAppropriate(this);
}

void Landmine::getDamaged()
{
    // Prevent from endless back-and-forth since the flame from explosion
    // will be recognized as damaging the landmine.
    if (!isAlive())
        return;
    
    kill();
    explode(getX(), getY());
}

void Landmine::explode(double lx, double ly)
{
    getWorld()->playSound(SOUND_LANDMINE_EXPLODE);
    double coords[9][2] = {
        {lx,                ly},
        {lx,                ly + SPRITE_HEIGHT},
        {lx + SPRITE_WIDTH, ly + SPRITE_HEIGHT},
        {lx + SPRITE_WIDTH, ly                },
        {lx + SPRITE_WIDTH, ly - SPRITE_HEIGHT},
        {lx,                ly - SPRITE_HEIGHT},
        {lx - SPRITE_WIDTH, ly - SPRITE_HEIGHT},
        {lx - SPRITE_WIDTH, ly                },
        {lx - SPRITE_WIDTH, ly + SPRITE_HEIGHT}
    };
    
    for (int i = 0; i < 9; i++)
        getWorld()->addActor(new Flame(coords[i][0], coords[i][1], up, getWorld()));
    
    // Introduce Pit
    getWorld()->addActor(new Pit(lx, ly, getWorld()));
}
//////////////////////////////////
////// MOVER Implementation //////
//////////////////////////////////
bool Mover::executeMovementPlan(int dist)
{
    int dir = getDirection();
    double nx = getX();
    double ny = getY();
    
    if (dir == up)
        ny += dist;
    else if (dir == right)
        nx += dist;
    else if (dir == down)
        ny -= dist;
    else if (dir == left)
        nx -= dist;
    
    if (!getWorld()->isMoverBlocked(this, dist, dir))
    {
        moveTo(nx, ny);
        return true;
    }
    else
        return false;
}

void Mover::triggerLandmineIfAppropriate(Landmine* l)
{
    l->getDamaged();
}

void Mover::getDamaged()
{
    kill();
    
    if (isZombie())
        getWorld()->playSound(SOUND_ZOMBIE_DIE);
    if (isCitizen())
        getWorld()->playSound(SOUND_CITIZEN_DIE);
    
    getWorld()->recordCitizenOrZombieRemoved(this);
}

/////////////////////////////////
///// ZOMBIE Implementation /////
/////////////////////////////////
void Zombie::doSomething()
{
    if (!checkAliveAndParalysis())
        return;
    
    double vx = getX();
    double vy = getY();
    int z_dir = getDirection();
    
    switch (z_dir)
    {
        case up:
            vy += SPRITE_HEIGHT;
            break;
        case right:
            vx += SPRITE_WIDTH;
            break;
        case down:
            vy -= SPRITE_HEIGHT;
            break;
        case left:
            vx -= SPRITE_WIDTH;
            break;
        default:
            break;
    }
    
    if (getWorld()->isZombieTriggerAt(vx, vy))
    {
        double otherX = vx;
        double otherY = vy;
        double dist   = 0;
        if (getWorld()->locateNearestVomitTrigger(vx, vy, otherX, otherY, dist))
        {
            if (getWorld()->isOverlappingCoord(vx, vy, otherX, otherY))
            {
                if (randInt(1,3) == 1)
                {
                    getWorld()->addActor(new Vomit(vx, vy, z_dir, getWorld()));
                    getWorld()->playSound(SOUND_ZOMBIE_VOMIT);
                    return;
                }
            }
        }
    }
    
    if (movementPlan() == 0)
    {
        // Set random movement plan DISTANCE
        m_movementPlanDist = randInt(3, 10);
        // Set Zombie movement plan DIRECTION
        chooseNewMovementPlan();
    }
    
    if (executeMovementPlan(1))
        m_movementPlanDist--;
    else
        m_movementPlanDist = 0;
}

bool Zombie::checkAliveAndParalysis()
{
    // If Zombie is dead or if it is currently a paralysis tick, return false
    if (!isAlive() || getWorld()->ticks() % 2 == 0)
        return false;
    
    return true;
}

void DumbZombie::chooseNewMovementPlan()
{
    // Set dumb zombie's dir to a random direction
    int dirs[] = {up, right, left, down};
    setDirection(dirs[randInt(0, 3)]);
}

void SmartZombie::chooseNewMovementPlan()
{
    double zx = getX();
    double zy = getY();
    double nearestX = 0;
    double nearestY = 0;
    double nearestDist = 0;
    getWorld()->locateNearestVomitTrigger(zx, zy, nearestX, nearestY, nearestDist);
    
    setZombieTowardsPerson(nearestX, nearestY, nearestDist);
}

void SmartZombie::setZombieTowardsPerson(double px, double py, double dist)
{
    double zx = getX();
    double zy = getY();
    int dirs[] = {up, right, down, left};
    if (dist > 80)
        setDirection(dirs[randInt(0, 3)]);
    else
    {
        int n = randInt(1, 2);
        // If Zombie and Person are on same column
        if (zx == px)
            n = 1;
        // If Zombie and Person are on same row
        else if (zy == py)
            n = 2;
        
        // Vertical choice
        if (n == 1)
        {
            if (zy < py)
                setDirection(dirs[0]);
            else
                setDirection(dirs[2]);
        }
        // Horizontal choice
        else
        {
            if (zx < px)
                setDirection(dirs[1]);
            else
                setDirection(dirs[3]);
        }
    }
}
/////////////////////////////////
////  PERSON Implementation  ////
/////////////////////////////////
void Person::raiseInfectionCountIfInfected()
{
    if (isInfected())
        m_infectionCount++;
}

void Person::vaccinate()
{
    m_isInfected = false;
    m_infectionCount = 0;
}

bool Person::isFullyInfected() const
{
    return (m_infectionCount == 500 && m_isInfected);
}

void Person::getInfectedByVomitIfAppropriate()
{
    if (isCitizen() && !isInfected())
        getWorld()->playSound(SOUND_CITIZEN_INFECTED);
    
    m_isInfected = true;
}
/////////////////////////////////
//// PENELOPE Implementation ////
/////////////////////////////////
void Penelope::doSomething()
{
    if (!isAlive())
        return;
    
    raiseInfectionCountIfInfected();
    
    if (isFullyInfected())
    {
        kill();
        return;
    }
    
    int ch;
    if (getWorld()->getKey(ch))
    {
        switch (ch)
        {
            case KEY_PRESS_UP:
                setDirection(up);
                executeMovementPlan(4);
                break;
            case KEY_PRESS_DOWN:
                setDirection(down);
                executeMovementPlan(4);
                break;
            case KEY_PRESS_LEFT:
                setDirection(left);
                executeMovementPlan(4);
                break;
            case KEY_PRESS_RIGHT:
                setDirection(right);
                executeMovementPlan(4);
                break;
            case KEY_PRESS_SPACE:
                if (flameCharges() > 0)
                {
                    useFlamethrower();
                    m_flameCharges--;
                }
                break;
            case KEY_PRESS_TAB:
                if (landmines() > 0)
                {
                    dropNewLandmine();
                    m_landmines--;
                }
                break;
            case KEY_PRESS_ENTER:
                if (vaccines() > 0)
                {
                    vaccinate();
                    m_vaccines--;
                }
                break;
                
        }
    }
}

void Penelope::useExitIfAppropriate()
{
    if (getWorld()->numCitizens() > 0)
        return;
    
    kill();
    getWorld()->exitPenelope();
}

void Penelope::pickUpGoodieIfAppropriate(Goodie* g)
{
    g->grantPenelopeSpecificGoodie(this);
    g->kill();
    getWorld()->playSound(SOUND_GOT_GOODIE);
    getWorld()->increaseScore(50);
}

void Penelope::useFlamethrower()
{
    getWorld()->playSound(SOUND_PLAYER_FIRE);
    
    int x = getX();
    int y = getY();
    int dir = getDirection();
    
    for (int i = 1; i < 4; i++)
    {
        if (dir == up || dir == down)
        {
            int j = i;
            if (dir == down)
                j *= -1;
            
            if (getWorld()->willFlameOverlapWallExit(x, y + j*SPRITE_HEIGHT))
                break;
            
            getWorld()->addActor(new Flame(x, y + j*SPRITE_HEIGHT, dir, getWorld()));
        }
        
        if (dir == right || dir == left)
        {
            int j = i;
            if (dir == left)
                j *= -1;
            
            if (getWorld()->willFlameOverlapWallExit(x + j*SPRITE_WIDTH, y))
                break;
            
            getWorld()->addActor(new Flame(x + j*SPRITE_WIDTH, y, dir, getWorld()));
        }
    }
}

void Penelope::dropNewLandmine()
{
    getWorld()->addActor(new Landmine(getX(), getY(), getWorld()));
}

void Penelope::addLandmine()
{
    m_landmines += 2;
}

void Penelope::addFlameCharges()
{
    m_flameCharges += 5;
}

void Penelope::addVaccine()
{
    m_vaccines++;
}
////////////////////////////
// CITIZEN IMPLEMENTATION //
////////////////////////////
void Citizen::doSomething()
{
    // Check if Citizen is Alive
    if (!isAlive())
        return;
    
    raiseInfectionCountIfInfected();
    
    if (getWorld()->ticks() % 2 == 0)
        return;
    
    if (isFullyInfected())
    {
        // See on spec: Citizen (2)
        kill();
        getWorld()->playSound(SOUND_ZOMBIE_BORN);

        // Tell StudentWorld that a citizen turned into a zombie
        getWorld()->recordCitizenOrZombieRemoved(this);
        
        // Introduce a new zombie (70% Dumb Zombie, 30% Smart Zombie)
        int n = randInt(1, 10);
        if (n <= 7)
            getWorld()->addActor(new DumbZombie(getX(), getY(), getWorld()));
        else
            getWorld()->addActor(new SmartZombie(getX(), getY(), getWorld()));
        
        return;
    }

    if (getWorld()->ticks() % 2 == 0)
        return;
    
    double cx = getX();
    double cy = getY();
    double triggerX = 0;
    double triggerY = 0;
    double triggerDist = 0;
    bool isThreat = false;
    if (getWorld()->locateNearestCitizenTrigger(cx, cy, triggerX, triggerY, triggerDist, isThreat))
    {
        // If nearest Citizen trigger is PENELOPE
        if (!isThreat && triggerDist <= 80)
            if (setDirTowardsPenelope(triggerX, triggerY))
                return;
        
        // Otherwise, Citizen want's to run away from zombie
        if (isThreat && triggerDist <= 80)
        {
            int dirs[] = {up, right, down, left};
            double dists[] = {-1, -1, -1, -1};
            double optimalDist = triggerDist;
            int    optimalDir  = getDirection();
            
            double zx = 0;
            double zy = 0;
            
            if (!getWorld()->isMoverBlocked(this, 2, up))
                getWorld()->locateNearestCitizenThreat(cx, cy + 2, zx, zy, dists[0]);
            if (!getWorld()->isMoverBlocked(this, 2, right))
                getWorld()->locateNearestCitizenThreat(cx + 2, cy, zx, zy, dists[1]);
            if (!getWorld()->isMoverBlocked(this, 2, down))
                getWorld()->locateNearestCitizenThreat(cx, cy - 2, zx, zy, dists[2]);
            if (!getWorld()->isMoverBlocked(this, 2, left))
                getWorld()->locateNearestCitizenThreat(cx - 2, cy, zx, zy, dists[3]);
            
            bool foundFarther = false;
            for (int i = 0; i < 4; i++)
            {
                if (dists[i] > optimalDist)
                {
                    foundFarther = true;
                    optimalDist = dists[i];
                    optimalDir  = dirs[i];
                }
            }
            
            if (!foundFarther)
                return;
            
            setDirection(optimalDir);
            executeMovementPlan(2);
        }
    }
}

void Citizen::useExitIfAppropriate()
{
    kill();
    m_exitedSafely = true;
    getWorld()->playSound(SOUND_CITIZEN_SAVED);
    getWorld()->recordCitizenOrZombieRemoved(this);
}

bool Citizen::setDirTowardsPenelope(double px, double py)
{
    double cx = getX();
    double cy = getY();
    
    if (cx == px || cy == py)
    {
        // If citizen and Penelope are on the same COLUMN
        if (cx == px)
        {
            // If Penelope is ABOVE citizen
            if (cy < py && !getWorld()->isMoverBlocked(this, 2, up))
            {
                setDirection(up);
                moveTo(cx, cy + 2);
            }
            // If Penelope is BELOW citizen
            else if (cy > py && !getWorld()->isMoverBlocked(this, 2, down))
            {
                setDirection(down);
                moveTo(cx, cy - 2);
            }
            return true;
        }
        // If citizen and Penelope are on the same ROW
        else if (cy == py)
        {
            // If Penelope is to the RIGHT of citizen
            if (cx < px && !getWorld()->isMoverBlocked(this, 2, right))
            {
                setDirection(right);
                moveTo(cx + 2, cy);
            }
            // If Penelope is to the LEFT of citizen
            else if (cx > px && !getWorld()->isMoverBlocked(this, 2, left))
            {
                setDirection(left);
                moveTo(cx - 2, cy);
            }
            return true;
        }
        
        // Citizen was blocked in both directions
        return false;
    }
    
    // Citizen (6b) - If citizen is NOT on the same row or column as Penelope
    int h_dir, v_dir;
    if (cx != px || cy != py)
    {
        if (cx < px)
            h_dir = right;
        else
            h_dir = left;
        
        if (cy < py)
            v_dir = up;
        else
            v_dir = down;
        
        // Choose one random out of two dirs that will get citizen closer to Penelope
        // 1 = h_dir, 2 = v_dir
        int randDir = randInt(1, 2);
        int newDirs[2];
        if (randDir == 1)
        {
            newDirs[0]  = h_dir;
            newDirs[1] = v_dir;
        }
        else
        {
            newDirs[0]  = v_dir;
            newDirs[1] = h_dir;
        }
        
        for (int i = 0; i < 2; i++)
        {
            if (!getWorld()->isMoverBlocked(this, 2, newDirs[i]))
            {
                setDirection(newDirs[i]);
            
                if (newDirs[i] == right)
                    moveTo(cx + 2, cy);
                else if (newDirs[i] == left)
                    moveTo(cx - 2, cy);
                else if (newDirs[i] == up)
                    moveTo(cx, cy + 2);
                else if (newDirs[i] == down)
                    moveTo(cx, cy - 2);
                
                return true;
            }
        }
    }
    // Citizen was blocked in both directions
    return false;
}
