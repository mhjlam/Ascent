#include "stdafx.h"

#include "../../Common.h"
#include "PatrolEnemy.h"
#include "../../map/GameWorld.h"

PatrolEnemy::PatrolEnemy(Ogre::Entity* entity, Ogre::SceneNode* node) : MovingEnemy(entity, node)
{
	mHealth = 100;

	// create patrol path, walk in a square
	Ogre::Vector3 nodePosition = mMainNode->getPosition();
	// move right
	mPatrolPoints.push_back(Ogre::Vector3(nodePosition.x + 500, nodePosition.y, nodePosition.z));
	// move back
	mPatrolPoints.push_back(Ogre::Vector3(nodePosition.x + 500, nodePosition.y, nodePosition.z - 500));
	// move left
	mPatrolPoints.push_back(Ogre::Vector3(nodePosition.x, nodePosition.y, nodePosition.z - 500));
	// move down (to original position)
	mPatrolPoints.push_back(Ogre::Vector3(nodePosition.x, nodePosition.y, nodePosition.z));

	// set movementspeed
	mSpeed = 100;
	// walk towards the first patrol point
	mCurrentTarget = 0;
	
	setTarget(mPatrolPoints[mCurrentTarget]);
}

void PatrolEnemy::update(const Ogre::Real& elapsed)
{
	Enemy::update(elapsed);

	// when not shooting, walk
	if (mAnimationState->getAnimationName() == "Walk")
	{
		Ogre::Real move = mSpeed * elapsed;
        mTargetDistance -= move;

		if (mTargetDistance <= 0.0f)
        {
            mMainNode->setPosition(mTarget);
			// if there's no new target, move to the first patrol point
			if (++mCurrentTarget >= (int)mPatrolPoints.size()) mCurrentTarget = 0;
			setTarget(mPatrolPoints[mCurrentTarget]);
		} 
		else 
		{
			mMainNode->translate(mDirection * move);
		}
	}
}

void PatrolEnemy::getDefaultAnimationState() 
{
	mAnimationState = mEntity->getAnimationState("Walk");
    mAnimationState->setLoop(true);
    mAnimationState->setEnabled(true);
}