#include "stdafx.h"
#include "EnemyFactory.h"

#include "../../Constants.h"
#include "../../Common.h"

#include "PatrolEnemy.h"
#include "FlyingEnemy.h"
#include "../../map/GameWorld.h"

EnemyFactory* EnemyFactory::mInstance = NULL;

EnemyFactory* EnemyFactory::getInstance() 
{
	if (mInstance == NULL) {
		mInstance = new EnemyFactory();
	}
	return mInstance;
}

EnemyFactory::EnemyFactory() : mEnemyCounter(0)
{}

Enemy* EnemyFactory::createEnemy(const Ogre::String& name, EntityType enemyType, Ogre::Vector3 position) 
{
	std::string meshFile = (enemyType == ENT_ENEMY_FLYING ? "razor.mesh" : "robot.mesh");

	Ogre::Entity* enemyEntity = Common::mSceneManager->createEntity(name, meshFile);
	enemyEntity->setCastShadows(true);
	enemyEntity->setQueryFlags(COLLIDABLE_ENEMY);	

	Ogre::SceneNode* enemyNode = Common::mSceneManager->getRootSceneNode()->createChildSceneNode(name + "Node", position);
	enemyNode->attachObject(enemyEntity);
    enemyNode->scale(Ogre::Vector3(5.0f));

    // and once for the overview
    Ogre::Entity* enemyOverview = Common::mSceneManagerOverview->createEntity(name, meshFile);
	enemyOverview->setCastShadows(false);
    enemyOverview->setMaterialName("TextureColors/Red");

    Ogre::SceneNode* nodeOverview = Common::mSceneManagerOverview->getRootSceneNode()->createChildSceneNode(name + "Node", position);
    nodeOverview->attachObject(enemyOverview);
    nodeOverview->scale(Ogre::Vector3(7.0f));

	Enemy* enemy = NULL;
	switch (enemyType)
	{
	    case ENT_ENEMY_STATIONARY:
		    enemy = new Enemy(enemyEntity, enemyNode);
		break;

	    case ENT_ENEMY_PATROL:
		    enemy = new PatrolEnemy(enemyEntity, enemyNode);
		break;

	    case ENT_ENEMY_FLYING:
		    enemy = new FlyingEnemy(enemyEntity, enemyNode);
		break;
	
        default:
            // nothing was created at this point, so no memory leaks
            throw Ogre::Exception(Ogre::Exception::ERR_INVALIDPARAMS, "Cannot construct desired type in EnemyFactory / not an enemy type", "Ascent");
        return NULL;
    }

    enemy->setType(enemyType);
    enemy->setStartPosition(position);

	enemyNode->setUserAny(Ogre::Any(enemy));
    GameWorld::getInstance()->registerEntity(enemy);

	return enemy;
}
