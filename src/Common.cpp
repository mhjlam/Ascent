#include "Common.hpp"

#include "Entities/Player/Player.hpp"


// Non-owning pointer to player (owned by GameWorld's entities_ list)
Player* Common::player = nullptr;
GameState Common::game_state;

// Ogre objects - these are owned and managed by Ogre/ApplicationContext
// We just store non-owning pointers here for convenient access
Ogre::Root*	Common::root = nullptr;
Ogre::Camera* Common::camera = nullptr;
Ogre::RenderWindow*	Common::render_window = nullptr;
Ogre::SceneManager*	Common::scene_manager = nullptr;
Ogre::RaySceneQuery* Common::ray_scene_query = nullptr;

Ogre::Camera* Common::overview_camera = nullptr;
Ogre::SceneManager*	Common::overview_scene_manager = nullptr;


void Common::cleanup() {
	// Player is owned by GameWorld's entities_ list, just clear our non-owning pointer
	player = nullptr;
	
	// RaySceneQuery is owned by us, not Ogre
	if (ray_scene_query && Common::scene_manager) {
		Common::scene_manager->destroyQuery(ray_scene_query);
		ray_scene_query = nullptr;
	}
}
