#ifndef _PROJECTILE_FACTORY_H_
#define _PROJECTILE_FACTORY_H_

#include "../../stdafx.h"
#include "../../Constants.h"
#include "Projectile.h"

/*
 *  Constructs the Projectiles based on the type specified
 */

class ProjectileFactory 
{
private:
	ProjectileFactory();

private:
	static ProjectileFactory*	mInstance;
	Ogre::uint					mPlayerProjectileCounter;
	Ogre::uint					mEnemyProjectileCounter;

public: 
	static ProjectileFactory* getInstance();

public:
	Projectile* createProjectile(EntityType projectileType, const Ogre::Vector3& position, const Ogre::Vector3& target, const Ogre::Quaternion& orientation, bool targetIsDirection = false);
	
	Ogre::uint getNumEnemyProjectiles()		{ return mEnemyProjectileCounter; }
	Ogre::uint getNumPlayerProjectiles()	{ return mPlayerProjectileCounter; }
};


#endif