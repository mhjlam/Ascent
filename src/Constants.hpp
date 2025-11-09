#pragma once

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

enum class QueryFlags { 
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

namespace Ascent {
	struct delete_ptr {
		template <typename T>
		void operator()(T& p) {
			if (p) {
				delete p;
				p = nullptr;
			}
		}
	};

	template <typename T, typename U>
	struct pair {
		T first;
		U second;

		pair(T t, U u) : first(t), second(u) {}
	};
}
