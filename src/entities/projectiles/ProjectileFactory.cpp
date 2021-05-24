#include "stdafx.h"
#include "ProjectileFactory.h"

#include "../../Common.h"
#include "../../Constants.h"

#include "EnemyProjectile.h"
#include "PlayerProjectile.h"
#include "../../map/GameWorld.h"

ProjectileFactory* ProjectileFactory::mInstance = NULL;

ProjectileFactory::ProjectileFactory() : mEnemyProjectileCounter(0), mPlayerProjectileCounter(0)
{
}

Projectile* ProjectileFactory::createProjectile(EntityType projectileType, const Ogre::Vector3& position, const Ogre::Vector3& target, const Ogre::Quaternion& orientation, bool targetIsDirection)
{
	std::string meshFile = "sphere.mesh";
    
    std::stringstream en, nn;
	en << "entity_player_projectile" << mEnemyProjectileCounter++;
	nn << "node_player_projectile" << mPlayerProjectileCounter++;
	
    Projectile* projectile = NULL;
    Ogre::SceneNode* projectileNode = NULL;
    Ogre::Entity* projectileEntity = NULL;

    Ogre::Vector3 direction = targetIsDirection ? target : target - position;
    direction.normalise();

    switch (projectileType)
	{
	    case ENT_PROJECTILE_ENEMY:
            projectileEntity = Common::mSceneManager->createEntity(nn.str(), meshFile);
            projectileNode = Common::mSceneManager->createSceneNode(nn.str() + "_node");
        
		    projectileNode->scale(0.5f, 0.5f, 0.5f);
	        projectileEntity->setMaterialName("TextureColors/Blue");
            projectileEntity->setCastShadows(false);
	        projectileEntity->setQueryFlags(COLLIDABLE_ENEMY_PROJECTILE);

            projectile = new EnemyProjectile(projectileEntity, projectileNode, direction, orientation);
		break;

	    case ENT_PROJECTILE_PLAYER:
            projectileEntity = Common::mSceneManager->createEntity(en.str(), meshFile);
            projectileNode = Common::mSceneManager->createSceneNode(en.str() + "_node");
		
            projectileNode->scale(0.15f, 0.05f, 3.0f);
	        projectileEntity->setMaterialName("TextureColors/Red");
	        projectileEntity->setCastShadows(false);
	        projectileEntity->setQueryFlags(COLLIDABLE_PLAYER_PROJECTILE);

            projectile = new PlayerProjectile(projectileEntity, projectileNode, direction, orientation);
	    break;

        default:
            // nothing was created at this point, so no memory leaks
            throw Ogre::Exception(Ogre::Exception::ERR_INVALIDPARAMS, "Cannot construct desired type in ProjectileFactory / not a projectile type", "Ascent");
        return NULL;
	}

    projectile->setType(projectileType);
    projectile->setStartPosition(position);

    projectileNode->rotate(orientation, Ogre::Node::TS_WORLD);
    projectileNode->translate(position, Ogre::Node::TS_WORLD);
    projectileNode->setOrientation(orientation);
	
    projectileNode->attachObject(projectileEntity);
    projectileNode->setUserAny(Ogre::Any(projectile));
	Common::mSceneManager->getRootSceneNode()->addChild(projectileNode);

    GameWorld::getInstance()->registerEntity(projectile);

	return projectile;
}

ProjectileFactory* ProjectileFactory::getInstance() 
{
	if (mInstance == NULL)
		mInstance = new ProjectileFactory();

	return mInstance;
}