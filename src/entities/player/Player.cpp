#include "stdafx.h"
#include "Player.h"

#include "../../Common.h"
#include "../../Constants.h"

#include "../projectiles/Projectile.h"
#include "../projectiles/ProjectileFactory.h"
#include "../BrushEntity.h"
#include "../../game/Game.h"

Player::Player(Ogre::SceneNode* const playerNode): GameEntity(NULL, playerNode),
mMovingForward(false), mMovingBackward(false), mMovingLeft(false), mMovingRight(false), mMovingUp(false), mMovingDown(false), 
mRollingLeft(false), mRollingRight(false), mShooting(false),
mTopMovingSpeed(35.0f), mTopRotationSpeed(80.0f), mVelocity(Ogre::Vector3::ZERO), mHealth(100), mCredits(0), mShootTimer(0.0f), mTurretType(TURRET_DOUBLE)
{
	// player node
    mMainNode = Common::mSceneManager->getRootSceneNode()->createChildSceneNode();
	mMainNode->attachObject(Common::mCamera);

	// turret nodes
	mTurretNodeCenter = mMainNode->createChildSceneNode("node_player_turret_center", Ogre::Vector3(0.0f, -100.0f, 0.0f));
	mTurretNodeLeft	  = mMainNode->createChildSceneNode("node_player_turret_left", Ogre::Vector3(-150.0f, -50.0f, 0.0f));
	mTurretNodeRight  = mMainNode->createChildSceneNode("node_player_turret_right", Ogre::Vector3(150.0f, -50.0f, 0.0f));

	// head lights (do not seem to work properly)
	//mHeadLightCenter = Common::mSceneManager->createLight("light_player_turret_center");
	//mHeadLightCenter->setType(Ogre::Light::LT_SPOTLIGHT);
	//mHeadLightCenter->setDiffuseColour(1, 1, 1);
	//mHeadLightCenter->setSpecularColour(1, 1, 1);
	//mHeadLightCenter->setPosition(0, 200, 200);
	//mHeadLightCenter->setSpotlightRange(Ogre::Radian(Ogre::Degree(45)), Ogre::Radian(Ogre::Degree(75)), 5.0f);
	//mHeadLightCenter->setCastShadows(true);
	//mMainNode->attachObject(mHeadLightCenter);
}

void Player::update(const Ogre::Real& elapsed)
{
	mShootTimer -= elapsed;

	if (mShooting && mShootTimer <= 0.0f)
	{
		mShootTimer = 0.5f;
		shoot();
	}

	accelerate(elapsed);
	roll(elapsed);
	detectCollisions();
}

void Player::injectKeyDown(const OIS::KeyEvent& evt)
{
	switch (evt.key)
	{
		case OIS::KC_W:			mMovingForward	= true;		break;
		case OIS::KC_S:			mMovingBackward = true;		break;
		case OIS::KC_A:			mMovingLeft		= true;		break;
		case OIS::KC_D:			mMovingRight	= true;		break;
		case OIS::KC_SPACE:		mMovingUp		= true;		break;
		case OIS::KC_LCONTROL:	mMovingDown		= true;		break;

		case OIS::KC_Q:			mRollingLeft	= true;		break;
		case OIS::KC_E:			mRollingRight	= true;		break;
	}
}

void Player::injectKeyUp(const OIS::KeyEvent& evt)
{
	switch (evt.key)
	{
		case OIS::KC_W:			mMovingForward	= false;	break;
		case OIS::KC_S:			mMovingBackward = false;	break;
		case OIS::KC_A:			mMovingLeft		= false;	break;
		case OIS::KC_D:			mMovingRight	= false;	break;
		case OIS::KC_SPACE:		mMovingUp		= false;	break;
		case OIS::KC_LCONTROL:	mMovingDown		= false;	break;

		case OIS::KC_Q:			mRollingLeft	= false;	break;
		case OIS::KC_E:			mRollingRight	= false;	break;
	}
}

void Player::injectMouseMove(const OIS::MouseEvent& evt)
{
	mMainNode->rotate(Ogre::Vector3::UNIT_Y, Ogre::Radian(Ogre::Degree(-evt.state.X.rel * 0.15f)));
	mMainNode->rotate(Ogre::Vector3::UNIT_X, Ogre::Radian(Ogre::Degree(-evt.state.Y.rel * 0.15f)));
}

void Player::injectMouseDown(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
	if (id == OIS::MB_Left) // shoot
	{
		mShooting = true;
	}
	else if (id == OIS::MB_Right)
	{
		mTurretType = (TurretType(mTurretType+1) != 4) ? TurretType(mTurretType+1) : TurretType(0);
	}
}

