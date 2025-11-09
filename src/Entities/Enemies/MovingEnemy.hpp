#pragma once

#include "Entities/Enemies/Enemy.hpp"


class MovingEnemy : public Enemy {
public:
	MovingEnemy(Ogre::Entity* entity, Ogre::SceneNode* node);
    void update(const Ogre::Real& elapsed);

protected:
	void set_target(Ogre::Vector3);
	virtual void get_default_anim_state() = 0;

protected:
	Ogre::uint target_;

	Ogre::Real speed_;
	Ogre::Real target_distance_;

	Ogre::Vector3 direction_;
	Ogre::Vector3 target_position_;
};
