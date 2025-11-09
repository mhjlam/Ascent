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


EnemyFactory::EnemyFactory() 
: num_enemies_(0) {
}


void EnemyFactory::create_enemy(const Ogre::String& name, EntityType enemy_type, Ogre::Vector3 position) {
	std::string mesh_file = (enemy_type == EntityType::EnemyFlying ? "razor.mesh" : "robot.mesh");
	
	Ogre::Entity* enemy_entity = Common::scene_manager->createEntity(name, mesh_file);
	
	// Fix material assignment - use our examples.material file in General group
	std::string material_name = (enemy_type == EntityType::EnemyFlying ? "Examples/Razor" : "Examples/Robot");
	for (unsigned int i = 0; i < enemy_entity->getNumSubEntities(); ++i) {
		enemy_entity->getSubEntity(i)->setMaterialName(material_name, "General");
	}
	
	enemy_entity->setCastShadows(true);
	enemy_entity->setQueryFlags(static_cast<Ogre::uint32>(QueryFlags::Enemy));	

	Ogre::SceneNode* enemy_node = Common::scene_manager->getRootSceneNode()->createChildSceneNode(name + "Node", position);
	enemy_node->attachObject(enemy_entity);
    enemy_node->scale(Ogre::Vector3(5.0f));
    
	std::unique_ptr<Enemy> enemy;
	switch (enemy_type) {
	    case EntityType::EnemyStationary:
		    enemy = std::make_unique<Enemy>(enemy_entity, enemy_node);
			break;

	    case EntityType::EnemyPatrol:
		    enemy = std::make_unique<PatrolEnemy>(enemy_entity, enemy_node);
			break;

	    case EntityType::EnemyFlying:
		    enemy = std::make_unique<FlyingEnemy>(enemy_entity, enemy_node);
			break;
	
        default: // nothing was created at this point, so no memory leaks
            throw Ogre::Exception(Ogre::Exception::ERR_INVALIDPARAMS, "Cannot construct desired type in EnemyFactory / not an enemy type", "Ascent");
    }

    enemy->set_type(enemy_type);
    enemy->set_start_position(position);

	enemy_node->setUserAny(Ogre::Any(enemy.get()));
    
    // Transfer ownership to GameWorld
    GameWorld::instance().register_entity(std::move(enemy));
}
