#include "Player.hpp"

#include <iostream>

#include "Common.hpp"
#include "Constants.hpp"

#include "Game/Game.hpp"
#include "Entities/BrushEntity.hpp"
#include "Entities/Projectiles/Projectile.hpp"
#include "Entities/Projectiles/ProjectileFactory.hpp"


Player::Player(Ogre::SceneNode* const playerNode)
: GameEntity(nullptr, playerNode)
, move_forward_(false)
, move_backward_(false)
, move_left_(false)
, move_right_(false)
, move_up_(false)
, move_down_(false)
, roll_left_(false)
, roll_right_(false)
, shooting_(false)
, top_speed_(35.0f)
, top_rotation_(180.0f)
, velocity_(Ogre::Vector3::ZERO)
, health_(100)
, credits_(0)
, shoot_timer_(0.0f)
, turret_type_(TurretType::Alternate) {
	// player node
    main_node_ = Common::scene_manager->getRootSceneNode()->createChildSceneNode();
	main_node_->attachObject(Common::camera);

	// turret nodes
	turret_center_node_ = main_node_->createChildSceneNode("node_player_turret_center", Ogre::Vector3(0.0f, -100.0f, 0.0f));
	turret_left_node_	  = main_node_->createChildSceneNode("node_player_turret_left", Ogre::Vector3(-150.0f, -50.0f, 0.0f));
	turret_right_node_  = main_node_->createChildSceneNode("node_player_turret_right", Ogre::Vector3(150.0f, -50.0f, 0.0f));

	// Flashlight disabled - now handled in GameWorld.cpp for better control
	flashlight_ = nullptr;
	flashlight_node_ = nullptr;
}

void Player::update(const Ogre::Real& elapsed) {
	shoot_timer_ -= elapsed;

	if (shooting_ && shoot_timer_ <= 0.0f) {
		shoot_timer_ = 0.5f;
		shoot();
	}

	// Flashlight update disabled - now handled in GameWorld.cpp

	accelerate(elapsed);
	roll(elapsed);
	detect_collisions();
}

void Player::inject_key_down(const OgreBites::KeyboardEvent& evt) {
	switch (evt.keysym.sym) {
		case 'w':
			move_forward_ = true;
			break;
		case 's':
			move_backward_ = true;
			break;
		case 'a':
			move_left_ = true;
			break;
		case 'd':
			move_right_ = true;
			break;
		case OgreBites::SDLK_SPACE:
			move_up_ = true;
			break;
		case OgreBites::SDLK_LSHIFT:
			move_down_ = true;
			break;
		case 'q':
			roll_left_ = true;
			break;
		case 'e':	
			roll_right_ = true;
			break;
	}
}

void Player::inject_key_up(const OgreBites::KeyboardEvent& evt) {
	switch (evt.keysym.sym) {
		case 'w':
			move_forward_	= false;
			 break;
		case 's':
			move_backward_ = false; 
			break;
		case 'a': 
			move_left_ = false; 
			break;
		case 'd':
			move_right_ = false; 
			break;
		case OgreBites::SDLK_SPACE:
			move_up_ = false; 
			break;
		case OgreBites::SDLK_LSHIFT:
			move_down_ = false; 
			break;
		case 'q':
			roll_left_ = false; 
			break;
		case 'e':
			roll_right_ = false; 
			break;
	}
}

void Player::inject_mouse_move(const OgreBites::MouseMotionEvent& evt) {
	main_node_->rotate(Ogre::Vector3::UNIT_Y, Ogre::Radian(Ogre::Degree(-evt.xrel * 0.15f)));
	main_node_->rotate(Ogre::Vector3::UNIT_X, Ogre::Radian(Ogre::Degree(-evt.yrel * 0.15f)));
}

void Player::inject_mouse_down(const OgreBites::MouseButtonEvent& evt) {
	if (evt.button == OgreBites::BUTTON_LEFT) { // shoot
		shooting_ = true;
	}
	else if (evt.button == OgreBites::BUTTON_RIGHT) {
		turret_type_ = (static_cast<int>(turret_type_) + 1 != 4) 
			? static_cast<TurretType>(static_cast<int>(turret_type_) + 1) 
			: static_cast<TurretType>(0);
	}
}

void Player::inject_mouse_up(const OgreBites::MouseButtonEvent& evt) {
	if (evt.button == OgreBites::BUTTON_LEFT) {
		shooting_ = false;
	}
}

