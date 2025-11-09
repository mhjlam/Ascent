#include "PlayerProjectile.hpp"

#include "Common.hpp"
#include "Constants.hpp"

#include "Enemies/Enemy.hpp"
#include "Map/GameWorld.hpp"


PlayerProjectile::PlayerProjectile(Ogre::Entity* const entity, 
								   Ogre::SceneNode* const node, 
								   const Ogre::Vector3& direction, 
								   const Ogre::Quaternion& orientation)
: Projectile(entity, node, direction, orientation) {
	damage_ = 25;
	speed_ = 7500.0f;
	range_ = 5000.0f;
}
