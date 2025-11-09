#pragma once

#include <Ogre.h>

#include "GameState.hpp"

class Player;  // Forward declaration

namespace Common {
	extern Player* player;
	extern GameState game_state;

	extern Ogre::Root*			root;
	extern Ogre::Camera*		camera;
	extern Ogre::RenderWindow*	render_window;
	extern Ogre::SceneManager*	scene_manager;
	extern Ogre::RaySceneQuery*	ray_scene_query;
	
	extern Ogre::Camera*		overview_camera;
	extern Ogre::SceneManager*	overview_scene_manager;


	void cleanup();
};
