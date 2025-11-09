#include "Enemy.hpp"

#include <numbers>
#include <iostream>

#include "Common.hpp"
#include "Map/GameWorld.hpp"
#include "Entities/Player/Player.hpp"
#include "Entities/Enemies/PatrolEnemy.hpp"
#include "Entities/Enemies/FlyingEnemy.hpp"
#include "Entities/Projectiles/EnemyProjectile.hpp"
#include "Entities/Projectiles/ProjectileFactory.hpp"


constexpr Ogre::Real FIVE_OVER_TWO = 5.0f / 2.0f;


Enemy::Enemy(Ogre::Entity* entity, Ogre::SceneNode* node)
: GameEntity(entity, node)
, fire_timer_(0)
, animation_state_(nullptr)
, health_(100) {
	// set a fixed yaw, so it will keep standing up when looking at the camera
	main_node_->setFixedYawAxis(true);
    get_default_anim_state();
}

void Enemy::update(const Ogre::Real& elapsed) {
	// enemy lookat camera
	get_node()->lookAt(Common::player->get_position(), Ogre::Node::TS_WORLD);
    //Common::overview_scene_manager->getSceneNode(get_node()->getName())->lookAt(Common::player->get_position(), Ogre::Node::TS_WORLD);

	fire_timer_ += elapsed;
    
    if (animation_state_ && fire_timer_ >= Ogre::Math::RangeRandom(2.0f, 10.0f) && Common::player->get_position().squaredDistance(get_position()) <= 5000.0f*5000.0f) {
		// run the shooting animation once
		animation_state_->setEnabled(false);
		
		if (entity_->hasSkeleton() && entity_->getSkeleton()->hasAnimation("Shoot")) {
			animation_state_ = entity_->getAnimationState("Shoot");

			// since the animation might have already run once (and it's not looped) reset the time position
			animation_state_->setTimePosition(0);
			animation_state_->setLoop(false);
			animation_state_->setEnabled(true);
		}

		// fire away
		fire_at(Common::player);
	} 
	else if (animation_state_ && animation_state_->getAnimationName() == "Shoot" && animation_state_->hasEnded()) {
		// if we ran the shooting animation switch back to other animation
		animation_state_->setEnabled(false);
		get_default_anim_state();
	}

	// yaw ninty degrees so the robot looks at the player from the front instead of from the side
	main_node_->yaw(Ogre::Radian(std::numbers::pi_v<float> / 2.0f));

	if (animation_state_) {
		animation_state_->addTime(elapsed);
	}
}

void Enemy::fire_at(const GameEntity* const entity) {
    if (entity) {
        fire_at(entity->get_position());
    }
}

void Enemy::fire_at(const Ogre::Vector3& location) {
    // create a projectile
    Ogre::Vector3 start_position(get_position() + 
        Ogre::Vector3(0.0f, get_bounding_box().getSize().y * FIVE_OVER_TWO, 0.0f));
	
    try {
        ProjectileFactory::instance().create_projectile(
            EntityType::EnemyProjectileType, start_position, location, main_node_->getOrientation());
    }
    catch (Ogre::Exception& e) {
        std::cerr << e.what() << '\n';
    }

    // reset timer
	fire_timer_ = 0;
}

void Enemy::hit(Ogre::uint damage) { 
    if (!is_alive()) {
        return;
    }

    if (health_ < damage) {
        Ogre::uint add_score = 0;

        // due to Ogre::any_cast (see Projectile.cpp ~40), it is not possible to use derived versions of hit()
        switch (this->get_type()) { // luckily, we can still use the this ptr for our cause
            case (EntityType::EnemyPatrol):
                add_score = 1000;
                break;

            case (EntityType::EnemyStationary):
                add_score = 500;
                break;

            case (EntityType::EnemyFlying):
                add_score = 2000;
                break;

            default:
                break;
        }

        // finalize
        Common::player->add_points(add_score);

        // for serialization purposes
		Common::game_state.add_dead_enemy(entity_->getName()); 

        // this enemy is done for
        destroy();
	}
    else {
        health_ -= damage;
    }
}

void Enemy::get_default_anim_state() {
	try {
		// Check if entity has animations before accessing animation state
		if (entity_->hasSkeleton() && entity_->getSkeleton()->hasAnimation("Idle")) {
			animation_state_ = entity_->getAnimationState("Idle");
			animation_state_->setLoop(true);
			animation_state_->setEnabled(true);
		}
    } 
	catch (Ogre::ItemIdentityException& e) {   
        std::cerr << e.what() << '\n';
	}
}
