#include "Game.hpp"

#include <iostream>
#include <fstream>
#include <cstdio>  // for std::remove
#ifdef _WIN32
#include <windows.h>
#endif

#include <OGRE/RTShaderSystem/OgreRTShaderSystem.h>
#include <OGRE/RTShaderSystem/OgreShaderSubRenderState.h>

#include "Common.hpp"

#include "Map/GameWorld.hpp"
#include "Map/LevelOne.hpp"
#include "Map/LevelTwo.hpp"

#include "Game/HUD.hpp"
#include "Game/LoadingBar.hpp"
#include "Game/InputHandler.hpp"
#include "Game/GameOverScreen.hpp"
#include "Game/NotificationManager.hpp"

#include "Entities/Player/Player.hpp"


Game& Game::instance() {
	static Game instance;  // Meyer's singleton - thread-safe since C++11, automatically destroyed at program exit
	return instance;
}


Game::Game()
: OgreBites::ApplicationContext("Ascent")
, resource_config_("resources.cfg")
, map_mode_(false)
, shutdown_requested_(false)
, map_transition_pending_(false)
, time_left_(180.0f)
, overlay_system_(nullptr)
, loading_bar_(nullptr)
, input_handler_(nullptr)
, game_over_(false)
, player_died_(false)
, has_save_file_(false) {
	// OverlaySystem will be created in setup() after Ogre is initialized
	// InputHandler will be created after 'this' is fully constructed
}

Game::~Game() {
    // unique_ptr automatically deletes loading_bar_

    // unload and destroy world - Meyer's singleton will be automatically destroyed
    GameWorld::instance().clear_map();

	// Cleanup Common resources (Player, RaySceneQuery)
	Common::cleanup();
	
	// Cleanup RT Shader System
	Ogre::RTShader::ShaderGenerator::destroy();
	
	// Don't delete overlay_system_ - it's owned by ApplicationContext
	// Don't delete Ogre objects - managed by ApplicationContext
}

void Game::createRoot() {
	// Get the executable directory
#ifdef _WIN32
	char exe_path[MAX_PATH];
	GetModuleFileNameA(NULL, exe_path, MAX_PATH);
	std::string exe_dir(exe_path);
	size_t pos = exe_dir.find_last_of("\\/");
	if (pos != std::string::npos) {
		exe_dir = exe_dir.substr(0, pos);
	}
#else
	std::string exe_dir = ".";
#endif

	// Build path to ogre.cfg in executable directory
	std::string config_path = exe_dir + "/ogre.cfg";
	
	// Call base class to create Root
	OgreBites::ApplicationContext::createRoot();
	
	// Manually load and apply the config from the executable directory
	if (std::ifstream(config_path).good()) {
		Ogre::ConfigFile cfg;
		cfg.load(config_path, "\t:=", false);
		
		// Get render system name
		Ogre::String render_system_name = cfg.getSetting("Render System");
		Ogre::RenderSystem* render_system = getRoot()->getRenderSystemByName(render_system_name);
		
		if (render_system) {
			// Apply all config options for this render system
			const auto& settings = cfg.getSettings(render_system_name);
			for (const auto& setting : settings) {
				try {
					render_system->setConfigOption(setting.first, setting.second);
				}
				catch (...) {
					std::cerr << "Failed to set option: " << setting.first << " = " << setting.second << '\n';
				}
			}
			
			// Set this as the active render system
			getRoot()->setRenderSystem(render_system);
		}
	}
	else {
		std::cerr << "Config file not found at: " << config_path << '\n';
	}
}

bool Game::oneTimeConfig() {
	// We've already configured in createRoot(), so just return true
	// This skips the restoreConfig() call and config dialog
	if (getRoot()->getRenderSystem()) {
		return true;
	}
	// Fallback to default behavior if config didn't work
	return OgreBites::ApplicationContext::oneTimeConfig();
}

