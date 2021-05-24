#include "stdafx.h"
#include "GameWorld.h"

#include "../Common.h"
#include "../entities/enemies/EnemyFactory.h"
#include "../entities/player/Player.h"
#include "../entities/BrushEntity.h"

GameWorld* GameWorld::mInstance = NULL;

/*
 *  Create the scene. Uses mCurrentMap for retrieving the data.
 */
void GameWorld::createScene()
{
    Common::mSceneManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE); // the performance hit is compensated by using fog
    Common::mSceneManagerOverview->setShadowTechnique(Ogre::SHADOWTYPE_NONE); // overview needs no fancy shadows

    // read map data
    const MapInfo& mapData = mCurrentMap->getMapInfo();
    Common::mSceneManager->setAmbientLight(mapData.ambientLight);
    Common::mSceneManagerOverview->setAmbientLight(Ogre::ColourValue::White);
    
    if (mapData.fogEnabled)
    {
        Common::mWindow->getViewport(0)->setBackgroundColour(mapData.fogColour);
        Common::mSceneManager->setFog(Ogre::FOG_LINEAR, mapData.fogColour, 0.0, mapData.fogStart, mapData.fogEnd);
    } 
    else
    {
        Common::mWindow->getViewport(0)->setBackgroundColour(Ogre::ColourValue::Black);
        Common::mSceneManager->setFog(); // sufficient to reset properties (defaults to FOG_NONE)
    }
    
    
    // load details from Map file and start creating
    const std::vector<BrushInfo*>& brushes = mCurrentMap->getBrushes();
    const std::vector<LightInfo*>& lights = mCurrentMap->getLights();
    const std::vector<EntityInfo*>& entities = mCurrentMap->getEntities();

    // the order in which they are loaded is not important, so we can do this in O(n) time instead of O(3n) by using:
    size_t max = std::max(std::max(lights.size(), brushes.size()), entities.size());
 
    for (size_t i = 0; i < max; ++i)
    {
        if (i < brushes.size())
            createBrush(brushes.at(i));
        
        if (i < lights.size())
            createLight(lights.at(i));

        if (i < entities.size())
        {
            switch (entities.at(i)->type)
            {
                case (ENT_PLAYER):
                    createPlayer(entities.at(i));
                break;

                case (ENT_ENEMY_FLYING):
                case (ENT_ENEMY_STATIONARY):
                case (ENT_ENEMY_PATROL):
                    createEnemy(entities.at(i));
                break;

                default:
                break;
            }
        }
    }
}

/*
 *  Creates a PointLight. Note that no GameEntity is created for a light.
 */
void GameWorld::createLight(const LightInfo* const light)
{
    Ogre::Light* pointLight = Common::mSceneManager->createLight(light->id);
    pointLight->setType(Ogre::Light::LT_POINT);
    pointLight->setPosition(light->position);
    pointLight->setDiffuseColour(light->colour);
    pointLight->setSpecularColour(light->colour);
    
    Ogre::SceneNode* node = Common::mSceneManager->getRootSceneNode()->createChildSceneNode();
    node->attachObject(pointLight);
}

/*
 *  Creates a 'Brush' which is basically a prefab cube mesh scaled and textured. 
 *  A BrushEntity class is set as the UserAny of the scene node.
 *  Be careful not to use 'type', use 'brushType'.
 */
void GameWorld::createBrush(const BrushInfo* const brush)
{
    // create Entity based on a prefab
    Ogre::Entity* brushEntity = Common::mSceneManager->createEntity(brush->id, Ogre::SceneManager::PT_CUBE);
    brushEntity->setMaterialName(brush->materialName);
    brushEntity->setCastShadows(brush->castShadows);
    brushEntity->setQueryFlags(COLLIDABLE_WALL);
    
    // create SceneNode
    Ogre::SceneNode* node = Common::mSceneManager->getRootSceneNode()->createChildSceneNode();
    node->attachObject(brushEntity);
    
    // the brush's position is its 'start' point, so we need to compute the center of the brush
    node->setPosition(Ogre::Vector3(brush->position.x + brush->dimensions.x / 2.0f,
                                    brush->position.y + brush->dimensions.y / 2.0f,
                                    brush->position.z + brush->dimensions.z / 2.0f));
    // prefab cube is 100x100x100, so we need to resize it to match the requested coordinates
    node->setScale(brush->dimensions * 0.01f);

    BrushEntity* brushUser = new BrushEntity(brushEntity, node, brush->brushType);
    brushUser->setType(ENT_BRUSH);
    node->setUserAny(Ogre::Any(brushUser));
    
    // we put brushes in a separate list because we don't want to iterate over them every time the game is updated
    // however, BrushEntities are needed and therefor must be created (and destroyed!) so we do keep track of them
    mBrushes.push_back(brushUser);
    
    // overview (this is duplicate code deluxe, but I currently see no other way as Ogre::Entities cannot be shared by different SceneNodes)
    if (brush->brushType != BRUSH_CEIL && brush->brushType != BRUSH_FLOOR) // do not add ceilings and walls (this is for a topdown view after all)
    {
        Ogre::Entity* brushOverview = Common::mSceneManagerOverview->createEntity(brush->id, Ogre::SceneManager::PT_CUBE);\
        brushOverview->setMaterialName(brush->materialName);
        brushOverview->setCastShadows(false); // no lights, no shadows
        
        Ogre::SceneNode* nodeOverview = Common::mSceneManagerOverview->getRootSceneNode()->createChildSceneNode();
        nodeOverview->showBoundingBox(true);
        nodeOverview->attachObject(brushOverview);
        
        // hide all brushes except for the start and ending pads
        if (brush->type != BRUSH_EXIT && brush->type != BRUSH_START)
            nodeOverview->setVisible(false);
    
        nodeOverview->setPosition(Ogre::Vector3(brush->position.x + brush->dimensions.x / 2.0f,
                                        brush->position.y + brush->dimensions.y / 2.0f,
                                        brush->position.z + brush->dimensions.z / 2.0f));
        nodeOverview->setScale(brush->dimensions * 0.01f);
    }
}

