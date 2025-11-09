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
}

void TextRenderer::add_crosshair() {
	crosshair_ = Ogre::OverlayManager::getSingleton().getByName("HUD/CrossHair");
	if (crosshair_) {
		show_crosshair();
	}
	else {
		std::cerr << "  ERROR: Crosshair overlay NOT found!" << std::endl;
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
	Ogre::OverlayElement* textBox = overlay_manager_->createOverlayElement("TextArea", id);
    textBox->setMetricsMode(Ogre::GMM_PIXELS);
    textBox->setPosition(x, y);
    textBox->setDimensions(width, height);
    textBox->setParameter("font_name", "BlueHighway");
    textBox->setParameter("char_height", "32");  // Much larger font
    textBox->setColour(color);
    textBox->setCaption(text);
	panel_->addChild(textBox);
}

void TextRenderer::remove_textbox(const std::string& id) {
	panel_->removeChild(id);
	overlay_manager_->destroyOverlayElement(id);
}
	
void TextRenderer::set_text(const std::string& id, const std::string& text, const Ogre::ColourValue& color) {
	Ogre::OverlayElement* textBox = overlay_manager_->getOverlayElement(id);
	textBox->setColour(color);
	textBox->setCaption(text);
}

const std::string TextRenderer::get_text(const std::string& id) {
	Ogre::OverlayElement* textBox = overlay_manager_->getOverlayElement(id);
	return textBox->getCaption();
}