void Player::injectMouseUp(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
	if (id == OIS::MB_Left)
	{
		mShooting = false;
	}
}

void Player::shoot()
{
	Ogre::Vector3 direction = Common::mCamera->getDerivedDirection();
	Ogre::Vector3 startposLeft = mTurretNodeLeft->_getDerivedPosition();
	Ogre::Vector3 startposCenter = mTurretNodeCenter->_getDerivedPosition();
	Ogre::Vector3 startposRight = mTurretNodeRight->_getDerivedPosition();
	Ogre::Quaternion orientationLeft = mTurretNodeLeft->_getDerivedOrientation();
	Ogre::Quaternion orientationCenter = mTurretNodeCenter->_getDerivedOrientation();
	Ogre::Quaternion orientationRight = mTurretNodeRight->_getDerivedOrientation();

    switch (mTurretType)
	{
		case TURRET_ALTERNATING:
		{
           Ogre::uint modulo = ProjectileFactory::getInstance()->getNumPlayerProjectiles() % 2;

			Ogre::Vector3 startpos = (modulo == 0) ? startposLeft : startposRight;
			Ogre::Quaternion orientation = (modulo == 0) ? orientationLeft : orientationRight;

			ProjectileFactory::getInstance()->createProjectile(ENT_PROJECTILE_PLAYER, startpos, direction, orientation, true);
		}
		break;

		case TURRET_DOUBLE:
			ProjectileFactory::getInstance()->createProjectile(ENT_PROJECTILE_PLAYER, startposLeft, direction, orientationLeft, true);
			ProjectileFactory::getInstance()->createProjectile(ENT_PROJECTILE_PLAYER, startposRight, direction, orientationRight, true);
		break;

		case TURRET_TRIPLE:
			ProjectileFactory::getInstance()->createProjectile(ENT_PROJECTILE_PLAYER, startposLeft, direction, orientationLeft, true);
			ProjectileFactory::getInstance()->createProjectile(ENT_PROJECTILE_PLAYER, startposCenter, direction, orientationCenter, true);
			ProjectileFactory::getInstance()->createProjectile(ENT_PROJECTILE_PLAYER, startposRight, direction, orientationRight, true);
		break;

		case TURRET_SINGLE:
			ProjectileFactory::getInstance()->createProjectile(ENT_PROJECTILE_PLAYER, startposCenter, direction, orientationCenter, true);
		default: // same as single
			ProjectileFactory::getInstance()->createProjectile(ENT_PROJECTILE_PLAYER, startposCenter, direction, orientationCenter, true);
		break;
	}
}

void Player::accelerate(const Ogre::Real& elapsed)
{
	// acceleration vector
	Ogre::Vector3 acceleration = Ogre::Vector3::ZERO;

	if (mMovingForward)		acceleration += Common::mCamera->getDirection();
	if (mMovingBackward)	acceleration -= Common::mCamera->getDirection();
	if (mMovingLeft)		acceleration -= Common::mCamera->getRight();
	if (mMovingRight)		acceleration += Common::mCamera->getRight();
	if (mMovingUp)			acceleration += Common::mCamera->getUp();
	if (mMovingDown)		acceleration -= Common::mCamera->getUp();

	if (acceleration.squaredLength() != 0) // gradually reach top speed
	{
		acceleration.normalise();
		mVelocity += acceleration * mTopMovingSpeed * (elapsed * 10)/2.0f;
	}
	else // gradually stop moving
	{
		mVelocity -= mVelocity * (elapsed * 10)/2.0f;
	}

	// epsilon: difference between 1 and smallest value greater than 1 representable by Ogre::Real
	Ogre::Real tooSmall = std::numeric_limits<Ogre::Real>::epsilon();

	if (mVelocity.squaredLength() > mTopMovingSpeed * mTopMovingSpeed) // keep camera velocity below top speed
	{
		mVelocity.normalise();
		mVelocity *= mTopMovingSpeed;
	}
	else if (mVelocity.squaredLength() < tooSmall * tooSmall) // set velocity to 0 if difference becomes too small
		mVelocity = Ogre::Vector3::ZERO;

	if (mVelocity != Ogre::Vector3::ZERO) // translate if previous conditions meet
		mMainNode->translate(mVelocity, Ogre::SceneNode::TS_LOCAL);
}

void Player::roll(const Ogre::Real& elapsed)
{
	Ogre::Real rotationSpeed = mTopRotationSpeed * elapsed;

	if (mRollingLeft)
		mMainNode->rotate(Ogre::Vector3::UNIT_Z, Ogre::Radian(Ogre::Degree(rotationSpeed)), Ogre::SceneNode::TS_LOCAL);
	else if (mRollingRight)
		mMainNode->rotate(Ogre::Vector3::NEGATIVE_UNIT_Z, Ogre::Radian(Ogre::Degree(rotationSpeed)), Ogre::SceneNode::TS_LOCAL);
}

