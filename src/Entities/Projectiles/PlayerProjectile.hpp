#pragma once

#include "Projectile.hpp"

class PlayerProjectile : public Projectile {
public:
	PlayerProjectile(Ogre::Entity* const entity, 
					 Ogre::SceneNode* const node, 
					 const Ogre::Vector3& direction, 
					 const Ogre::Quaternion& orientation);
};