void Player::shoot() {
	Ogre::Vector3 direction = Common::camera->getDerivedDirection();
	Ogre::Vector3 startposLeft = turret_left_node_->_getDerivedPosition();
	Ogre::Vector3 startposCenter = turret_center_node_->_getDerivedPosition();
	Ogre::Vector3 startposRight = turret_right_node_->_getDerivedPosition();
	Ogre::Quaternion orientationLeft = turret_left_node_->_getDerivedOrientation();
	Ogre::Quaternion orientationCenter = turret_center_node_->_getDerivedOrientation();
	Ogre::Quaternion orientationRight = turret_right_node_->_getDerivedOrientation();

    switch (turret_type_) {
		default:
		case TurretType::Single:
			ProjectileFactory::instance().create_projectile(EntityType::PlayerProjectileType, startposCenter, direction, orientationCenter, true);
			break;

		case TurretType::Double:
			ProjectileFactory::instance().create_projectile(EntityType::PlayerProjectileType, startposLeft, direction, orientationLeft, true);
			ProjectileFactory::instance().create_projectile(EntityType::PlayerProjectileType, startposRight, direction, orientationRight, true);
			break;

		case TurretType::Triple:
			ProjectileFactory::instance().create_projectile(EntityType::PlayerProjectileType, startposLeft, direction, orientationLeft, true);
			ProjectileFactory::instance().create_projectile(EntityType::PlayerProjectileType, startposCenter, direction, orientationCenter, true);
			ProjectileFactory::instance().create_projectile(EntityType::PlayerProjectileType, startposRight, direction, orientationRight, true);
			break;
			
		case TurretType::Alternate: {
           Ogre::uint modulo = ProjectileFactory::instance().get_num_player_projectiles() % 2;

			Ogre::Vector3 startpos = (modulo == 0) ? startposLeft : startposRight;
			Ogre::Quaternion orientation = (modulo == 0) ? orientationLeft : orientationRight;

			ProjectileFactory::instance().create_projectile(EntityType::PlayerProjectileType, startpos, direction, orientation, true);
			break;
		}
	}
}

void Player::accelerate(const Ogre::Real& elapsed) {
	// acceleration vector
	Ogre::Vector3 acceleration = Ogre::Vector3::ZERO;

	// Get camera orientation vectors from the camera's parent scene node
	Ogre::Quaternion camOrientation = Common::camera->getParentSceneNode()->getOrientation();
	Ogre::Vector3 forward = camOrientation * Ogre::Vector3::NEGATIVE_UNIT_Z;
	Ogre::Vector3 right = camOrientation * Ogre::Vector3::UNIT_X;
	Ogre::Vector3 up = camOrientation * Ogre::Vector3::UNIT_Y;

	if (move_forward_) {
		acceleration += forward;
	}
	if (move_backward_) {
		acceleration -= forward;
	}
	if (move_left_) {
		acceleration -= right;
	}
	if (move_right_) {
		acceleration += right;
	}
	if (move_up_) {
		acceleration += up;
	}
	if (move_down_) {
		acceleration -= up;
	}	

	if (acceleration.squaredLength() != 0) { // gradually reach top speed
		acceleration.normalise();
		velocity_ += acceleration * top_speed_ * (elapsed * 10)/2.0f;
	}
	else { // gradually stop moving
		velocity_ -= velocity_ * (elapsed * 10)/2.0f;
	}

	// epsilon: difference between 1 and smallest value greater than 1 representable by Ogre::Real
	Ogre::Real tooSmall = std::numeric_limits<Ogre::Real>::epsilon();

	if (velocity_.squaredLength() > top_speed_ * top_speed_) { // keep camera velocity below top speed
		velocity_.normalise();
		velocity_ *= top_speed_;
	}
	else if (velocity_.squaredLength() < tooSmall * tooSmall) { // set velocity to 0 if difference becomes too small
		velocity_ = Ogre::Vector3::ZERO;
	}

	if (velocity_ != Ogre::Vector3::ZERO) { // translate if previous conditions meet
		// Use TS_PARENT because velocity is calculated in world space
		main_node_->translate(velocity_, Ogre::SceneNode::TS_PARENT);
	}
}

void Player::roll(const Ogre::Real& elapsed) {
	Ogre::Real rotationSpeed = top_rotation_ * elapsed;

	if (roll_left_) {
		main_node_->rotate(Ogre::Vector3::UNIT_Z, Ogre::Radian(Ogre::Degree(rotationSpeed)), Ogre::SceneNode::TS_LOCAL);
	}
	else if (roll_right_) {
		main_node_->rotate(Ogre::Vector3::NEGATIVE_UNIT_Z, Ogre::Radian(Ogre::Degree(rotationSpeed)), Ogre::SceneNode::TS_LOCAL);
	}
}

