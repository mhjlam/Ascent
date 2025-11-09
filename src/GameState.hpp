#pragma once

#include <string>
#include <vector>

#include <Ogre.h>
#include <boost/algorithm/string.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>


class GameState {
public:
	void load();
	void add_dead_enemy(std::string deadEnemyEntityName);
	void clear_dead_enemies();

public: // getters
	bool is_dead(std::string enemyId);

public: // setters
	void set_current_level(int currentLevel);
	void set_player_health(int playerHealth);
	void set_player_location(Ogre::Vector3 playerPosition);
	void set_player_orientation(Ogre::Quaternion playerOrientation);
	void set_time_left(float timeLeft);
	void set_score(int score);

private:
	// use boost to implement serialization
	friend class boost::serialization::access;
	
	template<class Archive>
    void save(Archive & ar, [[maybe_unused]] const unsigned int version) const {
		// create a string that contains the enemies that are killed, separated by delimiter |
		std::string dead_enemies;
		for (size_t i = 0; i < dead_enemies_.size(); ++i) {
			dead_enemies += dead_enemies_[i];
			if (i != dead_enemies_.size() - 1) {
				dead_enemies += "|";
			}
		}

		// serialize the game state data
        ar & current_level_
		   & player_health_
		   & player_position_.x
		   & player_position_.y
		   & player_position_.z
		   & player_orientation_.w
		   & player_orientation_.x
		   & player_orientation_.y
		   & player_orientation_.z
		   & time_left_
		   & score_
		   & dead_enemies;
	}

    template<class Archive>
    void load(Archive & ar, [[maybe_unused]] const unsigned int version) {
		std::string dead_enemies;

		// deserialize game state data
		ar & current_level_
		   & player_health_
		   & player_position_.x
		   & player_position_.y
		   & player_position_.z
		   & player_orientation_.w
		   & player_orientation_.x
		   & player_orientation_.y
		   & player_orientation_.z
		   & time_left_
		   & score_
		   & dead_enemies;
		
		// split the killed enemies string with delimiter | and store them in the vector
		boost::split(dead_enemies_, dead_enemies, boost::is_any_of("|"));
    }
	// tell boost that there's a load and save method instead of just one serialize method
    BOOST_SERIALIZATION_SPLIT_MEMBER()

private:
	int current_level_;
	int player_health_;

	Ogre::Vector3 player_position_;
	Ogre::Quaternion player_orientation_;

	float time_left_;
	int score_;
	std::vector<std::string> dead_enemies_;
};

// tracking is not required since we don't serialize pointers
BOOST_CLASS_TRACKING(GameState, boost::serialization::track_never)
