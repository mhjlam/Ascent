#include "Projectile.hpp"

#include "Common.hpp"
#include "Enemies/Enemy.hpp"


Projectile::Projectile(Ogre::Entity* const entity, 
                       Ogre::SceneNode* const node, 
                       const Ogre::Vector3& direction, 
                       const Ogre::Quaternion& orientation)
: GameEntity(entity, node)
, orientation_(orientation)
, damage_(0)
, speed_(0)
, range_(0) {
    set_direction(direction);
}

void Projectile::update(const Ogre::Real& elapsed) {
    // check for collision
    Ogre::Ray frontRay(get_position(), get_direction());

	Common::ray_scene_query->setRay(frontRay);
	Common::ray_scene_query->setSortByDistance(true, 5);
    Common::ray_scene_query->setQueryMask(static_cast<Ogre::uint32>(QueryFlags::Wall) | static_cast<Ogre::uint32>(QueryFlags::Enemy));
	
	Ogre::RaySceneQueryResult::iterator it;
	Ogre::RaySceneQueryResult& result = Common::ray_scene_query->execute();

	for (it = result.begin(); it != result.end(); ++it) {
		if (it->movable->getQueryFlags() == static_cast<Ogre::uint32>(QueryFlags::Wall) && it->distance <= 100.0f) { // hit a wall
			destroy();
			return;
		}
		else if (it->movable->getQueryFlags() == static_cast<Ogre::uint32>(QueryFlags::Enemy) && it->distance <= 300.0f) { // hit an enemy
            if (is_entity_type(EntityType::PlayerProjectileType)) {
                // damage enemy
                if (it->movable->isAttached()) { // this should always be the case, but check anyway
                    if (it->movable->getParentNode()->getUserAny().isEmpty() == false) {
                        // sadly, this cast doesn't support abstract types, so all info about any subclasses of Enemy is lost
                        Enemy* const enemy = Ogre::any_cast<Enemy*>((it->movable->getParentNode()->getUserAny()));
                        if (enemy) { 
                            enemy->hit(damage_);
                        }
                    }
                }

                destroy();
			    return;
            }
        } 
	}

    // if it didn't hit anything, move in the specified direction
    Ogre::Vector3 distance = get_direction() * (speed_ * elapsed);
    main_node_->translate(distance, Ogre::Node::TS_WORLD);

    // check if the projectile has reached its max range
    if (main_node_->getPosition().squaredDistance(start_position_) > range_*range_) {
        destroy();
    }
}
