#include "HUD.hpp"
#include "TextRenderer.hpp"

#include <sstream>
#include <iomanip>

Hud& Hud::instance() {
    static Hud instance;
    return instance;
}

void Hud::initialize() {
    // Create crosshair
    TextRenderer::instance().add_crosshair();
    
    // Create HUD elements with labels and values separated for alignment
    TextRenderer::instance().add_textbox("hud_health_label", "HEALTH:", 10, 10, 200, 50, Ogre::ColourValue::White);
    TextRenderer::instance().add_textbox("hud_health", "100", 60, 10, 200, 50, Ogre::ColourValue::White);

    TextRenderer::instance().add_textbox("hud_timer_label", "TIMER:", 10, 30, 200, 50, Ogre::ColourValue::White);
    TextRenderer::instance().add_textbox("hud_timer", "3:00", 60, 30, 200, 50, Ogre::ColourValue::White);
    
    TextRenderer::instance().add_textbox("hud_score_label", "SCORE:", 10, 50, 200, 50, Ogre::ColourValue::White);
    TextRenderer::instance().add_textbox("hud_score", "0", 60, 50, 200, 50, Ogre::ColourValue::White);
}

void Hud::update(float time_left, int health, int score) {
    // Update health with color coding
    std::stringstream health_ss;
    health_ss << health;
    TextRenderer::instance().set_text("hud_health", health_ss.str(), get_health_color(health));
    
    // Update timer with color coding
    std::string time_str = format_time(time_left);
    TextRenderer::instance().set_text("hud_timer", time_str, get_timer_color(time_left));
    
    // Update score
    std::stringstream score_ss;
    score_ss << score;
    TextRenderer::instance().set_text("hud_score", score_ss.str());
}

void Hud::show_crosshair() {
    TextRenderer::instance().show_crosshair();
}

void Hud::hide_crosshair() {
    TextRenderer::instance().hide_crosshair();
}

void Hud::show_overview_text() {
    TextRenderer::instance().show_overview_map_text();
}

void Hud::hide_overview_text() {
    TextRenderer::instance().hide_overview_map_text();
}

Ogre::ColourValue Hud::get_health_color(int health) const {
    if (health <= 30) {
        return Ogre::ColourValue(1.0f, 0.0f, 0.0f); // Red - critical
    }
    else if (health <= 60) {
        return Ogre::ColourValue(1.0f, 1.0f, 0.0f); // Yellow - damaged
    }
    else {
        return Ogre::ColourValue(0.1f, 1.0f, 0.1f); // Green - healthy
    }
}

Ogre::ColourValue Hud::get_timer_color(float time_left) const {
    if (time_left < 0.0f) {
        return Ogre::ColourValue(1.0f, 0.0f, 0.0f); // Red - time's up
    }
    else if (time_left <= 30.0f) {
        return Ogre::ColourValue(1.0f, 0.0f, 0.0f); // Red - critical
    }
    else if (time_left <= 60.0f) {
        return Ogre::ColourValue(1.0f, 1.0f, 0.0f); // Yellow - warning
    }
    else {
        return Ogre::ColourValue(1.0f, 1.0f, 1.0f); // White - normal
    }
}

std::string Hud::format_time(float time_left) const {
    int total_seconds = static_cast<int>(time_left);
    
    // Clamp to 0:00 if negative
    if (total_seconds < 0) {
        return "0:00";
    }
    
    int minutes = total_seconds / 60;
    int seconds = total_seconds % 60;
    
    std::stringstream ss;
    ss << minutes << ":" << std::setw(2) << std::setfill('0') << seconds;
    return ss.str();
}
