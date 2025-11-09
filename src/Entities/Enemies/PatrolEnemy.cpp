#include "PatrolEnemy.hpp"

#include "Common.hpp"
#include "Map/GameWorld.hpp"


PatrolEnemy::PatrolEnemy(Ogre::Entity* entity, Ogre::SceneNode* node) : MovingEnemy(entity, node) {
	health_ = 100;

	// create patrol path, walk in a square
	Ogre::Vector3 node_position = main_node_->getPosition();
	patrol_points_.push_back(Ogre::Vector3(node_position.x + 500, node_position.y, node_position.z));
	patrol_points_.push_back(Ogre::Vector3(node_position.x + 500, node_position.y, node_position.z - 500));
	patrol_points_.push_back(Ogre::Vector3(node_position.x, node_position.y, node_position.z - 500));
	patrol_points_.push_back(Ogre::Vector3(node_position.x, node_position.y, node_position.z));

	// set movementspeed
	speed_ = 100;

	// walk towards the first patrol point
	target_ = 0;
	
	set_target(patrol_points_[target_]);
}

void PatrolEnemy::update(const Ogre::Real& elapsed) {
	Enemy::update(elapsed);

	// when not shooting, walk
	if (animation_state_ && animation_state_->getAnimationName() == "Walk") {
		Ogre::Real move = speed_ * elapsed;
		target_distance_ -= move;

		if (target_distance_ <= 0.0f) {
			main_node_->setPosition(target_position_);
			// cycle to next patrol point
			target_ = (target_ + 1) % patrol_points_.size();
			set_target(patrol_points_[target_]);
		} 
		else {
			main_node_->translate(direction_ * move);
		}
	}
}

void PatrolEnemy::get_default_anim_state() {
	// Check if entity has animations before accessing animation state
	if (entity_->hasSkeleton() && entity_->getSkeleton()->hasAnimation("Walk")) {
		animation_state_ = entity_->getAnimationState("Walk");
		animation_state_->setLoop(true);
		animation_state_->setEnabled(true);
	}
}