OgreBites::NativeWindowPair Game::createWindow(const Ogre::String& name, 
											   [[maybe_unused]] Ogre::uint32 w, 
											   [[maybe_unused]] Ogre::uint32 h, 
											   Ogre::NameValuePairList misc_params) {
	// Call the ApplicationContext method which delegates to the platform-specific implementation (SDL2)
	// Don't call ApplicationContextBase - it won't set up SDL2 properly
	return OgreBites::ApplicationContext::createWindow(name, w, h, misc_params);
}

void Game::setup() {
	// Call base class setup first - this creates the window
	OgreBites::ApplicationContext::setup();
	
	// ApplicationContext has now created root and window for us
	Common::render_window = getRenderWindow();
	Common::root = getRoot();
	
	// Initialize RT Shader System now that we've fixed vcpkg resource paths
	try {
		if (Ogre::RTShader::ShaderGenerator::initialize()) {
			
			// Get shader generator instance
			Ogre::RTShader::ShaderGenerator* shader_generator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
			
			// Configure for OpenGL (no cache needed)
			shader_generator->setTargetLanguage("glsl");
			shader_generator->setShaderCachePath("");
		}
		else {
			std::cerr << "RTSS: Failed to initialize\n";
		}
	} catch (const std::exception& e) {
		std::cerr << "RTSS: Exception: " << e.what() << '\n';
	}
	
	// Get the OverlaySystem singleton (ApplicationContext creates it)
	overlay_system_ = Ogre::OverlaySystem::getSingletonPtr();
	
	// Create InputHandler now that Game is fully constructed
	input_handler_ = std::make_unique<InputHandler>(this);
	
	// Register input and frame listeners IMMEDIATELY after window creation
	addInputListener(this);
	Common::root->addFrameListener(this);

	// Grab the window to capture mouse input for the game
	setWindowGrab(true);
	
	// Clear any existing save file from previous session
	std::remove("save");

	setup_resources();

	LoadingBar initial_loading_bar;
	initial_loading_bar.start(Common::render_window);

	setup_scene_managers();
	setup_cameras();
	
	setup_viewport(Common::scene_manager);

	load_resources();

	create_overlay();
    create_world();
	
	initial_loading_bar.finish();
	
	// Create the loading bar for level transitions
	loading_bar_ = std::make_unique<LoadingBar>();
}

// initialize scene managers for both 'game modes' (default/overview)
void Game::setup_scene_managers() {
	Common::scene_manager = Common::root->createSceneManager();
	Common::overview_scene_manager = Common::root->createSceneManager();
	
	// Set up RTSS for per-pixel lighting now that resource paths are fixed
	Ogre::RTShader::ShaderGenerator* shader_generator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
	if (shader_generator) {
		// Register scene managers to enable shader generation
		shader_generator->addSceneManager(Common::scene_manager);
		shader_generator->addSceneManager(Common::overview_scene_manager);
	} else {
		std::cerr << "RTSS: ShaderGenerator not available - using fixed function\n";
	}
	
	// Disable shadows for simplicity (as in the original code)
	Common::scene_manager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
	Common::overview_scene_manager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
	
	// Add OverlaySystem to scene managers so overlays are rendered
	Common::scene_manager->addRenderQueueListener(overlay_system_);
	Common::overview_scene_manager->addRenderQueueListener(overlay_system_);
}