void Player::detect_collisions() {
	Ogre::Vector3 currentpos = main_node_->_getDerivedPosition();

	Ogre::Ray downRay(Ogre::Vector3(currentpos.x, currentpos.y + 50.0f, currentpos.z), Ogre::Vector3::NEGATIVE_UNIT_Y);
	Ogre::Ray upRay(Ogre::Vector3(currentpos.x, currentpos.y - 50.0f, currentpos.z), Ogre::Vector3::UNIT_Y);
	Ogre::Ray leftRay(Ogre::Vector3(currentpos.x + 50.0f, currentpos.y, currentpos.z),Ogre::Vector3::NEGATIVE_UNIT_X);
	Ogre::Ray rightRay(Ogre::Vector3(currentpos.x - 50.0f, currentpos.y, currentpos.z), Ogre::Vector3::UNIT_X);
	Ogre::Ray frontRay(Ogre::Vector3(currentpos.x, currentpos.y, currentpos.z - 50.0f), Ogre::Vector3::NEGATIVE_UNIT_Z);
	Ogre::Ray backRay(Ogre::Vector3(currentpos.x, currentpos.y, currentpos.z + 50.0f), Ogre::Vector3::UNIT_Z);

	Ogre::Ray rays[] = { downRay, upRay, leftRay, rightRay, frontRay, backRay };
	Ogre::RaySceneQueryResult::iterator it;
	
	for (int i = 0; i < 6; ++i) { // each ray
		Common::ray_scene_query->setRay(rays[i]);
		Common::ray_scene_query->setSortByDistance(true);
		Common::ray_scene_query->setQueryMask(static_cast<Ogre::uint32>(QueryFlags::Wall));

		Ogre::RaySceneQueryResult& result = Common::ray_scene_query->execute();

		for (it = result.begin(); it != result.end(); ++it) { // all walls this ray passes through
			if (it->movable->getQueryFlags() == static_cast<Ogre::uint32>(QueryFlags::Wall)) {
				Ogre::Vector3 origin = rays[i].getOrigin();
				Ogre::Vector3 direction = rays[i].getDirection();
				Ogre::Real directionLength = direction.length();
				Ogre::Real distance = it->distance;

				direction /= directionLength; // direction is now a unit vector
				Ogre::Vector3 intersectionpos = origin + (direction * distance); // position of the intersection

				if (intersectionpos.squaredDistance(currentpos) <= 100.0f*100.0f) {
					// check if the player hit the ending point
                	if (it->movable->isAttached()) {
                    	// note that the BrushEntity is attached to the SceneNode, not the Ogre::Entity itself
                    	const BrushEntity* const brush = Ogre::any_cast<BrushEntity*>(it->movable->getParentNode()->getUserAny());

                    	// this is the ending point
                    	if (brush->is_brush_type(BrushType::Exit)) {
                        	// so we trigger the map's end
                        	Game::instance()->trigger_map_end();
                        	return;
                    	}
                	}

					if (i==0) {
						main_node_->translate(Ogre::Vector3(currentpos.x, intersectionpos.y + 100.0f, currentpos.z) - currentpos);
					}
					else if (i==1) {
						main_node_->translate(Ogre::Vector3(currentpos.x, intersectionpos.y - 100.0f, currentpos.z) - currentpos);
					}
					else if (i==2) {
						main_node_->translate(Ogre::Vector3(intersectionpos.x + 100.0f, currentpos.y, currentpos.z) - currentpos);
					}
					else if (i==3) {
						main_node_->translate(Ogre::Vector3(intersectionpos.x - 100.0f, currentpos.y, currentpos.z) - currentpos);
					}
					else if (i==4) {
						main_node_->translate(Ogre::Vector3(currentpos.x, currentpos.y, intersectionpos.z + 100.0f) - currentpos);
					}
					else if (i==5) {
						main_node_->translate(Ogre::Vector3(currentpos.x, currentpos.y, intersectionpos.z - 100.0f) - currentpos);
					}

					break;
				}
			}
		}
	}
}

void Player::reset_position() {
	stop_movement();

	Ogre::Vector3 currentpos = main_node_->_getDerivedPosition();
	main_node_->translate(start_position_ - currentpos);
}

void Player::stop_movement() {
	move_forward_ = false; 
	move_backward_ = false;
	move_left_ = false;
	move_right_ = false;
	move_up_ = false;
	move_down_ = false;

	roll_left_ = false; 
	roll_right_ = false;
	shooting_ = false;

    velocity_ = Ogre::Vector3::ZERO;
}
