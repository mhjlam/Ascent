#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include "stdafx.h"

/*
 *  All constant variables, including the delete_ptr functor
 */

enum BrushType			{ BRUSH_CEIL, BRUSH_WALL, BRUSH_FLOOR, BRUSH_START, BRUSH_EXIT };
enum EntityType         { 
                          ENT_PLAYER,
                          ENT_ENEMY_STATIONARY, ENT_ENEMY_PATROL, ENT_ENEMY_FLYING,
                          ENT_PROJECTILE_ENEMY, ENT_PROJECTILE_PLAYER,
                          ENT_BRUSH
                        };

enum QueryFlags 
{ 
	COLLIDABLE_WALL				 = 1<<0,
	COLLIDABLE_ENEMY			 = 1<<1,
	COLLIDABLE_ENEMY_PROJECTILE  = 1<<2,
	COLLIDABLE_PLAYER_PROJECTILE = 1<<3,
	COLLIDABLE_PICKUP			 = 1<<4
};

namespace Ascent
{
	struct delete_ptr
	{
		template <typename T>
		void operator()(T& p)
		{
			if (p)
			{
				delete p;
				p = NULL;
			}
		}
	};

	template <typename T, typename U>
	struct pair
	{
		T first;
		U second;

		pair(T t, U u) : first(t), second(u) {}
	};
}


#endif