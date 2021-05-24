#include "stdafx.h"
#include "FlyingEnemy.h"

#include <time.h>
#include "../../Common.h"

#include "../projectiles/ProjectileFactory.h"
#include "../../map/GameWorld.h"

FlyingEnemy::FlyingEnemy(Ogre::Entity* entity, Ogre::SceneNode* node) : MovingEnemy(entity, node) 
{
	srand((unsigned int)time(NULL));

	mMovementTimer = 0;
	mSpeed = 1000;
	mHealth = 150;
	setTarget(getPosition());
}

void FlyingEnemy::update(const Ogre::Real& elapsed) 
{
	// don't call Enemy's update function here, as it will crash due to this class not having a mAnimationState

    mMovementTimer += elapsed;

	if (mMovementTimer >= 2) {
		mMovementTimer = 0;

		Ogre::Vector3 target = getPosition();
		
		// make a dodging motion in a random direction
		Ogre::uint num = rand() % 6;

		if (num < 1)
			target.y += 200;
		else if (num < 2)
			target.y -= 200;
		else if (num < 3)
			target.x += 200;
		else if (num < 4)
			target.x -= 200;
		else if (num < 5)
			target.x += 200;
		else if (num < 6)
			target.x -= 200;

		setTarget(target);
	} 
	else 
	{
		Ogre::Real move = mSpeed * elapsed;
		mTargetDistance -= move;

		if (mTargetDistance <= 0.0f)
			moveTo(mTarget);
		else
			translate(mDirection * move);
	}

	// enemy lookat camera
	getNode()->lookAt(Common::mPlayer->getPosition(), Ogre::Node::TS_WORLD);

	// yaw 180 degrees so the ship looks at the player from the front instead of from the back
	getNode()->yaw(Ogre::Radian(Ogre::Math::PI));

	mFireTimer += elapsed;

	if (mFireTimer >= 3) 
        fireProjectileAt(Common::mPlayer);
}

void FlyingEnemy::getDefaultAnimationState() 
{
	// this mesh has no animations
}
