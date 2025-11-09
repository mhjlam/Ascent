#include "TextRenderer.hpp"
#include <iostream>

TextRenderer& TextRenderer::instance() {
	static TextRenderer instance;  // Meyer's singleton - thread-safe since C++11
	return instance;
}

TextRenderer::TextRenderer() {
	overlay_manager_ = Ogre::OverlayManager::getSingletonPtr();
	overlay_ = overlay_manager_->create("HUD/Text");
	
	panel_ = static_cast<Ogre::OverlayContainer*>(overlay_manager_->createOverlayElement("Panel", "container"));
	panel_->setDimensions(1, 1);
	panel_->setPosition(0, 0);
	
	overlay_->add2D(panel_);
	overlay_->show();
	
	// Create overview map text element (initially hidden)
	overview_map_text_ = overlay_manager_->createOverlayElement("TextArea", "OverviewMapText");
	overview_map_text_->setMetricsMode(Ogre::GMM_RELATIVE);
	overview_map_text_->setParameter("horz_align", "center");  // Center horizontally on screen
	overview_map_text_->setParameter("vert_align", "top");     // Align from top
	overview_map_text_->setLeft(0);  // No horizontal offset (centered by horz_align)
	overview_map_text_->setTop(10.0f / 768.0f);  // Convert pixels to relative
	overview_map_text_->setWidth(800.0f / 1024.0f);
	overview_map_text_->setHeight(100.0f / 768.0f);
	overview_map_text_->setParameter("font_name", "BlueHighway");
	overview_map_text_->setParameter("char_height", "0.025");  // 2.5% of screen height for sharp text
	overview_map_text_->setParameter("alignment", "center");  // Center text within the text area
	overview_map_text_->setParameter("colour_top", "1 1 0");  // Yellow
	overview_map_text_->setParameter("colour_bottom", "1 1 0");  // Yellow
	overview_map_text_->setCaption("Overview Map");
	overview_map_text_->hide();  // Initially hidden
	
	panel_->addChild(overview_map_text_);
}

void TextRenderer::add_crosshair() {
	crosshair_ = Ogre::OverlayManager::getSingleton().getByName("HUD/CrossHair");
	if (crosshair_) {
		show_crosshair();
	}
	else {
		std::cerr << "  ERROR: Crosshair overlay NOT found!\n";
	}
}

void TextRenderer::show_crosshair() {
	if (crosshair_) {
		crosshair_->show();
	}
}

void TextRenderer::hide_crosshair() {
	if (crosshair_) {
		crosshair_->hide();
	}
}

void TextRenderer::add_textbox(const std::string& id, 
							  const std::string& text, 
							  Ogre::Real x, Ogre::Real y, 
							  Ogre::Real width, Ogre::Real height, 
							  const Ogre::ColourValue& color) {
	Ogre::OverlayElement* text_box = overlay_manager_->createOverlayElement("TextArea", id);
    text_box->setMetricsMode(Ogre::GMM_RELATIVE);
    text_box->setPosition(x / 1024.0f, y / 768.0f);  // Convert pixel positions to relative
    text_box->setDimensions(width / 1024.0f, height / 768.0f);  // Convert pixel dimensions to relative
    text_box->setParameter("font_name", "BlueHighway");
    text_box->setParameter("char_height", "0.025");  // 2.5% of screen height for sharp, scalable text
    text_box->setColour(color);
    text_box->setCaption(text);
	panel_->addChild(text_box);
}

void TextRenderer::remove_textbox(const std::string& id) {
	panel_->removeChild(id);
	overlay_manager_->destroyOverlayElement(id);
}
	
void TextRenderer::set_text(const std::string& id, const std::string& text, const Ogre::ColourValue& color) {
	Ogre::OverlayElement* text_box = overlay_manager_->getOverlayElement(id);
	text_box->setColour(color);
	text_box->setCaption(text);
}

const std::string TextRenderer::get_text(const std::string& id) {
	Ogre::OverlayElement* text_box = overlay_manager_->getOverlayElement(id);
	return text_box->getCaption();
}

void TextRenderer::show_overview_map_text() {
	if (overview_map_text_) {
		overview_map_text_->show();
	}
}

void TextRenderer::hide_overview_map_text() {
	if (overview_map_text_) {
		overview_map_text_->hide();
	}
}
