#pragma once

#include <boost/serialization/access.hpp>

#include "Entities/GameEntity.hpp"


class Enemy : public GameEntity {
public:
	Enemy(Ogre::Entity* entity, Ogre::SceneNode* node);
	virtual ~Enemy() = default;

    virtual void update(const Ogre::Real& elapsed);
	void hit(Ogre::uint damage);

protected:
    void fire_at(const GameEntity* const entity);
    void fire_at(const Ogre::Vector3& location);
    virtual void get_default_anim_state();

protected:
	friend class boost::serialization::access;

protected:
	Ogre::uint health_;
	Ogre::Real fire_timer_;
	Ogre::AnimationState* animation_state_;
};
