#ifndef _PLAYER_PROJECTILE_H_
#define _PLAYER_PROJECTILE_H_

#include "../../stdafx.h"
#include "Projectile.h"

/*
 *  Differs very little from the base Projectile, except for damage, speed and type
 */

class PlayerProjectile : public Projectile
{
public:
	PlayerProjectile(Ogre::Entity* const entity, Ogre::SceneNode* const node, const Ogre::Vector3& direction, const Ogre::Quaternion& orientation);

};

#endif