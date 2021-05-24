#ifndef _ENEMY_PROJECTILE_H_
#define _ENEMY_PROJECTILE_H_

#include "../../stdafx.h"
#include "Projectile.h"

/*
 *  Differs very little from base Projectile except for damage, speed and type
 */

class EnemyProjectile : public Projectile
{
public:
	EnemyProjectile(Ogre::Entity* const entity, Ogre::SceneNode* const node, const Ogre::Vector3& direction, const Ogre::Quaternion& orientation);

    void update(const Ogre::Real& elapsed);
};

#endif