/*
 *  Creates an Enemy by calling the EnemyFactory. 
 *  The Enemy created is automatically registered (factory calls our registerEntity function)
 */
void GameWorld::createEnemy(const EntityInfo* const enemy)
{
	if (!Common::mState.isDead(enemy->id)) // when loading a game, this enemy should not be created if it was dead
	{	
        try
        {   EnemyFactory::getInstance()->createEnemy(enemy->id, enemy->type, enemy->position);  }
        catch (Ogre::Exception& e)
        {   
            // occurs if the enemy->type is not a valid type to be constructed by the EnemyFactory
            // not sure how to print to Ogre's log, parameters are a mystery to me
            e.what(); // suppress compiler warning
        }
    }
}

/*
 *  Creates and sets up the player
 */
void GameWorld::createPlayer(const EntityInfo* player)
{
    Ogre::SceneNode* const playerNode = Common::mSceneManager->getRootSceneNode()->createChildSceneNode(player->id + "_node");
    
    Common::mPlayer = new Player(playerNode);
    registerEntity(Common::mPlayer);
    
    Common::mPlayer->setType(ENT_PLAYER);
    Common::mPlayer->setPosition(player->position);
	Common::mPlayer->setStartPosition(player->position);
    Common::mPlayer->setDirection(player->direction);
}

/*
 *  Destroys all custom GameEntities and lets Ogre clear/destroy the rest. 
 */
void GameWorld::destroyScene()
{
    abortUpdating(); // halt updating
    
    // destroy our GameEntities
    std::for_each(mEntities.begin(), mEntities.end(), Ascent::delete_ptr());
    mEntities.clear();   

    // destroy our BrushEntities
    std::for_each(mBrushes.begin(), mBrushes.end(), Ascent::delete_ptr());
    mBrushes.clear();
     
    // tell Ogre to destroy all entities, lights and scenenodes
    Common::mSceneManager->clearScene(); 
    Common::mSceneManagerOverview->clearScene();

    mCurrentMap = NULL;
}

/*
 *  Tells GameWorld to reconstruct the scene using the given map. 
 */
void GameWorld::loadMap(Map* const map)
{
    if (map)
    {
        destroyScene();
        mCurrentMap = map;
        createScene();
    }
}

/*
 *  Clears the map, making it empty. 
 */
void GameWorld::clearMap()
{
    if (mCurrentMap)
        destroyScene();
}

/*
 *  Register this GameEntity so we can update and delete it when neccesary.
 *  This is mostly used by the factory classes.
 */
void GameWorld::registerEntity(GameEntity* const entity)
{
    mEntities.push_back(entity);
}

/*
 *  Manually destroy this GameEntity. 
 */
void GameWorld::destroyEntity(GameEntity* const entity)
{
    std::list<GameEntity*>::iterator it = mEntities.begin();

    while (it != mEntities.end())
    {
        if (*it == entity)
        {
            delete *it;
            mEntities.erase(it); // note: invalidates any iterator in use (particularly the one in update())

            break;
        }
        
        ++it;
    }
}

/*
 *  Allow the GameEntities in the world to update themselves by calling their update function.
 */
void GameWorld::update(const Ogre::Real& elapsed)
{
    if (mAbortUpdates)
    {
        mAbortUpdates = false; // reset
        return;
    }

    if (mCurrentMap)
    {
        std::list<GameEntity*>::iterator it = mEntities.begin();
        Ogre::uint i = 0; // used for manually resetting the iterator when required

        while (it != mEntities.end())
        {
            if (*it != NULL)
            {
                if ((*it)->isAlive())
                { 
                    (*it)->update(elapsed); // might kill the object, so it will be removed the next time our update is called
                    
                    if (mAbortUpdates)  // stop this instant, the iterator is invalid as the scene has been cleared or something similar
                        return;
                    
                    ++it;
                    ++i;

                    continue; // all is fine, continue with the next entity
                } 
            }
            
            // if all is not fine (entity is NULL or dead), remove it
            destroyEntity(*it);

            // reset the iterator (see destroyEntity for details)
            it = mEntities.begin();

            // the item is removed from the list, so we want to go back to i
            std::advance(it, i);     
        }
    }
}

void GameWorld::abortUpdating()
{
    mAbortUpdates = true;
}