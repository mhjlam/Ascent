#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "../../stdafx.h"
#include "../GameEntity.h"

/*
 *  The Player. 
 *
 *  Has no mesh (Ogre::Entity) and is based on Ogre's Cameraman
 *
 */

enum TurretType { TURRET_SINGLE, TURRET_ALTERNATING, TURRET_DOUBLE, TURRET_TRIPLE };

class Player : public GameEntity
{
private: 
	// switches
	bool mMovingForward, mMovingBackward, mMovingLeft, mMovingRight, mMovingUp,	mMovingDown;
	bool mRollingLeft, mRollingRight;
	bool mShooting;

	// math
	Ogre::Real			mTopMovingSpeed;
	Ogre::Real			mTopRotationSpeed;
	Ogre::Vector3		mVelocity;

	// player attachments
	Ogre::SceneNode*	mTurretNodeCenter;
	Ogre::SceneNode*	mTurretNodeLeft;
	Ogre::SceneNode*	mTurretNodeRight;
	Ogre::Light*		mHeadLightCenter;

	// variables
	Ogre::uint			mHealth;
	Ogre::uint			mCredits;
	Ogre::Real			mShootTimer;
	TurretType			mTurretType;
    
private: // procedures
	void shoot();
	void accelerate(const Ogre::Real& elapsed);
	void roll(const Ogre::Real& elapsed);
	void detectCollisions();

    friend class boost::serialization::access;
	template<class Archive>
    void save(Archive &ar, const unsigned int version) const
    {
		ar & boost::serialization::base_object<bus_stop>(*this);
    }

	template<class Archive>
	void load(Archive &ar, const unsigned int version)
    {
		ar & boost::serialization::base_object<bus_stop>(*this);
		GameWorld::getInstance()->registerEntity(&this);
	}

public: 
	void update(const Ogre::Real& elapsed);
	void resetPosition();
    void stopMovement();

	// input
	void injectKeyDown(const OIS::KeyEvent& evt);
	void injectKeyUp(const OIS::KeyEvent& evt);
	void injectMouseMove(const OIS::MouseEvent& evt);
	void injectMouseDown(const OIS::MouseEvent& evt, OIS::MouseButtonID id);
	void injectMouseUp(const OIS::MouseEvent& evt, OIS::MouseButtonID id);

	// GETTERS/SETTERS
    const Ogre::uint getHealth()				{ return mHealth; }
	const Ogre::uint getCredits()				{ return mCredits; }
	
	void setHealth(const Ogre::uint health)		{ if (health > 0) mHealth = health; }
	void addScorePoints(const Ogre::uint value) { if (value > 0) mCredits += value; }
	void setScore(const Ogre::uint score)		{ mCredits = score; }

	void hit(Ogre::uint dmg)					{ mHealth -= dmg; }


public: // ctor
    Player()	{};
	Player(Ogre::SceneNode* const playerNode);
};

#endif