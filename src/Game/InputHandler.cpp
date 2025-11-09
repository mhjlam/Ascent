#include "InputHandler.hpp"

#include "Game.hpp"
#include "Common.hpp"
#include "Map/GameWorld.hpp"
#include "Game/GameOverScreen.hpp"
#include "Game/NotificationManager.hpp"
#include "Entities/Player/Player.hpp"


InputHandler::InputHandler(Game* game)
    : game_(game) {
}

bool InputHandler::handle_key_pressed(const OgreBites::KeyboardEvent& event) {
    // ESC always works - highest priority
    if (event.keysym.sym == OgreBites::SDLK_ESCAPE) {
        return handle_escape_key();
    }
    
    // Handle game over screen input (blocks other inputs)
    if (game_->is_game_over()) {
        return handle_game_over_input(event);
    }
    
    // System keys (F11, Alt+Enter for fullscreen)
    if (handle_fullscreen_toggle(event)) {
        return true;
    }
    
    // Save/Load keys (F5, F8)
    if (handle_save_load_keys(event)) {
        return true;
    }
    
    // Map view toggle (TAB)
    if (handle_map_view_toggle(event)) {
        return true;
    }
    
    // Restart key (DELETE/BACKSPACE)
    if (handle_restart_key(event)) {
        return true;
    }
    
    // Forward gameplay input to player
    forward_input_to_player(event);
    
    return true;
}

bool InputHandler::handle_key_released(const OgreBites::KeyboardEvent& event) {
    // Only forward to player if in gameplay mode
    if (!game_->is_map_mode() && !game_->is_game_over() && Common::player) {
        Common::player->inject_key_up(event);
    }
    return true;
}

bool InputHandler::handle_mouse_moved(const OgreBites::MouseMotionEvent& event) {
    // Only forward to player if in gameplay mode
    if (!game_->is_map_mode() && !game_->is_game_over() && Common::player) {
        Common::player->inject_mouse_move(event);
    }
    return true;
}

bool InputHandler::handle_mouse_pressed(const OgreBites::MouseButtonEvent& event) {
    // Only forward to player if in gameplay mode
    if (!game_->is_map_mode() && !game_->is_game_over() && Common::player) {
        Common::player->inject_mouse_down(event);
    }
    return true;
}

bool InputHandler::handle_mouse_released(const OgreBites::MouseButtonEvent& event) {
    // Only forward to player if in gameplay mode
    if (!game_->is_map_mode() && !game_->is_game_over() && Common::player) {
        Common::player->inject_mouse_up(event);
    }
    return true;
}

// Private helper methods

bool InputHandler::handle_escape_key() {
    game_->request_shutdown();
    return true;
}

bool InputHandler::handle_game_over_input(const OgreBites::KeyboardEvent& event) {
    if (event.keysym.sym == OgreBites::SDLK_RETURN || event.keysym.sym == OgreBites::SDLK_KP_ENTER) {
        // Restart game from level 1
        game_->restart_from_beginning();
        return true;
    }
    else if (event.keysym.sym == OgreBites::SDLK_F8) {
        // Try to load saved game
        game_->load_game();
        return true;
    }
    
    // Ignore all other inputs when game over
    return true;
}

bool InputHandler::handle_fullscreen_toggle(const OgreBites::KeyboardEvent& event) {
    bool should_toggle = false;
    
    // F11 key
    if (event.keysym.sym == OgreBites::SDLK_F11) {
        should_toggle = true;
    }
    // Alt+Enter
    else if (event.keysym.sym == OgreBites::SDLK_RETURN 
             && (event.keysym.mod & OgreBites::KMOD_ALT)) {
        should_toggle = true;
    }
    
    if (should_toggle) {
        bool is_currently_fullscreen = Common::render_window->isFullScreen();
        Common::render_window->setFullscreen(!is_currently_fullscreen, 
            Common::render_window->getWidth(), 
            Common::render_window->getHeight());
        return true;
    }
    
    return false;
}

bool InputHandler::handle_save_load_keys(const OgreBites::KeyboardEvent& event) {
    if (event.keysym.sym == OgreBites::SDLK_F5) {
        if (!game_->is_game_over()) {
            game_->save_game();
        }
        return true;
    }
    else if (event.keysym.sym == OgreBites::SDLK_F8) {
        game_->load_game();
        return true;
    }
    
    return false;
}

bool InputHandler::handle_map_view_toggle(const OgreBites::KeyboardEvent& event) {
    if (event.keysym.sym != '\t') {
        return false;
    }
    
    // Ignore TAB when game is over
    if (game_->is_game_over()) {
        return true;
    }
    
    // Toggle map mode
    game_->toggle_map_mode();
    
    // Stop player movement when switching views
    if (Common::player) {
        Common::player->stop_movement();
    }
    
    // Set polygon mode (kept for consistency, though both are solid)
    if (game_->is_map_mode()) {
        Common::overview_camera->setPolygonMode(Ogre::PM_SOLID);
    } else {
        Common::overview_camera->setPolygonMode(Ogre::PM_SOLID);
    }
    
    // Position camera directly above player at fixed height
    if (Common::overview_scene_manager->hasSceneNode("OverviewCameraNode")) {
        auto* camera_node = Common::overview_scene_manager->getSceneNode("OverviewCameraNode");
        if (camera_node && Common::player) {
            Ogre::Vector3 playerPos = Common::player->get_position();
            // Apply 180-degree Y rotation transform: (x,y,z) -> (-x,y,-z)
            camera_node->setPosition(-playerPos.x, 200.0f, -playerPos.z);
        }
    }
    
    game_->swap_view();
    
    return true;
}

bool InputHandler::handle_restart_key(const OgreBites::KeyboardEvent& event) {
    if (event.keysym.sym == OgreBites::SDLK_DELETE || event.keysym.sym == '\b') {
        game_->trigger_map_restart();
        return true;
    }
    
    return false;
}

void InputHandler::forward_input_to_player(const OgreBites::KeyboardEvent& event) {
    // Only forward to player if in gameplay mode
    if (!game_->is_map_mode() && !game_->is_game_over() && Common::player) {
        Common::player->inject_key_down(event);
    }
}
