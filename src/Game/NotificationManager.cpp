#include "NotificationManager.hpp"

#include <iostream>


NotificationManager& NotificationManager::instance() {
	static NotificationManager instance;
	return instance;
}

NotificationManager::NotificationManager()
: overlay_(nullptr)
, panel_(nullptr)
, text_element_(nullptr)
, overlay_manager_(nullptr)
, active_(false)
, elapsed_time_(0.0f)
, display_time_(0.0f)
, fade_time_(0.0f)
, total_time_(0.0f) {
	// Don't create overlay here - defer until first use
}

NotificationManager::~NotificationManager() {
	if (overlay_ && overlay_manager_) {
		overlay_manager_->destroy(overlay_);
	}
}

void NotificationManager::create_overlay() {
	if (overlay_) {
		return; // Already created
	}
	
	overlay_manager_ = Ogre::OverlayManager::getSingletonPtr();
	
	if (!overlay_manager_) {
		return;
	}
	
	// Try to get existing overlay first (in case of level reload)
	try {
		overlay_ = overlay_manager_->getByName("NotificationOverlay");
		if (overlay_) {
			panel_ = static_cast<Ogre::OverlayContainer*>(overlay_manager_->getOverlayElement("NotificationPanel"));
			text_element_ = overlay_manager_->getOverlayElement("NotificationText");
			return;
		}
	}
	catch (...) {
		// Overlay doesn't exist yet, create it
	}
	
	// Create overlay
	overlay_ = overlay_manager_->create("NotificationOverlay");
	overlay_->setZOrder(600);  // High z-order to appear on top
	
	// Create panel container
	panel_ = static_cast<Ogre::OverlayContainer*>(
		overlay_manager_->createOverlayElement("Panel", "NotificationPanel"));
	panel_->setMetricsMode(Ogre::GMM_PIXELS);
	panel_->setPosition(0, 0);
	panel_->setDimensions(1920, 1080);  // Full screen
	
	// Create text element - use alignment to center it on screen
	text_element_ = overlay_manager_->createOverlayElement("TextArea", "NotificationText");
	text_element_->setMetricsMode(Ogre::GMM_RELATIVE);
	text_element_->setParameter("horz_align", "center");  // Center horizontally on screen
	text_element_->setParameter("vert_align", "top");     // Align from top
	text_element_->setLeft(0);  // No horizontal offset (centered by horz_align)
	text_element_->setTop(0.25f);  // 25% from top
	text_element_->setWidth(0.8f);  // 80% of screen width
	text_element_->setHeight(0.1f);  // 10% of screen height
	text_element_->setParameter("font_name", "BlueHighway");
	text_element_->setParameter("char_height", "0.035");  // 3.5% of screen height for sharp, visible notifications
	text_element_->setParameter("alignment", "center");  // Center text within the text area
	text_element_->setParameter("colour_top", "1 1 0");  // Bright yellow
	text_element_->setParameter("colour_bottom", "1 1 0");  // Bright yellow
	text_element_->setCaption("");
	
	panel_->addChild(text_element_);
	overlay_->add2D(panel_);
	
	// Initially hide the overlay
	overlay_->hide();
}

void NotificationManager::show_level_notification(int level_number, float display_time, float fade_time) {
	std::string text = "Level " + std::to_string(level_number);
	show_notification(text, display_time, fade_time);
}

void NotificationManager::show_save_notification(float display_time, float fade_time) {
	show_notification("Saved", display_time, fade_time);
}

void NotificationManager::show_load_notification(float display_time, float fade_time) {
	show_notification("Loaded", display_time, fade_time);
}

void NotificationManager::show_notification(const std::string& text, float display_time, float fade_time) {
	// Ensure overlay is created
	if (!overlay_) {
		create_overlay();
	}
	
	if (!overlay_ || !text_element_) {
		return;
	}
	
	active_ = true;
	elapsed_time_ = 0.0f;
	display_time_ = display_time;
	fade_time_ = fade_time;
	total_time_ = display_time + fade_time;
	
	text_element_->setCaption(text);
	text_element_->setParameter("colour_top", "1 1 0");  // Reset to bright yellow
	text_element_->setParameter("colour_bottom", "1 1 0");
	overlay_->show();
}

void NotificationManager::update(float delta_time) {
	if (!active_) {
		return;
	}
	
	elapsed_time_ += delta_time;
	
	if (elapsed_time_ >= total_time_) {
		// Notification finished
		clear();
	} else if (elapsed_time_ >= display_time_) {
		// In fade-out phase
		float fade_progress = (elapsed_time_ - display_time_) / fade_time_;
		float alpha = 1.0f - fade_progress;
		
		// Fade to transparent by modifying colour with alpha
		std::string color = std::to_string(alpha) + " " + std::to_string(alpha) + " 0";
		text_element_->setParameter("colour_top", color);
		text_element_->setParameter("colour_bottom", color);
	}
}

void NotificationManager::clear() {
	if (!overlay_) {
		return;
	}
	
	active_ = false;
	elapsed_time_ = 0.0f;
	overlay_->hide();
}

void NotificationManager::hide_overlay() {
	if (overlay_) {
		overlay_->hide();
	}
}

void NotificationManager::show_overlay() {
	if (overlay_ && active_) {
		overlay_->show();
	}
}
