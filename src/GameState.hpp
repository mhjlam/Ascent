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
	void add_dead_enemy(std::string target_enemy_id);
	void clear_dead_enemies();

public: // getters
	bool is_dead(std::string enemy_id);

public: // setters
	void set_current_level(int current_level);
	void set_player_health(int player_health);
	void set_player_location(Ogre::Vector3 player_position);
	void set_player_orientation(Ogre::Quaternion player_orientation);
	void set_time_left(float time_left);
	void set_score(int score);

private:
	// use boost to implement serialization
	friend class boost::serialization::access;
	
	template<class Archive>
    void save(Archive & archive, [[maybe_unused]] const unsigned int version) const {
		// create a string that contains the enemies that are killed, separated by delimiter |
		std::string dead_enemies;
		for (size_t i = 0; i < dead_enemies_.size(); ++i) {
			dead_enemies += dead_enemies_[i];
			if (i != dead_enemies_.size() - 1) {
				dead_enemies += "|";
			}
		}

		// serialize the game state data
        archive & current_level_
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
    void load(Archive & archive, [[maybe_unused]] const unsigned int version) {
		std::string dead_enemies;

		// deserialize game state data
		archive & current_level_
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
	int score_;
	int current_level_;
	int player_health_;
	float time_left_;

	Ogre::Vector3 player_position_;
	Ogre::Quaternion player_orientation_;
	std::vector<std::string> dead_enemies_;
};

// tracking is not required since we don't serialize pointers
BOOST_CLASS_TRACKING(GameState, boost::serialization::track_never)
