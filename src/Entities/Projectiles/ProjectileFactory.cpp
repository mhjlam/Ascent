
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

void ProjectileFactory::create_projectile(EntityType projectileType, 
                                                const Ogre::Vector3& position, 
                                                const Ogre::Vector3& target, 
                                                const Ogre::Quaternion& orientation, 
                                                bool targetIsDirection) {
	std::string meshFile = "sphere.mesh";
    
    std::unique_ptr<Projectile> projectile;
    Ogre::SceneNode* projectileNode = nullptr;
    Ogre::Entity* projectileEntity = nullptr;

    Ogre::Vector3 direction = targetIsDirection ? target : target - position;
    direction.normalise();

    std::string entityName, nodeName, lightName;

    switch (projectileType) {
	    case EntityType::EnemyProjectileType:
            entityName = "entity_enemy_projectile" + std::to_string(num_enemy_projectiles_);
            nodeName = "node_enemy_projectile" + std::to_string(num_enemy_projectiles_);
            lightName = "light_enemy_projectile" + std::to_string(num_enemy_projectiles_);
            num_enemy_projectiles_++;

            projectileEntity = Common::scene_manager->createEntity(entityName, meshFile);
            projectileNode = Common::scene_manager->createSceneNode(nodeName);
        
		    projectileNode->scale(0.5f, 0.5f, 0.5f);
	        projectileEntity->setMaterialName("TextureColors/Blue");
            projectileEntity->setCastShadows(false);
            projectileEntity->setRenderQueueGroup(Ogre::RENDER_QUEUE_MAX);  // Render last to avoid RTSS issues
	        projectileEntity->setQueryFlags(static_cast<Ogre::uint32>(QueryFlags::EnemyProjectile));

            projectile = std::make_unique<EnemyProjectile>(projectileEntity, projectileNode, direction, orientation);
		    break;

	    case EntityType::PlayerProjectileType:
            entityName = "entity_player_projectile" + std::to_string(num_player_projectiles_);
            nodeName = "node_player_projectile" + std::to_string(num_player_projectiles_);
            lightName = "light_player_projectile" + std::to_string(num_player_projectiles_);
            num_player_projectiles_++;

            projectileEntity = Common::scene_manager->createEntity(entityName, meshFile);
            projectileNode = Common::scene_manager->createSceneNode(nodeName);
		
            projectileNode->scale(0.15f, 0.05f, 3.0f);
	        projectileEntity->setMaterialName("TextureColors/Red");
	        projectileEntity->setCastShadows(false);
            projectileEntity->setRenderQueueGroup(Ogre::RENDER_QUEUE_MAX);  // Render last to avoid RTSS issues
	        projectileEntity->setQueryFlags(static_cast<Ogre::uint32>(QueryFlags::PlayerProjectile));

            projectile = std::make_unique<PlayerProjectile>(projectileEntity, projectileNode, direction, orientation);
	        break;

        default:
            // nothing was created at this point, so no memory leaks
            throw Ogre::Exception(Ogre::Exception::ERR_INVALIDPARAMS, "Cannot construct desired type in ProjectileFactory / not a projectile type", "Ascent");
	}

    projectile->set_type(projectileType);
    projectile->set_start_position(position);

    projectileNode->rotate(orientation, Ogre::Node::TS_WORLD);
    projectileNode->translate(position, Ogre::Node::TS_WORLD);
    projectileNode->setOrientation(orientation);
	
    projectileNode->attachObject(projectileEntity);
    projectileNode->setUserAny(Ogre::Any(projectile.get()));
    
    // Create billboard for plasma glow effect
    Ogre::BillboardSet* billboardSet = Common::scene_manager->createBillboardSet(nodeName + "_billboard", 1);
    billboardSet->setMaterialName("Plasma/Flare");
    billboardSet->setDefaultDimensions(projectileType == EntityType::EnemyProjectileType ? 3.0f : 2.0f, 
                                       projectileType == EntityType::EnemyProjectileType ? 3.0f : 2.0f);
    
    Ogre::Billboard* billboard = billboardSet->createBillboard(Ogre::Vector3::ZERO);
    billboard->setColour(projectileType == EntityType::EnemyProjectileType ? 
                         Ogre::ColourValue(0.3f, 0.8f, 2.5f) : 
                         Ogre::ColourValue(2.5f, 0.5f, 0.3f));
    
    projectileNode->attachObject(billboardSet);
    
    // Add to scene
	Common::scene_manager->getRootSceneNode()->addChild(projectileNode);

    // Transfer ownership to GameWorld
    GameWorld::instance().register_entity(std::move(projectile));
}