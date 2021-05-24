#include "stdafx.h"
#include "PlayerProjectile.h"

#include "../../Constants.h"
#include "../../Common.h"
#include "../enemies/Enemy.h"
#include "../../map/GameWorld.h"

PlayerProjectile::PlayerProjectile(Ogre::Entity* const entity, Ogre::SceneNode* const node, const Ogre::Vector3& direction, const Ogre::Quaternion& orientation) : 
Projectile(entity, node, direction, orientation)
{
	mDamage = 25;
	mSpeed = 7500.0f;
	mRange = 5000.0f;
}