#include "stdafx.h"
#include "GameState.h"
#include "Common.h"
#include "game\Game.h"

void GameState::load()
{
	// first load the proper map
	Game::getInstance()->loadMap(mCurrentLevel);
	// restore the saved values in that map
	Common::mPlayer->setHealth(mPlayerHealth);
	Common::mPlayer->setPosition(mPlayerPosition);
	Game::getInstance()->setTimeLeft(mTimeLeft);
	Common::mPlayer->setScore(mScore);
	Common::mPlayer->getNode()->setOrientation(mPlayerOrientation);
}

void GameState::setCurrentLevel(int currentLevel)
{
	mCurrentLevel = currentLevel;
}

void GameState::setPlayerHealth(int playerHealth)
{
	mPlayerHealth = playerHealth;
}

void GameState::setPlayerPosition(Ogre::Vector3 playerPosition)
{
	mPlayerPosition = playerPosition;
}

void GameState::setPlayerOrientation(Ogre::Quaternion playerOrientation)
{
	mPlayerOrientation = playerOrientation;
}

void GameState::setTimeLeft(float timeLeft)
{
	mTimeLeft = timeLeft;
}

void GameState::setScore(int score)
{
	mScore = score;
}

void GameState::addDeadEnemy(std::string deadEnemyEntityName)
{
	mDeadEnemies.push_back(deadEnemyEntityName);
}

bool GameState::isDead(std::string enemyId) 
{
	// loop through dead enemy ids, if the given enemy id is found, return true
	for (std::vector<std::string>::iterator itr = mDeadEnemies.begin(); itr != mDeadEnemies.end(); ++itr) {
		if (*itr == enemyId) return true;
	}
	// otherwise return false
	return false;
}

void GameState::clearDeadEnemies() 
{
	mDeadEnemies.clear();
}