#include "stdafx.h"
#include "MovingEnemy.h"

MovingEnemy::MovingEnemy(Ogre::Entity* entity, Ogre::SceneNode* node) : Enemy(entity, node) 
{

}

void MovingEnemy::setTarget(Ogre::Vector3 target)
{
	mTarget = target;
	mDirection = mTarget - mMainNode->getPosition();
	mTargetDistance = mDirection.normalise();
}

void MovingEnemy::update(const Ogre::Real& elapsed)
{
    Enemy::update(elapsed);
}
