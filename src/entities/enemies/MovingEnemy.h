#ifndef MOVINGENEMY_H_
#define MOVINGENEMY_H_

#include "../../stdafx.h"
#include "Enemy.h"

/*
 *  Base class for moving enemies to help dealing with movement
 */

class MovingEnemy : public Enemy 
{
protected:
	Ogre::Real		mSpeed;
	Ogre::Vector3	mTarget;
	Ogre::uint		mCurrentTarget;
	Ogre::Vector3	mDirection;
	Ogre::Real		mTargetDistance;

protected:
	void setTarget(Ogre::Vector3);
	virtual void getDefaultAnimationState() = 0;
	
public:
	MovingEnemy(Ogre::Entity* entity, Ogre::SceneNode* node);

    void update(const Ogre::Real& elapsed);
};

#endif