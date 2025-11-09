#pragma once

#include "Entities/Enemies/MovingEnemy.hpp"


class FlyingEnemy : public MovingEnemy {
public:
	FlyingEnemy(Ogre::Entity* entity, Ogre::SceneNode* node);
    void update(const Ogre::Real& elapsed) override;

private:
	void get_default_anim_state() override;

private:
	Ogre::Real move_timer_;	
};
