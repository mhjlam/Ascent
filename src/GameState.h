#ifndef GAMESTATE_H_
#define GAMESTATE_H_

#include "stdafx.h"

#include <string>
#include <vector>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/algorithm/string.hpp>

/*
 *  GameState is a serializable class which contains the information required to load and save the game
 */

class GameState 
{
	// contains the current map the player is in
	int mCurrentLevel;
	// contains the players current health
	int mPlayerHealth;
	// contains the players position
	Ogre::Vector3 mPlayerPosition;
	// contains the players orientation
	Ogre::Quaternion mPlayerOrientation;
	// contains the time the player has left to finish the level
	float mTimeLeft;
	// contains the amount of credits the player has acquired
	int mScore;
	// contains the enemies that are killed and should therefore not be created
	std::vector<std::string> mDeadEnemies;

	// use boost to implement serialization
	friend class boost::serialization::access;
	template<class Archive>
    void save(Archive & ar, const unsigned int version) const
    {
		// create a string that contains the enemies that are killed, separated by delimiter |
		std::string sDeadEnemies;
		for (std::vector<std::string>::const_iterator itr = mDeadEnemies.begin(); itr != mDeadEnemies.end(); ++itr) 
		{
			sDeadEnemies += *itr;
			if (itr != mDeadEnemies.end() - 1) sDeadEnemies += "|";
	    }

		// serialize the game state data
        ar 
			& mCurrentLevel
			& mPlayerHealth
		
			& mPlayerPosition.x
			& mPlayerPosition.y
			& mPlayerPosition.z
		
			& mPlayerOrientation.w
			& mPlayerOrientation.x
			& mPlayerOrientation.y
			& mPlayerOrientation.z

			& mTimeLeft
			& mScore
			& sDeadEnemies;
	}

    template<class Archive>
    void load(Archive & ar, const unsigned int version)
    {
		std::string sDeadEnemies;

		// deserialize game state data
		ar 
			& mCurrentLevel
			& mPlayerHealth
		
			& mPlayerPosition.x
			& mPlayerPosition.y
			& mPlayerPosition.z
		
			& mPlayerOrientation.w
			& mPlayerOrientation.x
			& mPlayerOrientation.y
			& mPlayerOrientation.z

			& mTimeLeft
			& mScore
			& sDeadEnemies;
		
		// split the killed enemies string with delimiter | and store them in the vector
		boost::split(mDeadEnemies, sDeadEnemies, boost::is_any_of("|"));
    }
	// tell boost that there's a load and save method instead of just one serialize method
    BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
	// loads the current level using the game state data
	void load();
	// setters
	void setCurrentLevel(int currentLevel);
	void setPlayerHealth(int playerHealth);
	void setPlayerPosition(Ogre::Vector3 playerPosition);
	void setPlayerOrientation(Ogre::Quaternion playerOrientation);
	void setTimeLeft(float timeLeft);
	void setScore(int score);
	// adds a dead enemies entity name to the list
	void addDeadEnemy(std::string deadEnemyEntityName);
	// returns true when the given id is dead according to this gamestate
	// false otherwise
	bool isDead(std::string enemyId);
	// clears the dead enemies list
	void clearDeadEnemies();
};
// disable tracking to avoid a warning from boost. tracking is not required since we
// don't serialize pointers
BOOST_CLASS_TRACKING(GameState, boost::serialization::track_never)

#endif