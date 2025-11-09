#include "MovingEnemy.hpp"


MovingEnemy::MovingEnemy(Ogre::Entity* entity, Ogre::SceneNode* node)
: Enemy(entity, node) {
}


void MovingEnemy::set_target(Ogre::Vector3 target) {
	target_position_ = target;
	direction_ = target_position_ - main_node_->getPosition();
	target_distance_ = direction_.normalise();
}

void MovingEnemy::update(const Ogre::Real& elapsed) {
    Enemy::update(elapsed);
}
