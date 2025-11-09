#include "Enemy.hpp"

#include <numbers>

#include "Common.hpp"
#include "Map/GameWorld.hpp"
#include "Entities/Player/Player.hpp"
#include "Entities/Enemies/PatrolEnemy.hpp"
#include "Entities/Enemies/FlyingEnemy.hpp"
#include "Projectiles/EnemyProjectile.hpp"
#include "Projectiles/ProjectileFactory.hpp"


int Enemy::num_projectiles_ = 0;

Enemy::Enemy(Ogre::Entity* entity, Ogre::SceneNode* node) :	GameEntity(entity, node), fire_timer_(0), animation_state_(nullptr), health_(100) {
	// set a fixed yaw, so it will keep standing up when looking at the camera
	main_node_->setFixedYawAxis(true);
  
    get_default_anim_state();
}

void Enemy::update(const Ogre::Real& elapsed) {
	// enemy lookat camera
	get_node()->lookAt(Common::player->get_position(), Ogre::Node::TS_WORLD);
    //Common::overview_scene_manager->getSceneNode(get_node()->getName())->lookAt(Common::player->get_position(), Ogre::Node::TS_WORLD);

	fire_timer_ += elapsed;
    
    if (fire_timer_ >= Ogre::Math::RangeRandom(2.0f, 10.0f) && Common::player->get_position().squaredDistance(get_position()) <= 5000.0f*5000.0f) {
		// run the shooting animation once
		animation_state_->setEnabled(false);
		animation_state_ = entity_->getAnimationState("Shoot");

		// since the animation might have already run once (and it's not looped) reset the time position
		animation_state_->setTimePosition(0);
		animation_state_->setLoop(false);
		animation_state_->setEnabled(true);

		// fire away
		fire_at(Common::player);
	} 
	else if (animation_state_->getAnimationName() == "Shoot" && animation_state_->hasEnded()) {
		// if we ran the shooting animation switch back to other animation
		animation_state_->setEnabled(false);

		get_default_anim_state();
	}

	// yaw ninty degrees so the robot looks at the player from the front instead of from the side
	main_node_->yaw(Ogre::Radian(std::numbers::pi_v<float> / 2.0f));

	animation_state_->addTime(elapsed);
}

void Enemy::fire_at(const GameEntity* const entity) {
    if (entity) {
        fire_at(entity->get_position());
    }
}

void Enemy::fire_at(const Ogre::Vector3& location) {
    // create a projectile
    Ogre::Vector3 startPosition(get_position() + Ogre::Vector3(0.0f, get_bounding_box().getSize().y * (5/2), 0.0f));
	
    try {
        ProjectileFactory::instance().create_projectile(EntityType::EnemyProjectileType, startPosition, location, main_node_->getOrientation());
    }
    catch (Ogre::Exception& e) {
        // occurs if the given projectile is not a valid type to be constructed by the ProjectileFactory
        // should not occur as the type is hardcoded, but we don't want any funky crashes

        // not sure how to print to Ogre's log, parameters are a mystery to me
        e.what(); // suppress compiler warning
    }

    // reset timer
	fire_timer_ = 0;
}

void Enemy::hit(Ogre::uint dmg) { 
    if (!is_alive()) {
        return;
    }

    if (health_ < dmg) {
        Ogre::uint scoreToAdd = 0;

        // due to Ogre::any_cast (see Projectile.cpp ~40), it is not possible to use derived versions of hit()
        switch (this->get_type()) { // luckily, we can still use the this ptr for our cause
            case (EntityType::EnemyPatrol):
                scoreToAdd = 1000;
                break;

            case (EntityType::EnemyStationary):
                scoreToAdd = 500;
                break;

            case (EntityType::EnemyFlying):
                scoreToAdd = 2000;
                break;

            default:
                break;
        }

        // finalize
        Common::player->add_points(scoreToAdd);

        // for serialization purposes
		Common::game_state.add_dead_enemy(entity_->getName()); 

        // this enemy is done for
        destroy();
	}
    else {
        health_ -= dmg;
    }
}

void Enemy::get_default_anim_state() {
	try {
        animation_state_ = entity_->getAnimationState("Idle");
        animation_state_->setLoop(true);
        animation_state_->setEnabled(true);
    } 
	catch (Ogre::ItemIdentityException& e) {   
		// means that this mesh has no animations
        // also, Ogre will automatically put a message about this in the log
        e.what(); // surpress compiler warning
	}
}
