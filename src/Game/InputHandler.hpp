#pragma once

#include <OGRE/Bites/OgreInput.h>

// Forward declaration
class Game;

class InputHandler {
public:
    explicit InputHandler(Game* game);
    
    // Input event handlers
    bool handle_key_pressed(const OgreBites::KeyboardEvent& event);
    bool handle_key_released(const OgreBites::KeyboardEvent& event);
    bool handle_mouse_moved(const OgreBites::MouseMotionEvent& event);
    bool handle_mouse_pressed(const OgreBites::MouseButtonEvent& event);
    bool handle_mouse_released(const OgreBites::MouseButtonEvent& event);
    
private:
    Game* game_;  // Non-owning pointer to Game instance
    
    // Key handler helper methods
    bool handle_escape_key();
    bool handle_game_over_input(const OgreBites::KeyboardEvent& event);
    bool handle_fullscreen_toggle(const OgreBites::KeyboardEvent& event);
    bool handle_save_load_keys(const OgreBites::KeyboardEvent& event);
    bool handle_map_view_toggle(const OgreBites::KeyboardEvent& event);
    bool handle_restart_key(const OgreBites::KeyboardEvent& event);
    void forward_input_to_player(const OgreBites::KeyboardEvent& event);
};
