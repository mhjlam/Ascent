#pragma once

#include "Entities/Enemies/MovingEnemy.hpp"


class FlyingEnemy : public MovingEnemy {
public:
	FlyingEnemy(Ogre::Entity* entity, Ogre::SceneNode* node);
    void update(const Ogre::Real& elapsed);

private:
	void get_default_anim_state();

private:
	Ogre::Real move_timer_;	
};
