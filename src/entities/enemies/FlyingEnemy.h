#ifndef FLYINGENEMY_H_
#define FLYINGENEMY_H_

#include "../../stdafx.h"
#include "MovingEnemy.h"

/*
 *  This enemy flies in the air and randomly moves up/down/left or right at a certain interval.
 */

class FlyingEnemy : public MovingEnemy 
{
private:
	Ogre::Real mMovementTimer;
	
	void getDefaultAnimationState();
public:
	FlyingEnemy(Ogre::Entity* entity, Ogre::SceneNode* node);

    void update(const Ogre::Real& elapsed);
};

#endif