void Player::detectCollisions()
{
	Ogre::Vector3 currentpos = mMainNode->_getDerivedPosition();

	Ogre::Ray downRay(Ogre::Vector3(currentpos.x, currentpos.y + 50.0f, currentpos.z), Ogre::Vector3::NEGATIVE_UNIT_Y);
	Ogre::Ray upRay(Ogre::Vector3(currentpos.x, currentpos.y - 50.0f, currentpos.z), Ogre::Vector3::UNIT_Y);
	Ogre::Ray leftRay(Ogre::Vector3(currentpos.x + 50.0f, currentpos.y, currentpos.z),Ogre::Vector3::NEGATIVE_UNIT_X);
	Ogre::Ray rightRay(Ogre::Vector3(currentpos.x - 50.0f, currentpos.y, currentpos.z), Ogre::Vector3::UNIT_X);
	Ogre::Ray frontRay(Ogre::Vector3(currentpos.x, currentpos.y, currentpos.z - 50.0f), Ogre::Vector3::NEGATIVE_UNIT_Z);
	Ogre::Ray backRay(Ogre::Vector3(currentpos.x, currentpos.y, currentpos.z + 50.0f), Ogre::Vector3::UNIT_Z);

	Ogre::Ray rays[] = { downRay, upRay, leftRay, rightRay, frontRay, backRay };
	Ogre::RaySceneQueryResult::iterator it;
	
	for (int i = 0; i < 6; ++i) // each ray
	{
		Common::mRaySceneQuery->setRay(rays[i]);
		Common::mRaySceneQuery->setSortByDistance(true);
		Common::mRaySceneQuery->setQueryMask(COLLIDABLE_WALL);

		Ogre::RaySceneQueryResult& result = Common::mRaySceneQuery->execute();

		for (it = result.begin(); it != result.end(); ++it) // all walls this ray passes through
		{
			if (it->movable->getQueryFlags() == COLLIDABLE_WALL)
            {
				Ogre::Vector3 origin = rays[i].getOrigin();
				Ogre::Vector3 direction = rays[i].getDirection();
				Ogre::Real directionLength = direction.length();
				Ogre::Real distance = it->distance;

				direction /= directionLength; // direction is now a unit vector
				Ogre::Vector3 intersectionpos = origin + (direction * distance); // position of the intersection

				if (intersectionpos.squaredDistance(currentpos) <= 100.0f*100.0f)
				{
					// check if the player hit the ending point
                	if (it->movable->isAttached())
                	{
                    	// note that the BrushEntity is attached to the SceneNode, not the Ogre::Entity itself
                    	const BrushEntity* const brush = Ogre::any_cast<BrushEntity*>(it->movable->getParentNode()->getUserAny());

                    	// this is the ending point
                    	if (brush->isType(BRUSH_EXIT))
                    	{
                        	// so we trigger the map's end
                        	Game::getInstance()->triggerMapEnd();
                        	return;
                    	}
                	}

					if      (i==0)	mMainNode->translate(Ogre::Vector3(currentpos.x, intersectionpos.y + 100.0f, currentpos.z) - currentpos);
					else if (i==1)	mMainNode->translate(Ogre::Vector3(currentpos.x, intersectionpos.y - 100.0f, currentpos.z) - currentpos);
					else if (i==2)	mMainNode->translate(Ogre::Vector3(intersectionpos.x + 100.0f, currentpos.y, currentpos.z) - currentpos);
					else if (i==3)	mMainNode->translate(Ogre::Vector3(intersectionpos.x - 100.0f, currentpos.y, currentpos.z) - currentpos);
					else if (i==4)	mMainNode->translate(Ogre::Vector3(currentpos.x, currentpos.y, intersectionpos.z + 100.0f) - currentpos);
					else if (i==5)	mMainNode->translate(Ogre::Vector3(currentpos.x, currentpos.y, intersectionpos.z - 100.0f) - currentpos);

					break;
				}
			}
		}
	}
}

void Player::resetPosition()
{
	stopMovement();

	Ogre::Vector3 currentpos = mMainNode->_getDerivedPosition();
	mMainNode->translate(mStartPosition - currentpos);
}

void Player::stopMovement()
{
	mMovingForward = false; 
	mMovingBackward = false;
	mMovingLeft = false;
	mMovingRight = false;
	mMovingUp = false;
	mMovingDown = false;

	mRollingLeft = false; 
	mRollingRight = false;
	mShooting = false;

    mVelocity = Ogre::Vector3::ZERO;
}
