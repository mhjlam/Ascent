#pragma once

#include "Projectile.hpp"

class EnemyProjectile : public Projectile {
public:
	EnemyProjectile(Ogre::Entity* const entity, 
                    Ogre::SceneNode* const node, 
                    const Ogre::Vector3& direction, 
                    const Ogre::Quaternion& orientation);

    void update(const Ogre::Real& elapsed);
};
