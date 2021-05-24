#ifndef _GAME_WORLD_H_
#define _GAME_WORLD_H_

#include "../stdafx.h"

#include "Map.h"
#include "../entities/GameEntity.h"

/*
 *  GameWorld is responsible for parsing the Map files, creating the required objects
 *  updating them and and deleting them. This is a singleton class.
 */

class GameWorld
{
private:
    static GameWorld* mInstance;

    std::list<GameEntity*> mEntities;   // entities that require updating
    std::list<GameEntity*> mBrushes;   // brushes don't require updating, so they go in a separate list
    Map* mCurrentMap;

    bool mAbortUpdates; // stop the update functions
    
    // creates the scene by parsing the map
    void createScene();
    void createBrush(const BrushInfo* const brush);
    void createLight(const LightInfo* const light);
    void createEnemy(const EntityInfo* const enemy);
    void createPlayer(const EntityInfo* const player);

    // destroys all GameEntities and clears the scene
    void destroyScene();

public:  
    inline static GameWorld* const getInstance();
    inline static void destroySingleton();

    // this GameEntity was created outside of GameWorld (projectiles for instance) but must be managed
    void registerEntity(GameEntity* const entity);

    // manually destroy an Entity, also destroying its Ogre::Entity and the Ogre::SceneNode it is attached to
    void destroyEntity(GameEntity* const entity);

    // loading and clearing the Map
    void loadMap(Map* const map);
    void clearMap();

    // update all GameEntities stored in mEntities
    void update(const Ogre::Real& elapsed);

    // stops the update function from iterating any further, like when a new map has been loaded
    void abortUpdating();
};

// static stuff
inline GameWorld* const GameWorld::getInstance()
{
    if (!mInstance)
        mInstance = new GameWorld();
    
    return mInstance;
}

inline void GameWorld::destroySingleton()
{
    if (mInstance)
        mInstance->clearMap();

    delete mInstance;
    mInstance = NULL;
}

#endif
