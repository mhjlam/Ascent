#pragma once

#include "Constants.hpp"
#include "Entities/GameEntity.hpp"


class Projectile : public GameEntity {
public:
    Projectile(Ogre::Entity* const entity, 
			   Ogre::SceneNode* const node, 
			   const Ogre::Vector3& direction, 
			   const Ogre::Quaternion& orientation);
	~Projectile() override = default;

public:
    void update(const Ogre::Real& elapsed) override;

protected:
	Ogre::uint damage_;
	Ogre::Real speed_;
	Ogre::Real range_;
	Ogre::Quaternion orientation_;	
};