// initialize cameras for both scene managers
void Game::setup_cameras() {
	// setup playercam
    Common::camera = Common::scene_manager->createCamera("MainCamera");
	Common::camera->setNearClipDistance(5);

    // setup overview camera - give it a different name to avoid conflicts
	Common::overview_camera = Common::overview_scene_manager->createCamera("OverviewCamera");
	Common::overview_camera->setNearClipDistance(100.0f);
	Common::overview_camera->setFarClipDistance(50000.0f); // Must be large enough to see the map from 10000 units up
	
	// Use orthographic projection for a proper top-down map view
	Common::overview_camera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
	// Note: Ortho window will be set in swap_view() based on viewport aspect ratio
	
	// Add ambient light to overview scene so we can see the geometry (reduced from 1.0 to avoid washing out)
	Common::overview_scene_manager->setAmbientLight(Ogre::ColourValue(0.8f, 0.8f, 0.8f));
	Common::overview_scene_manager->setFog(Ogre::FOG_NONE); // Disable any fog
	
    // Attach camera to scene node and orient it to look straight down
    auto* camera_node = Common::overview_scene_manager->getRootSceneNode()->createChildSceneNode("OverviewCameraNode");
    camera_node->attachObject(Common::overview_camera);
    
	// Look straight down (negative Y axis)
    camera_node->setOrientation(Ogre::Quaternion(Ogre::Degree(-90), Ogre::Vector3::UNIT_X));

    // Position at origin initially - will be moved when TAB is pressed
    camera_node->setPosition(0, 10000, 0);
}

// setup a viewport for the specified scene manager
bool Game::setup_viewport(Ogre::SceneManager* scene_manager) {
	if (!scene_manager) {
		return false;
	}

	Common::render_window->removeAllViewports();

	// Use MainCamera for the initial viewport
	Ogre::Camera* camera = Common::camera; // Use the actual camera pointer instead of looking up by name
	Ogre::Viewport* viewport = Common::render_window->addViewport(camera);
    viewport->setBackgroundColour(Ogre::ColourValue::Black);
    
    // Enable RTSS material scheme for per-pixel lighting
    viewport->setMaterialScheme(Ogre::MSN_SHADERGEN);
    
    // Enable overlays on this viewport
    viewport->setOverlaysEnabled(true);

	// set aspect ration to match viewport
	camera->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight()));

	return true;
}

// setup all the resources referenced to in the resources file so they can be used
void Game::setup_resources() {
	// Get the executable directory to locate the config file
#ifdef _WIN32
	char exe_path[MAX_PATH];
	GetModuleFileNameA(NULL, exe_path, MAX_PATH);
	std::string exe_dir(exe_path);
	size_t pos = exe_dir.find_last_of("\\/");
	if (pos != std::string::npos) {
		exe_dir = exe_dir.substr(0, pos);
	}
#else
	std::string exe_dir = ".";
#endif

	// Build full path to resource config file in executable directory
	std::string resource_config_path = exe_dir + "/" + resource_config_;
	
	// Load resource paths from config file
    Ogre::ConfigFile config_file;
    config_file.load(resource_config_path);

	// Helper lambda to expand environment variables in paths
	auto expand_env_vars = [](const std::string& path) -> std::string {
		std::string result = path;
		size_t start = 0;
		while ((start = result.find("$(", start)) != std::string::npos) {
			size_t end = result.find(")", start);
			if (end != std::string::npos) {
				std::string var_name = result.substr(start + 2, end - start - 2);
#ifdef _WIN32
				char* env_value = nullptr;
				size_t len = 0;
				if (_dupenv_s(&env_value, &len, var_name.c_str()) == 0 && env_value != nullptr) {
					result.replace(start, end - start + 1, env_value);
					free(env_value);
				}
				else {
					start = end + 1; // Skip this variable if not found
				}
#else
				const char* env_value = std::getenv(var_name.c_str());
				if (env_value) {
					result.replace(start, end - start + 1, env_value);
				}
				else {
					start = end + 1; // Skip this variable if not found
				}
#endif
			}
			else {
				break;
			}
		}
		return result;
	};

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator sit = config_file.getSectionIterator();

    Ogre::String section_name, type_name, arch_name;
    while (sit.hasMoreElements()) {
        section_name = sit.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap* settings = sit.getNext();
        
		for (const auto& [type, arch] : *settings) {
            type_name = type;
            arch_name = expand_env_vars(arch); // Expand environment variables
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch_name, type_name, section_name);
        }
    }
}

