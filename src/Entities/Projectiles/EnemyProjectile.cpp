#include "EnemyProjectile.hpp"

#include "Common.hpp"
#include "Constants.hpp"

#include "Entities/Player/Player.hpp"


EnemyProjectile::EnemyProjectile(Ogre::Entity* const entity, 
								 Ogre::SceneNode* const node, 
								 const Ogre::Vector3& direction, 
								 const Ogre::Quaternion& orientation)
: Projectile(entity, node, direction, orientation) {
	damage_ = 10;
	speed_ = 4000.0f;
	range_ = 10000.0f;
}

void EnemyProjectile::update(const Ogre::Real& elapsed) {
    // because the player doesn't have an entity, it has no hitboxes and can therefore not be raytraced
    if (get_position().squaredDistance(Common::player->get_position()) <= 200.0f*200.0f) {
		Common::player->hit(damage_);
        destroy();
	}
	else {
        Projectile::update(elapsed);
	}
}
