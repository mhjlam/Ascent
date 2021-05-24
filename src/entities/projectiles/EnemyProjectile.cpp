#include "stdafx.h"
#include "EnemyProjectile.h"

#include "../../Constants.h"
#include "../../Common.h"

EnemyProjectile::EnemyProjectile(Ogre::Entity* const entity, Ogre::SceneNode* const node, const Ogre::Vector3& direction, const Ogre::Quaternion& orientation) : 
Projectile(entity, node, direction, orientation)
{
	mDamage = 10;
	mSpeed = 2500.0f;
	mRange = 10000.0f;
}

void EnemyProjectile::update(const Ogre::Real& elapsed)
{
    // because the player doesn't have an entity, it has no hitboxes and can therefore not be raytraced
    if (getPosition().squaredDistance(Common::mPlayer->getPosition()) <= 200.0f*200.0f)
	{
		Common::mPlayer->hit(mDamage);
        destroyMe();
	} else
        Projectile::update(elapsed);
}