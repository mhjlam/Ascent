#pragma once

#include "Constants.hpp"
#include "Entities/Projectiles/Projectile.hpp"


class ProjectileFactory {
public: 
	static ProjectileFactory& instance();
	
	// Delete copy and move operations
	ProjectileFactory(const ProjectileFactory&) = delete;
	ProjectileFactory& operator=(const ProjectileFactory&) = delete;
	ProjectileFactory(ProjectileFactory&&) = delete;
	ProjectileFactory& operator=(ProjectileFactory&&) = delete;

public:
	void create_projectile(EntityType projectileType, 
						   const Ogre::Vector3& position, 
						   const Ogre::Vector3& target, 
						   const Ogre::Quaternion& orientation, 
						   bool targetIsDirection = false);
	
	Ogre::uint get_num_enemy_projectiles() const { 	return num_enemy_projectiles_; }
	Ogre::uint get_num_player_projectiles() const { return num_player_projectiles_; }

private:
	ProjectileFactory();
	~ProjectileFactory() = default;

private:
	Ogre::uint num_player_projectiles_;
	Ogre::uint num_enemy_projectiles_;
};
