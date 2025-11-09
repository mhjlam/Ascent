#include "FlyingEnemy.hpp"

#include <random>
#include <numbers>

#include "Common.hpp"
#include "Map/GameWorld.hpp"
#include "Entities/Player/Player.hpp"
#include "Entities/Projectiles/ProjectileFactory.hpp"


FlyingEnemy::FlyingEnemy(Ogre::Entity* entity, Ogre::SceneNode* node)
: MovingEnemy(entity, node) {
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
		static std::random_device rd;
		static std::mt19937 gen(rd());
		static std::uniform_int_distribution<> distrib(0, 5);
		Ogre::uint num = distrib(gen);

		const int direction = num % 3; // 0, 1, or 2
		const int sign = (num < 3) ? 1 : -1;
		
		if (direction == 0) {
			target.y += sign * 200;
		}
		else {
			target.x += sign * 200;
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
