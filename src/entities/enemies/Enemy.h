#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "../../stdafx.h"
#include "../GameEntity.h"

/*
 *  Enemy class functions as the (base) controller class for the enemies.
 *  AI and basic updating functionality is provided in the update function.
 */

class Enemy : public GameEntity
{
protected: // variables
	friend class boost::serialization::access;
	static int					mProjectileCounter;
	Ogre::Real					mFireTimer;
	Ogre::AnimationState*		mAnimationState;
	
	Ogre::uint mHealth;

	// functions
    void fireProjectileAt(const GameEntity* const entity);
    void fireProjectileAt(const Ogre::Vector3& location);
    virtual void getDefaultAnimationState();
public:
	Enemy(Ogre::Entity* entity, Ogre::SceneNode* node);
	virtual ~Enemy() {};

    virtual void update(const Ogre::Real& elapsed);

	void hit(Ogre::uint dmg);
};

#endif