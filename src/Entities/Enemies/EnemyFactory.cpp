#include "EnemyFactory.hpp"

#include <iostream>

#include "Common.hpp"
#include "Constants.hpp"
#include "Map/GameWorld.hpp"
#include "Entities/Enemies/PatrolEnemy.hpp"
#include "Entities/Enemies/FlyingEnemy.hpp"


EnemyFactory& EnemyFactory::instance() {
	static EnemyFactory instance;  // Meyer's singleton - thread-safe since C++11
	return instance;
}

EnemyFactory::EnemyFactory() : num_enemies_(0) {}

void EnemyFactory::create_enemy(const Ogre::String& name, EntityType enemyType, Ogre::Vector3 position) {
	std::string meshFile = (enemyType == EntityType::EnemyFlying ? "razor.mesh" : "robot.mesh");
	
	Ogre::Entity* enemyEntity = Common::scene_manager->createEntity(name, meshFile);
	
	// Fix material assignment - use our examples.material file in General group
	std::string materialName = (enemyType == EntityType::EnemyFlying ? "Examples/Razor" : "Examples/Robot");
	for (unsigned int i = 0; i < enemyEntity->getNumSubEntities(); ++i) {
		enemyEntity->getSubEntity(i)->setMaterialName(materialName, "General");
	}
	
	enemyEntity->setCastShadows(true);
	enemyEntity->setQueryFlags(static_cast<Ogre::uint32>(QueryFlags::Enemy));	

	Ogre::SceneNode* enemyNode = Common::scene_manager->getRootSceneNode()->createChildSceneNode(name + "Node", position);
	enemyNode->attachObject(enemyEntity);
    enemyNode->scale(Ogre::Vector3(5.0f));
    
	std::unique_ptr<Enemy> enemy;
	switch (enemyType) {
	    case EntityType::EnemyStationary:
		    enemy = std::make_unique<Enemy>(enemyEntity, enemyNode);
			break;

	    case EntityType::EnemyPatrol:
		    enemy = std::make_unique<PatrolEnemy>(enemyEntity, enemyNode);
			break;

	    case EntityType::EnemyFlying:
		    enemy = std::make_unique<FlyingEnemy>(enemyEntity, enemyNode);
			break;
	
        default: // nothing was created at this point, so no memory leaks
            throw Ogre::Exception(Ogre::Exception::ERR_INVALIDPARAMS, "Cannot construct desired type in EnemyFactory / not an enemy type", "Ascent");
    }

    enemy->set_type(enemyType);
    enemy->set_start_position(position);

	enemyNode->setUserAny(Ogre::Any(enemy.get()));
    
    // Transfer ownership to GameWorld
    GameWorld::instance().register_entity(std::move(enemy));
}
