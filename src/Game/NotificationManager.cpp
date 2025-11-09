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
	text_element_->setMetricsMode(Ogre::GMM_PIXELS);
	text_element_->setParameter("horz_align", "center");  // Center horizontally on screen
	text_element_->setParameter("vert_align", "top");     // Align from top
	text_element_->setLeft(0);  // No horizontal offset (centered by horz_align)
	text_element_->setTop(270);  // 25% of 1080 = 270 pixels from top
	text_element_->setWidth(800);  // Reasonable width for text
	text_element_->setHeight(100);
	text_element_->setParameter("font_name", "BlueHighway");
	text_element_->setParameter("char_height", "32");  // Match font's native size for sharp rendering
	text_element_->setParameter("alignment", "center");  // Center text within the text area
	text_element_->setParameter("colour_top", "1 1 0");  // Bright yellow
	text_element_->setParameter("colour_bottom", "1 1 0");  // Bright yellow
	text_element_->setCaption("");
	
	panel_->addChild(text_element_);
	overlay_->add2D(panel_);
	
	// Initially hide the overlay
	overlay_->hide();
}

void NotificationManager::show_level_notification(int levelNumber, float displayTime, float fadeTime) {
	std::string text = "Level " + std::to_string(levelNumber);
	show_notification(text, displayTime, fadeTime);
}

void NotificationManager::show_save_notification(float displayTime, float fadeTime) {
	show_notification("Saved", displayTime, fadeTime);
}

void NotificationManager::show_load_notification(float displayTime, float fadeTime) {
	show_notification("Loaded", displayTime, fadeTime);
}

void NotificationManager::show_notification(const std::string& text, float displayTime, float fadeTime) {
	// Ensure overlay is created
	if (!overlay_) {
		create_overlay();
	}
	
	if (!overlay_ || !text_element_) {
		return;
	}
	
	active_ = true;
	elapsed_time_ = 0.0f;
	display_time_ = displayTime;
	fade_time_ = fadeTime;
	total_time_ = displayTime + fadeTime;
	
	text_element_->setCaption(text);
	text_element_->setParameter("colour_top", "1 1 0");  // Reset to bright yellow
	text_element_->setParameter("colour_bottom", "1 1 0");
	overlay_->show();
}

void NotificationManager::update(float deltaTime) {
	if (!active_) {
		return;
	}
	
	elapsed_time_ += deltaTime;
	
	if (elapsed_time_ >= total_time_) {
		// Notification finished
		clear();
	} else if (elapsed_time_ >= display_time_) {
		// In fade-out phase
		float fadeProgress = (elapsed_time_ - display_time_) / fade_time_;
		float alpha = 1.0f - fadeProgress;
		
		// Fade to transparent by modifying colour with alpha
		std::string colorStr = std::to_string(alpha) + " " + std::to_string(alpha) + " 0";
		text_element_->setParameter("colour_top", colorStr);
		text_element_->setParameter("colour_bottom", colorStr);
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
