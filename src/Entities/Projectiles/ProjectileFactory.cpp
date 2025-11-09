
#include "ProjectileFactory.hpp"

#include "Entities/Projectiles/EnemyProjectile.hpp"
#include "Entities/Projectiles/PlayerProjectile.hpp"

#include "Common.hpp"
#include "Constants.hpp"
#include "Map/GameWorld.hpp"

ProjectileFactory::ProjectileFactory() 
: num_enemy_projectiles_(0)
, num_player_projectiles_(0) {}

ProjectileFactory& ProjectileFactory::instance() {
	static ProjectileFactory instance;  // Meyer's singleton - thread-safe since C++11
	return instance;
}

void ProjectileFactory::create_projectile(EntityType projectile_type, 
                                          const Ogre::Vector3& position, 
                                          const Ogre::Vector3& target, 
                                          const Ogre::Quaternion& orientation, 
                                          bool target_is_direction) {
	std::string mesh_file = "sphere.mesh";
    
    std::unique_ptr<Projectile> projectile;
    Ogre::SceneNode* projectile_node = nullptr;
    Ogre::Entity* projectile_entity = nullptr;

    Ogre::Vector3 direction = target_is_direction ? target : target - position;
    direction.normalise();

    std::string entity_name;
    std::string node_name;
    std::string light_name;

    switch (projectile_type) {
	    case EntityType::EnemyProjectileType:
            entity_name = "entity_enemy_projectile" + std::to_string(num_enemy_projectiles_);
            node_name = "node_enemy_projectile" + std::to_string(num_enemy_projectiles_);
            light_name = "light_enemy_projectile" + std::to_string(num_enemy_projectiles_);
            num_enemy_projectiles_++;

            projectile_entity = Common::scene_manager->createEntity(entity_name, mesh_file);
            projectile_node = Common::scene_manager->createSceneNode(node_name);
        
		    projectile_node->scale(0.5f, 0.5f, 0.5f);
	        projectile_entity->setMaterialName("TextureColors/Blue");
            projectile_entity->setCastShadows(false);
            projectile_entity->setRenderQueueGroup(Ogre::RENDER_QUEUE_MAIN);  // Render in main queue before overlays
	        projectile_entity->setQueryFlags(static_cast<Ogre::uint32>(QueryFlags::EnemyProjectile));

            projectile = std::make_unique<EnemyProjectile>(projectile_entity, projectile_node, direction, orientation);
		    break;

	    case EntityType::PlayerProjectileType:
            entity_name = "entity_player_projectile" + std::to_string(num_player_projectiles_);
            node_name = "node_player_projectile" + std::to_string(num_player_projectiles_);
            light_name = "light_player_projectile" + std::to_string(num_player_projectiles_);
            num_player_projectiles_++;

            projectile_entity = Common::scene_manager->createEntity(entity_name, mesh_file);
            projectile_node = Common::scene_manager->createSceneNode(node_name);
		
            projectile_node->scale(0.15f, 0.05f, 3.0f);
	        projectile_entity->setMaterialName("TextureColors/Red");
	        projectile_entity->setCastShadows(false);
            projectile_entity->setRenderQueueGroup(Ogre::RENDER_QUEUE_MAIN);  // Render in main queue before overlays
	        projectile_entity->setQueryFlags(static_cast<Ogre::uint32>(QueryFlags::PlayerProjectile));

            projectile = std::make_unique<PlayerProjectile>(projectile_entity, projectile_node, direction, orientation);
	        break;

        default:
            // nothing was created at this point, so no memory leaks
            throw Ogre::Exception(Ogre::Exception::ERR_INVALIDPARAMS, 
                "Cannot construct desired type in ProjectileFactory / not a projectile type", "Ascent");
	}

    projectile->set_type(projectile_type);
    projectile->set_start_position(position);

    projectile_node->rotate(orientation, Ogre::Node::TS_WORLD);
    projectile_node->translate(position, Ogre::Node::TS_WORLD);
    projectile_node->setOrientation(orientation);
	
    projectile_node->attachObject(projectile_entity);
    projectile_node->setUserAny(Ogre::Any(projectile.get()));
    
    // Create billboard for plasma glow effect
    Ogre::BillboardSet* billboard_set = Common::scene_manager->createBillboardSet(node_name + "_billboard", 1);
    billboard_set->setMaterialName("Plasma/Flare");
    billboard_set->setDefaultDimensions(projectile_type == EntityType::EnemyProjectileType ? 3.0f : 2.0f, 
                                       projectile_type == EntityType::EnemyProjectileType ? 3.0f : 2.0f);
    
    Ogre::Billboard* billboard = billboard_set->createBillboard(Ogre::Vector3::ZERO);
    billboard->setColour(projectile_type == EntityType::EnemyProjectileType ? 
                         Ogre::ColourValue(0.3f, 0.8f, 2.5f) : 
                         Ogre::ColourValue(2.5f, 0.5f, 0.3f));
    
    projectile_node->attachObject(billboard_set);
    
    // Add to scene
	Common::scene_manager->getRootSceneNode()->addChild(projectile_node);

    // Transfer ownership to GameWorld
    GameWorld::instance().register_entity(std::move(projectile));
}
