#pragma once

#include "Enemy.hpp"
#include "Constants.hpp"


class Enemy;

class EnemyFactory {
public: 
	static EnemyFactory& instance();
	
	// Delete copy and move operations
	EnemyFactory(const EnemyFactory&) = delete;
	EnemyFactory& operator=(const EnemyFactory&) = delete;
	EnemyFactory(EnemyFactory&&) = delete;
	EnemyFactory& operator=(EnemyFactory&&) = delete;
	
	void create_enemy(const Ogre::String& name, EntityType enemy_type, Ogre::Vector3 position);

private:
	EnemyFactory();
	~EnemyFactory() = default;

private:
	Ogre::uint num_enemies_;
};