void Game::load_resources() {
	// default mipmap level
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

	// make resources ready for use
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	// set up Ray Query
	Common::ray_scene_query = Common::scene_manager->createRayQuery(Ogre::Ray());
}

void Game::create_overlay() {
	// Initialize HUD
	Hud::instance().initialize();
}

// initialize maps
void Game::create_world() {
    GameWorld::instance().register_map(std::make_unique<LevelOne>());
    GameWorld::instance().register_map(std::make_unique<LevelTwo>());
    GameWorld::instance().load_next_map(); // will load first map
    time_left_ = 180.0f;
    map_transition_pending_ = false;  // Ensure clean state at game start
    
    // Don't show notification here - it will be shown in setup()
}

// restart current map
void Game::trigger_map_restart() {
    time_left_ = 180.0f;
    game_over_ = false;
    player_died_ = false;
    map_transition_pending_ = false;  // Clear any pending transition
    
    // Hide game over screen if it's showing
    GameOverScreen::instance().hide();
    
    if (map_mode_) {
        swap_view();
	}
    
    // Show loading screen
    if (loading_bar_) {
        loading_bar_->show_level_loading(Common::render_window, "Restarting Level...");
    }
    
    GameWorld::instance().reload_current_map();
    
    // Hide loading screen
    if (loading_bar_) {
        loading_bar_->hide_level_loading();
    }
    
    // Show level notification
    int current_level = static_cast<int>(GameWorld::instance().get_current_map_index()) + 1;
    NotificationManager::instance().show_level_notification(current_level);
}

// end the current map
void Game::trigger_map_end() {
    // Don't immediately transition - set flag to defer until next frame
    // This prevents destroying the player while it's still executing update()
    // Only set the flag if not already pending (prevent double-trigger)
    if (!map_transition_pending_) {
        map_transition_pending_ = true;
    }
}

// updated per frame
bool Game::frameRenderingQueued(const Ogre::FrameEvent& event) {
    if (Common::render_window && Common::render_window->isClosed()) {
		return false;
	}

	if (shutdown_requested_) {
		return false;
	}

	// Handle deferred map transition FIRST, before any updates
	if (map_transition_pending_) {
		map_transition_pending_ = false;
		
		Common::game_state.clear_dead_enemies();
		time_left_ = 180.0f;
		game_over_ = false;      // Reset game over state for new level
		player_died_ = false;    // Reset death state for new level
		
		if (map_mode_) {
			swap_view();
		}
		
		// Check if there are more maps BEFORE loading (index hasn't been incremented yet)
		if (GameWorld::instance().has_more_maps()) {
			// Show loading screen for next level
			if (loading_bar_) {
				loading_bar_->show_level_loading(Common::render_window, "Loading Next Level...");
			}
			
			GameWorld::instance().load_next_map();
			
			// Hide loading screen
			if (loading_bar_) {
				loading_bar_->hide_level_loading();
			}
			
			// Show level notification
			int current_level = static_cast<int>(GameWorld::instance().get_current_map_index()) + 1;  // +1 for display (1-based)
			NotificationManager::instance().show_level_notification(current_level);
		}
		else {
			// No more maps - game completed, show victory screen
			game_over_ = true;
			int final_score = Common::player ? Common::player->get_score() : 0;
			GameOverScreen::instance().show_victory_screen(final_score, has_save_file_);
		}
	}

	// Update notification manager
	NotificationManager::instance().update(event.timeSinceLastFrame);

	// end-game conditions
	if (time_left_ < -2.0f && !game_over_) {
		game_over_ = true;
		int final_score = Common::player ? Common::player->get_score() : 0;
		GameOverScreen::instance().show_death_screen(final_score, has_save_file_);
	}
	
	// Check for player death
    if (Common::player && Common::player->get_health() <= 0 && !player_died_ && !game_over_) {
		player_died_ = true;
		game_over_ = true;
		int final_score = Common::player->get_score();
		GameOverScreen::instance().show_death_screen(final_score, has_save_file_);
		// Don't auto-restart, wait for player input
		return true;
	}

    // update game world and timer (only when not in mapmode and not game over)
    if (!map_mode_ && !game_over_) {
        GameWorld::instance().update(event.timeSinceLastFrame);
        time_left_ -= event.timeSinceLastFrame;
    }
    
    // Update player indicator position and orientation in overview scene
    if (Common::player && Common::overview_scene_manager->hasSceneNode("PlayerOverviewNode")) {
        Ogre::SceneNode* player_overview_node = Common::overview_scene_manager->getSceneNode("PlayerOverviewNode");
        player_overview_node->setPosition(Common::player->get_position());
        
        // Rotate arrow to point in player's facing direction
        // Get player's yaw (rotation around Y axis)
        Ogre::Quaternion player_orientation = Common::player->get_node()->getOrientation();
        player_overview_node->setOrientation(player_orientation);
    }
    
    // Update overview camera position to follow player (if in map mode)
    // Camera orientation stays fixed looking straight down
    if (map_mode_ && Common::player && Common::overview_scene_manager->hasSceneNode("OverviewCameraNode")) {
        auto* camera_node = Common::overview_scene_manager->getSceneNode("OverviewCameraNode");
        if (camera_node) {
            Ogre::Vector3 playerPos = Common::player->get_position();
            // Apply 180-degree Y rotation transform: (x,y,z) -> (-x,y,-z)
            camera_node->setPosition(-playerPos.x, 15000.0f, -playerPos.z);
            
            // Update overview visibility based on player's vertical position
            GameWorld::instance().update_overview_visibility(playerPos.y);
        }
    }

    update_overlay();
    return true;
}

