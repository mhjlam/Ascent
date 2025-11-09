#pragma once

#include "Entities/Enemies/MovingEnemy.hpp"


class PatrolEnemy : public MovingEnemy {
public:
	PatrolEnemy(Ogre::Entity* entity, Ogre::SceneNode* node);
    void update(const Ogre::Real& elapsed);

private:
	void get_default_anim_state();

private:
	std::vector<Ogre::Vector3> patrol_points_;
};
