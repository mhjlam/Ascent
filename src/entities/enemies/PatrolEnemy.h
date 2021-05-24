#ifndef _PATROL_ENEMY_H_
#define _PATROL_ENEMY_H_

#include "../../stdafx.h"
#include "MovingEnemy.h"

/*
 *  Enemy that stands on the floor (even though there is no gravity) and consistently patrols a certain rectangluar area (defined in constructor)
 */

class PatrolEnemy : public MovingEnemy
{
private:
	std::vector<Ogre::Vector3> mPatrolPoints;

private:
	void getDefaultAnimationState();

public:
	PatrolEnemy(Ogre::Entity* entity, Ogre::SceneNode* node);

public:
    void update(const Ogre::Real& elapsed);
};

#endif