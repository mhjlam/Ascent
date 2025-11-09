#pragma once

#include <Ogre.h>
#include <OgreOverlay.h>
#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>
#include <OgreOverlayElement.h>
#include <string>

class TextRenderer {
public:
	static TextRenderer& instance();
	
	// Delete copy and move operations
	TextRenderer(const TextRenderer&) = delete;
	TextRenderer& operator=(const TextRenderer&) = delete;
	TextRenderer(TextRenderer&&) = delete;
	TextRenderer& operator=(TextRenderer&&) = delete;

public:
	void add_crosshair();
	void hide_crosshair();
	void show_crosshair();

	void add_textbox(const std::string& id, 
					 const std::string& text, 
					 Ogre::Real x, Ogre::Real y, 
					 Ogre::Real width, Ogre::Real height, 
					 const Ogre::ColourValue& color = Ogre::ColourValue(1.0f, 1.0f, 1.0f));
	void remove_textbox(const std::string& id);
	
	const std::string get_text(const std::string& id);
	void set_text(const std::string& id, 
		          const std::string& text, 
				  const Ogre::ColourValue& color = Ogre::ColourValue(1.0f, 1.0f, 1.0f));
	
private:
	TextRenderer();
	~TextRenderer() = default;

private:
	Ogre::Overlay* overlay_;
	Ogre::Overlay* crosshair_;
	Ogre::OverlayContainer*	panel_;
	Ogre::OverlayManager* overlay_manager_;
};