// swap 'game mode' between default/overview
void Game::swap_view() {
	if (map_mode_) {
		// Switching to overview/map mode
		Ogre::Viewport* viewport = Common::render_window->getViewport(0);
		
		viewport->setCamera(Common::overview_camera);
		viewport->setBackgroundColour(Ogre::ColourValue(0.2f, 0.2f, 0.3f)); // Dark blue background for contrast
		
		// Use default material scheme for overview (no RTSS shaders needed for simple unlit materials)
		viewport->setMaterialScheme("");  // Empty string = default scheme
		
		// Set orthographic window size based on viewport aspect ratio
		Ogre::Real aspect_ratio = Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight());
		Ogre::Real view_height = 10000.0f; // Large window to see big area around player (geometry is 0.01x scale)
		Ogre::Real view_width = view_height * aspect_ratio;
		Common::overview_camera->setOrthoWindow(view_width, view_height);
		
		// Debug: Check scene manager contents
		Ogre::SceneManager::MovableObjectIterator it = Common::overview_scene_manager->getMovableObjectIterator("Entity");
		int entity_count = 0;
		while (it.hasMoreElements()) {
			it.getNext();
			entity_count++;
		}
		
		Common::overview_camera->setAspectRatio(aspect_ratio);
		Hud::instance().hide_crosshair();
		Hud::instance().show_overview_text();
		NotificationManager::instance().hide_overlay();
	}
    else {
		// Switching back to normal game view
		Ogre::Viewport* viewport = Common::render_window->getViewport(0);
	    viewport->setCamera(Common::camera);
		viewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f)); // Black background for normal view
		
		// Restore RTSS material scheme for per-pixel lighting in gameplay
		viewport->setMaterialScheme(Ogre::MSN_SHADERGEN);
		
		Hud::instance().show_crosshair();
		Hud::instance().hide_overview_text();
		NotificationManager::instance().show_overlay();
	}
}

// update overlay
void Game::update_overlay() {
	// Get current values (default to 0 if player doesn't exist)
	int health = Common::player ? Common::player->get_health() : 0;
	int score = Common::player ? Common::player->get_score() : 0;
	
	// Update HUD
	Hud::instance().update(time_left_, health, score);
}

