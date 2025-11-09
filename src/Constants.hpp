#pragma once

#include <Ogre.h>

enum class BrushType {
	Ceiling,
	Wall,
	Floor,
	Start,
	Exit,
};

enum class EntityType {
    Brush,
	PlayerEntity,  // Renamed from Player to avoid conflict with class Player
    EnemyStationary, 
	EnemyPatrol, 
	EnemyFlying,
    EnemyProjectileType,  // Renamed to avoid conflict with class EnemyProjectile
	PlayerProjectileType, // Renamed to avoid conflict with class PlayerProjectile
};

enum class QueryFlags : Ogre::uint32 { 
	Wall 				= 1<<0,
	Enemy 				= 1<<1,
	EnemyProjectile 	= 1<<2,
	PlayerProjectile 	= 1<<3,
	Pickup 				= 1<<4,
};

// Bitwise operators for QueryFlags
constexpr QueryFlags operator|(QueryFlags a, QueryFlags b) {
	return static_cast<QueryFlags>(static_cast<int>(a) | static_cast<int>(b));
}

constexpr bool operator==(QueryFlags a, QueryFlags b) {
	return static_cast<int>(a) == static_cast<int>(b);
}

// Helper to convert QueryFlags to Ogre::uint32
constexpr Ogre::uint32 to_ogre_flags(QueryFlags flags) {
	return static_cast<Ogre::uint32>(flags);
}
