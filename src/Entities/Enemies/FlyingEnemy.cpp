#include "FlyingEnemy.hpp"

#include <ctime>
#include <numbers>

#include "Common.hpp"
#include "Map/GameWorld.hpp"
#include "Entities/Player/Player.hpp"
#include "Projectiles/ProjectileFactory.hpp"


FlyingEnemy::FlyingEnemy(Ogre::Entity* entity, Ogre::SceneNode* node) : MovingEnemy(entity, node) {
	srand((unsigned int)time(nullptr));

	move_timer_ = 0;
	speed_ = 1000;
	health_ = 150;
	set_target(get_position());
}

void FlyingEnemy::update(const Ogre::Real& elapsed) {
    move_timer_ += elapsed;

	if (move_timer_ >= 2) {
		move_timer_ = 0;

		Ogre::Vector3 target = get_position();
		
		// make a dodging motion in a random direction
		Ogre::uint num = rand() % 6;

		if (num < 1) {
			target.y += 200;
		}
		else if (num < 2) {
			target.y -= 200;
		}
		else if (num < 3) {
			target.x += 200;
		}
		else if (num < 4) {
			target.x -= 200;
		}
		else if (num < 5) {
			target.x += 200;
		}
		else if (num < 6) {
			target.x -= 200;
		}

		set_target(target);
	} 
	else {
		Ogre::Real move = speed_ * elapsed;
		target_distance_ -= move;

		if (target_distance_ <= 0.0f) {
			move_to(target_position_);
		}
		else {
			translate(direction_ * move);
		}
	}

	// enemy lookat camera
	get_node()->lookAt(Common::player->get_position(), Ogre::Node::TS_WORLD);

	// yaw 180 degrees so the ship looks at the player from the front instead of from the back
	get_node()->yaw(Ogre::Radian(std::numbers::pi_v<float>));

	fire_timer_ += elapsed;

	if (fire_timer_ >= 3) {
        fire_at(Common::player);
	}
}

void FlyingEnemy::get_default_anim_state() {
	// this mesh has no animations
}
