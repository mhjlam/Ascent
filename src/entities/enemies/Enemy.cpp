#include "stdafx.h"
#include "Enemy.h"

#include "../../Common.h"
#include "PatrolEnemy.h"
#include "FlyingEnemy.h"
#include "../projectiles/EnemyProjectile.h"
#include "../projectiles/ProjectileFactory.h"
#include "../../map/GameWorld.h"

int Enemy::mProjectileCounter = 0;

Enemy::Enemy(Ogre::Entity* entity, Ogre::SceneNode* node) :	GameEntity(entity, node), mFireTimer(0), mAnimationState(NULL), mHealth(100)
{
	// set a fixed yaw, so it will keep standing up when looking at the camera
	mMainNode->setFixedYawAxis(true);
  
    getDefaultAnimationState();
}

void Enemy::update(const Ogre::Real& elapsed) 
{
	// enemy lookat camera
	getNode()->lookAt(Common::mPlayer->getPosition(), Ogre::Node::TS_WORLD);
    //Common::mSceneManagerOverview->getSceneNode(getNode()->getName())->lookAt(Common::mPlayer->getPosition(), Ogre::Node::TS_WORLD);

	mFireTimer += elapsed;
    
    if (mFireTimer >= Ogre::Math::RangeRandom(2.0f, 10.0f) && Common::mPlayer->getPosition().squaredDistance(getPosition()) <= 5000.0f*5000.0f) 
	{
		// run the shooting animation once
		mAnimationState->setEnabled(false);
		mAnimationState = mEntity->getAnimationState("Shoot");

		// since the animation might have already run once (and it's not looped) reset the time position
		mAnimationState->setTimePosition(0);
		mAnimationState->setLoop(false);
		mAnimationState->setEnabled(true);

		// fire away
		fireProjectileAt(Common::mPlayer);
	} 
	else if (mAnimationState->getAnimationName() == "Shoot" && mAnimationState->hasEnded()) 
	{
		// if we ran the shooting animation switch back to other animation
		mAnimationState->setEnabled(false);

		getDefaultAnimationState();
	}

	// yaw ninty degrees so the robot looks at the player from the front instead of from the side
	mMainNode->yaw(Ogre::Radian(Ogre::Math::PI / 2));

	mAnimationState->addTime(elapsed);
}

void Enemy::fireProjectileAt(const GameEntity* const entity)
{
    if (entity)
        fireProjectileAt(entity->getPosition());
}

void Enemy::fireProjectileAt(const Ogre::Vector3& location)
{
    // create a projectile
    Ogre::Vector3 startPosition(getPosition() + Ogre::Vector3(0.0f, getBoundingBox().getSize().y * (5/2), 0.0f));
	
    try
    {   ProjectileFactory::getInstance()->createProjectile(ENT_PROJECTILE_ENEMY, startPosition, location, mMainNode->getOrientation());     }
    catch (Ogre::Exception& e)
    {
        // occurs if the given projectile is not a valid type to be constructed by the ProjectileFactory
        // should not occur as the type is hardcoded, but we don't want any funky crashes

        // not sure how to print to Ogre's log, parameters are a mystery to me
        e.what(); // suppress compiler warning
    }

    // reset timer
	mFireTimer = 0;
}

void Enemy::hit(Ogre::uint dmg) 
{ 
    if (!isAlive())
        return;

    if (mHealth < dmg)
	{
        Ogre::uint scoreToAdd = 0;

        // due to Ogre::any_cast (see Projectile.cpp ~40), it is not possible to use derived versions of hit()
        switch (this->getType()) // luckily, we can still use the this ptr for our cause
        {
            case (ENT_ENEMY_PATROL):
                scoreToAdd = 1000;
            break;

            case (ENT_ENEMY_STATIONARY):
                scoreToAdd = 500;
            break;

            case (ENT_ENEMY_FLYING):
                scoreToAdd = 2000;
            break;

            default:
            break;
        }

        // finalize
        Common::mPlayer->addScorePoints(scoreToAdd);

        // for serialization purposes
		Common::mState.addDeadEnemy(mEntity->getName()); 

        // this enemy is done for
        destroyMe();
	}
    else
        mHealth -= dmg;
}

void Enemy::getDefaultAnimationState() 
{
	try
    {
        mAnimationState = mEntity->getAnimationState("Idle");
        mAnimationState->setLoop(true);
        mAnimationState->setEnabled(true);
    } 
	catch (Ogre::ItemIdentityException& e)
	{   
		// means that this mesh has no animations
        // also, Ogre will automatically put a message about this in the log
        e.what(); // surpress compiler warning
	}
}
