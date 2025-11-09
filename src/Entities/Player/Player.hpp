#pragma once

#include <Ogre.h>
#include <OGRE/Bites/OgreInput.h>
#include <boost/serialization/base_object.hpp>

#include "Entities/GameEntity.hpp"


class GameWorld;  // Forward declaration

enum class TurretType { 
	Single, 
	Double, 
	Triple,
	Alternate,
};


class Player : public GameEntity {
public:
    Player() = default;
	Player(Ogre::SceneNode* const player_node);

public: 
	void update(const Ogre::Real& elapsed);
	void reset_position();
    void stop_movement();

	// input
	void inject_key_down(const OgreBites::KeyboardEvent& event);
	void inject_key_up(const OgreBites::KeyboardEvent& event);
	void inject_mouse_move(const OgreBites::MouseMotionEvent& event);
	void inject_mouse_down(const OgreBites::MouseButtonEvent& event);
	void inject_mouse_up(const OgreBites::MouseButtonEvent& event);

	// GETTERS/SETTERS
    const Ogre::uint get_health() { 
		return health_;
	}
	const Ogre::uint get_score() { 
		return credits_;
	}
	
	void set_health(const Ogre::uint health)	{ 
		if (health > 0) {
			health_ = health;
		}
	}
	void add_points(const Ogre::uint value) { 
		if (value > 0) {
			credits_ += value;
		}
	}
	void set_score(const Ogre::uint score) { 
		credits_ = score;
	}

	void hit(Ogre::uint dmg) { 
		health_ -= dmg;
	}

private: // procedures
	void shoot();
	void accelerate(const Ogre::Real& elapsed);
	void roll(const Ogre::Real& elapsed);
	void detect_collisions();

    friend class boost::serialization::access;
	template<class Archive>
    void save(Archive& archive, const unsigned int version) const {
		archive & boost::serialization::base_object<GameEntity>(*this);
    }

	template<class Archive>
	void load(Archive& archive, const unsigned int version) {
		archive & boost::serialization::base_object<GameEntity>(*this);
		GameWorld::instance().register_entity(this);
	}

private: 
	// switches
	bool move_forward_;
	bool move_backward_;
	bool move_left_;
	bool move_right_;
	bool move_up_;
	bool move_down_;
	bool roll_left_;
	bool roll_right_;
	bool shooting_;

	// math
	Ogre::Real top_speed_;
	Ogre::Real top_rotation_;
	Ogre::Vector3 velocity_;

	// player attachments
	Ogre::SceneNode* turret_center_node_;
	Ogre::SceneNode* turret_left_node_;
	Ogre::SceneNode* turret_right_node_;
	Ogre::SceneNode* flashlight_node_;
	Ogre::Light* flashlight_;

	// variables
	Ogre::uint health_;
	Ogre::uint credits_;
	Ogre::Real shoot_timer_;
	TurretType turret_type_;
};
