#ifndef _ENEMY_FACTORY_H_
#define _ENEMY_FACTORY_H_

#include "../../stdafx.h"
#include "../../Constants.h"
#include "Enemy.h"

/*
 *  Factory for all the enemy types
 */

class Enemy;

class EnemyFactory 
{
private:
	Ogre::uint mEnemyCounter;
	
private:
	static EnemyFactory* mInstance;
	EnemyFactory();

public: 
	static EnemyFactory* getInstance();
	Enemy* createEnemy(const Ogre::String& name, EntityType enemyType, Ogre::Vector3 position);
};

#endif