void Game::load_map(int map_id) {
    // Load a specific map by ID (used during save game restoration)
    // map_id is 1-based, but GameWorld uses 0-based indexing
    if (map_id > 0) {
        size_t index = static_cast<size_t>(map_id - 1);
        GameWorld::instance().load_map_by_index(index);
    }
}

void Game::set_time_left(float time_left) {
	time_left_ = time_left;
}

void Game::save() {
	std::ofstream ofs("save");
	boost::archive::text_oarchive archive(ofs);
	
	// current_map_index_ is 0-based, but GameState uses 1-based indexing
	int current_level = static_cast<int>(GameWorld::instance().get_current_map_index()) + 1;
	Common::game_state.set_current_level(current_level);
	Common::game_state.set_player_health(Common::player->get_health());
	Common::game_state.set_player_location(Common::player->get_position());
	Common::game_state.set_time_left(time_left_);
	Common::game_state.set_score(Common::player->get_score());
	Common::game_state.set_player_orientation(Common::player->get_node()->getOrientation());

	archive << Common::game_state;
	
	has_save_file_ = true;
	
	// Show save notification
	NotificationManager::instance().show_save_notification();
}

void Game::load() {
	std::ifstream ifs("save");
	
	// Check if save file exists
	if (!ifs.good()) {
		std::cerr << "No save file found!\n";
		return;
	}
	
	try {
		// Show loading screen
		if (loading_bar_) {
			loading_bar_->show_level_loading(Common::render_window, "Loading Save...");
		}
		
		boost::archive::text_iarchive archive(ifs);
		archive >> Common::game_state;
		// restore previously saved state
		Common::game_state.load();
		
		// Hide loading screen
		if (loading_bar_) {
			loading_bar_->hide_level_loading();
		}
		
		// Reset game over state
		game_over_ = false;
		player_died_ = false;
		map_transition_pending_ = false;  // Clear any pending transition
		GameOverScreen::instance().hide();
		
		// Show load notification
		NotificationManager::instance().show_load_notification();
	}
	catch (const std::exception& e) {
		std::cerr << "Error loading save file: " << e.what() << '\n';
		if (loading_bar_) {
			loading_bar_->hide_level_loading();
		}
	}
}

// Public methods for InputHandler
void Game::request_shutdown() {
	shutdown_requested_ = true;
	getRoot()->queueEndRendering();
}

void Game::restart_from_beginning() {
	game_over_ = false;
	player_died_ = false;
	map_transition_pending_ = false;  // Clear any pending transition
	GameOverScreen::instance().hide();
	
	// Reset to first level
	GameWorld::instance().load_map_by_index(0);
	time_left_ = 180.0f;
	
	// Show level notification
	NotificationManager::instance().show_level_notification(1);
}

void Game::save_game() {
	save();
}

void Game::load_game() {
	load();
}

void Game::toggle_map_mode() {
	map_mode_ = !map_mode_;
}

bool Game::keyPressed(const OgreBites::KeyboardEvent& event) {
	return input_handler_->handle_key_pressed(event);
}

bool Game::keyReleased(const OgreBites::KeyboardEvent& event) {
	return input_handler_->handle_key_released(event);
}

bool Game::mouseMoved(const OgreBites::MouseMotionEvent& event) {
	return input_handler_->handle_mouse_moved(event);
}

bool Game::mousePressed(const OgreBites::MouseButtonEvent& event) {
	return input_handler_->handle_mouse_pressed(event);
}

bool Game::mouseReleased(const OgreBites::MouseButtonEvent& event) {
	return input_handler_->handle_mouse_released(event);
}

// main start of this class
void Game::run() {
#ifdef _DEBUG
	resource_config_ = "resources_d.cfg";
#else
	resource_config_ = "resources.cfg";
#endif
	
	// ApplicationContext handles the initialization and rendering loop
	initApp();
	getRoot()->startRendering();
	closeApp();
}

void Game::shutdown() {
	// Called when application is closing
	shutdown_requested_ = true;
}
