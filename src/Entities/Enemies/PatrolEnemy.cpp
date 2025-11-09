#include "PatrolEnemy.hpp"

#include "Common.hpp"
#include "Map/GameWorld.hpp"


PatrolEnemy::PatrolEnemy(Ogre::Entity* entity, Ogre::SceneNode* node) : MovingEnemy(entity, node) {
	health_ = 100;

	// create patrol path, walk in a square
	Ogre::Vector3 nodePosition = main_node_->getPosition();
	patrol_points_.push_back(Ogre::Vector3(nodePosition.x + 500, nodePosition.y, nodePosition.z));
	patrol_points_.push_back(Ogre::Vector3(nodePosition.x + 500, nodePosition.y, nodePosition.z - 500));
	patrol_points_.push_back(Ogre::Vector3(nodePosition.x, nodePosition.y, nodePosition.z - 500));
	patrol_points_.push_back(Ogre::Vector3(nodePosition.x, nodePosition.y, nodePosition.z));

	// set movementspeed
	speed_ = 100;

	// walk towards the first patrol point
	target_ = 0;
	
	set_target(patrol_points_[target_]);
}

void PatrolEnemy::update(const Ogre::Real& elapsed) {
	Enemy::update(elapsed);

	// when not shooting, walk
	if (animation_state_->getAnimationName() == "Walk") {
		Ogre::Real move = speed_ * elapsed;
        target_distance_ -= move;

		if (target_distance_ <= 0.0f) {
            main_node_->setPosition(target_position_);
			// if there's no new target, move to the first patrol point
			if (++target_ >= (int)patrol_points_.size()) {
				target_ = 0;
			}
			set_target(patrol_points_[target_]);
		} 
		else {
			main_node_->translate(direction_ * move);
		}
	}
}

void PatrolEnemy::get_default_anim_state() {
	animation_state_ = entity_->getAnimationState("Walk");
    animation_state_->setLoop(true);
    animation_state_->setEnabled(true);
}
