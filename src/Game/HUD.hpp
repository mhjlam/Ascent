#pragma once

#include <Ogre.h>

class Hud {
public:
    static Hud& instance();

    // Initialization
    void initialize();
    
    // Update HUD values
    void update(float time_left, int health, int score);
    
    // Visibility control
    void show_crosshair();
    void hide_crosshair();
    void show_overview_text();
    void hide_overview_text();
    
private:
    Hud() = default;
    ~Hud() = default;
    
    // Delete copy/move constructors for singleton
    Hud(const Hud&) = delete;
    Hud& operator=(const Hud&) = delete;
    Hud(Hud&&) = delete;
    Hud& operator=(Hud&&) = delete;
    
    // Helper methods
    Ogre::ColourValue get_health_color(int health) const;
    Ogre::ColourValue get_timer_color(float time_left) const;
    std::string format_time(float time_left) const;
};
