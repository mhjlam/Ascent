#pragma once

#include <Ogre.h>
#include <OgreOverlay.h>
#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>
#include <OgreOverlayElement.h>
#include <string>

/**
 * Manages temporary notification overlays that appear and fade out
 * Used for level start notifications, save/load confirmations, etc.
 */
class NotificationManager {
public:
	static NotificationManager& instance();
	
	// Delete copy and move operations
	NotificationManager(const NotificationManager&) = delete;
	NotificationManager& operator=(const NotificationManager&) = delete;
	NotificationManager(NotificationManager&&) = delete;
	NotificationManager& operator=(NotificationManager&&) = delete;

public:
	// Show a notification with specified display time and fade duration
	void show_level_notification(int level_number, float display_time = 1.5f, float fade_time = 1.0f);
	void show_save_notification(float display_time = 0.5f, float fade_time = 0.5f);
	void show_load_notification(float display_time = 0.5f, float fade_time = 0.5f);
	
	// Update the notification state (fade out animation)
	void update(float delta_time);
	
	// Clear any active notification
	void clear();
	
	// Hide/show notification overlay (for mode switches)
	void hide_overlay();
	void show_overlay();

private:
	NotificationManager();
	~NotificationManager();
	
	void create_overlay();
	void show_notification(const std::string& text, float display_time, float fade_time);

private:
	Ogre::Overlay* overlay_;
	Ogre::OverlayContainer* panel_;
	Ogre::OverlayElement* text_element_;
	Ogre::OverlayManager* overlay_manager_;
	
	// Animation state
	bool active_;
	float elapsed_time_;
	float display_time_;
	float fade_time_;
	float total_time_;
};
