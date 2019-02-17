#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include "Level.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}

int StudentWorld::init()
{
    Level lev(assetPath());
    string levelFile = "level01.txt";

    Level::LoadResult result = lev.loadLevel(levelFile);
    if (result == Level::load_fail_file_not_found)
        cerr << "Cannot find level01.txt data file" << endl;
    else if (result == Level::load_fail_bad_format)
            cerr << "Your level was improperly formatted" << endl;
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
                        liActor.push_back(new Wall(SPRITE_WIDTH * i, SPRITE_HEIGHT * j, this));
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
    
    // make Penelope doSomething
    m_Penelope->doSomething();
    
    // make Actors doSomething
    list<Actor*>::iterator itrActor;
    for (itrActor = liActor.begin(); itrActor != liActor.end(); itrActor++)
        (*itrActor)->doSomething();
        
    decLives();
    return GWSTATUS_CONTINUE_GAME;
//    return GWSTATUS_PLAYER_DIED;
}

void StudentWorld::cleanUp()
{
    delete m_Penelope;
    
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
