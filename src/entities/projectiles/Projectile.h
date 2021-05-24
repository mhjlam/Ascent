#ifndef _PROJECTILE_H_
#define _PROJECTILE_H_

#include "../../stdafx.h"
#include "../../Constants.h"
#include "../GameEntity.h"

/*
 *  Basic projectile controller class
 */

class Projectile : public GameEntity
{
protected:
	Ogre::Quaternion	mOrientation;
	
	Ogre::uint			mDamage;
	Ogre::Real			mSpeed;
	Ogre::Real			mRange;

public: // ctor
    Projectile(Ogre::Entity* const entity, Ogre::SceneNode* const node, const Ogre::Vector3& direction, const Ogre::Quaternion& orientation);
	virtual ~Projectile() {};

    virtual void update(const Ogre::Real& elapsed);
};


#endif