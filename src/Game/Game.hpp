#pragma once

#include <memory>
#include <Ogre.h>
#include <OGRE/Bites/OgreInput.h>
#include <OGRE/Bites/OgreApplicationContext.h>
#include <OGRE/Overlay/OgreOverlaySystem.h>

#include "Map/Map.hpp"
#include "GameState.hpp"

// Forward declarations
class LoadingBar;

class Game 
: public OgreBites::ApplicationContext
, public OgreBites::InputListener {
public:
    static Game* instance();
	
public:
	void load_map(int map_id);
	void set_time_left(float time_left);
	void trigger_map_end();
	void trigger_map_restart();
    void run();

private: // singleton
	static Game* instance_;
	Game();
	~Game();

private:
	void update_overlay();
	void swap_view();
	void save();
	void load();

	void setup_scene_managers();
    void setup_cameras();
	bool setup_viewport(Ogre::SceneManager* scene_manager);
	void setup_resources();
	void load_resources();
	void create_overlay();
    void create_world();

    // Override ApplicationContext methods
    void createRoot() override;
    bool oneTimeConfig() override;
    void setup() override;
    bool frameRenderingQueued(const Ogre::FrameEvent& event) override;
	void shutdown() override;
	
	// Override to configure window settings
	OgreBites::NativeWindowPair createWindow(const Ogre::String& name, 
		Ogre::uint32 w, Ogre::uint32 h, Ogre::NameValuePairList misc_params) override;

	// Override InputListener methods
    bool keyPressed(const OgreBites::KeyboardEvent& event) override;
    bool keyReleased(const OgreBites::KeyboardEvent& event) override;
    bool mouseMoved(const OgreBites::MouseMotionEvent& event) override;
    bool mousePressed(const OgreBites::MouseButtonEvent& event) override;
    bool mouseReleased(const OgreBites::MouseButtonEvent& event) override;

private:
	// GAME SPECIFIC
	bool map_mode_;
	bool shutdown_requested_;
	Ogre::Real time_left_;
	
	// config files
	Ogre::String resource_config_;
	
	// Overlay system (required for overlays to render in modern Ogre)
	Ogre::OverlaySystem* overlay_system_;
	
	// Loading bar for level transitions
	std::unique_ptr<LoadingBar> loading_bar_;
	
	// Game over state
	bool game_over_;
	bool player_died_;
	bool has_save_file_;  // Track if a save file exists in this session
};
