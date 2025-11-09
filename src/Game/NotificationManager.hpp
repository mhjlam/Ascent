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
	void show_level_notification(int levelNumber, float displayTime = 1.5f, float fadeTime = 1.0f);
	void show_save_notification(float displayTime = 0.5f, float fadeTime = 0.5f);
	void show_load_notification(float displayTime = 0.5f, float fadeTime = 0.5f);
	
	// Update the notification state (fade out animation)
	void update(float deltaTime);
	
	// Clear any active notification
	void clear();

private:
	NotificationManager();
	~NotificationManager();
	
	void create_overlay();
	void show_notification(const std::string& text, float displayTime, float fadeTime);

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
