#include "GameOverScreen.hpp"

#include <iostream>


GameOverScreen& GameOverScreen::instance() {
	static GameOverScreen instance;
	return instance;
}

GameOverScreen::GameOverScreen()
: visible_(false)
, overlay_(nullptr)
, panel_(nullptr)
, overlay_manager_(nullptr)
, title_text_(nullptr)
, score_text_(nullptr)
, restart_prompt_(nullptr)
, load_prompt_(nullptr)
, exit_prompt_(nullptr)
, title_shadow_(nullptr)
, score_shadow_(nullptr)
, restart_shadow_(nullptr)
, load_shadow_(nullptr)
, exit_shadow_(nullptr) {}

GameOverScreen::~GameOverScreen() {
	if (overlay_ && overlay_manager_) {
		overlay_manager_->destroy(overlay_);
	}
}

void GameOverScreen::create_overlay() {
	if (overlay_) {
		return; // Already created
	}
	
	overlay_manager_ = Ogre::OverlayManager::getSingletonPtr();
	
	if (!overlay_manager_) {
		return;
	}
	
	// Create overlay
	overlay_ = overlay_manager_->create("GameOverOverlay");
	overlay_->setZOrder(650);  // High z-order (max is 655)
	
	// Create semi-transparent background panel
	panel_ = static_cast<Ogre::OverlayContainer*>(
		overlay_manager_->createOverlayElement("Panel", "GameOverPanel"));
	panel_->setMetricsMode(Ogre::GMM_PIXELS);
	panel_->setPosition(0, 0);
	panel_->setDimensions(1920, 1080);
	panel_->setParameter("colour", "0 0 0 0.95");  // Much darker background for better readability
	
	// Create container for shadows (lower Z-order)
	Ogre::OverlayContainer* shadow_container = static_cast<Ogre::OverlayContainer*>(
		overlay_manager_->createOverlayElement("Panel", "ShadowContainer"));
	shadow_container->setMetricsMode(Ogre::GMM_PIXELS);
	shadow_container->setPosition(0, 0);
	shadow_container->setDimensions(1920, 1080);
	
	// Create container for text (higher Z-order)
	Ogre::OverlayContainer* text_container = static_cast<Ogre::OverlayContainer*>(
		overlay_manager_->createOverlayElement("Panel", "TextContainer"));
	text_container->setMetricsMode(Ogre::GMM_PIXELS);
	text_container->setPosition(0, 0);
	text_container->setDimensions(1920, 1080);
	
	// Create title text shadow
	title_shadow_ = overlay_manager_->createOverlayElement("TextArea", "GameOverTitleShadow");
	title_shadow_->setMetricsMode(Ogre::GMM_RELATIVE);
	title_shadow_->setParameter("horz_align", "center");
	title_shadow_->setParameter("vert_align", "top");
	title_shadow_->setLeft(3.0f / 1024.0f);  // Offset for shadow effect
	title_shadow_->setTop(203.0f / 768.0f);
	title_shadow_->setWidth(1.0f);
	title_shadow_->setHeight(0.13f);
	title_shadow_->setParameter("font_name", "BlueHighway");
	title_shadow_->setParameter("char_height", "0.08");  // 8% of screen height for large title
	title_shadow_->setParameter("alignment", "center");
	title_shadow_->setColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 0.8f));  // Black shadow
	
	// Create title text (Game Over / Victory) - centered
	title_text_ = overlay_manager_->createOverlayElement("TextArea", "GameOverTitle");
	title_text_->setMetricsMode(Ogre::GMM_RELATIVE);
	title_text_->setParameter("horz_align", "center");
	title_text_->setParameter("vert_align", "top");
	title_text_->setLeft(0);
	title_text_->setTop(200.0f / 768.0f);  // 200 pixels from top
	title_text_->setWidth(1.0f);
	title_text_->setHeight(0.13f);
	title_text_->setParameter("font_name", "BlueHighway");
	title_text_->setParameter("char_height", "0.08");  // 8% of screen height for large title
	title_text_->setParameter("alignment", "center");
	title_text_->setColour(Ogre::ColourValue(1.0f, 0.0f, 0.0f, 1.0f));
	
	// Create score text shadow
	score_shadow_ = overlay_manager_->createOverlayElement("TextArea", "GameOverScoreShadow");
	score_shadow_->setMetricsMode(Ogre::GMM_RELATIVE);
	score_shadow_->setParameter("horz_align", "center");
	score_shadow_->setParameter("vert_align", "top");
	score_shadow_->setLeft(2.0f / 1024.0f);
	score_shadow_->setTop(352.0f / 768.0f);
	score_shadow_->setWidth(0.8f);
	score_shadow_->setHeight(0.1f);
	score_shadow_->setParameter("font_name", "BlueHighway");
	score_shadow_->setParameter("char_height", "0.06");  // 6% of screen height
	score_shadow_->setParameter("alignment", "center");
	score_shadow_->setColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 0.8f));
	
	// Create score text
	score_text_ = overlay_manager_->createOverlayElement("TextArea", "GameOverScore");
	score_text_->setMetricsMode(Ogre::GMM_RELATIVE);
	score_text_->setParameter("horz_align", "center");
	score_text_->setParameter("vert_align", "top");
	score_text_->setLeft(0);
	score_text_->setTop(350.0f / 768.0f);
	score_text_->setWidth(0.8f);
	score_text_->setHeight(0.1f);
	score_text_->setParameter("font_name", "BlueHighway");
	score_text_->setParameter("char_height", "0.06");  // 6% of screen height
	score_text_->setParameter("alignment", "center");
	score_text_->setColour(Ogre::ColourValue(1.0f, 1.0f, 1.0f, 1.0f));
	
	// Create restart prompt shadow
	restart_shadow_ = overlay_manager_->createOverlayElement("TextArea", "GameOverRestartShadow");
	restart_shadow_->setMetricsMode(Ogre::GMM_RELATIVE);
	restart_shadow_->setParameter("horz_align", "center");
	restart_shadow_->setParameter("vert_align", "top");
	restart_shadow_->setLeft(2.0f / 1024.0f);
	restart_shadow_->setTop(502.0f / 768.0f);
	restart_shadow_->setWidth(0.8f);
	restart_shadow_->setHeight(0.08f);
	restart_shadow_->setParameter("font_name", "BlueHighway");
	restart_shadow_->setParameter("char_height", "0.04");  // 4% of screen height
	restart_shadow_->setParameter("alignment", "center");
	restart_shadow_->setColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 0.8f));
	restart_shadow_->setCaption("Press ENTER to Restart");
	
	// Create restart prompt
	restart_prompt_ = overlay_manager_->createOverlayElement("TextArea", "GameOverRestart");
	restart_prompt_->setMetricsMode(Ogre::GMM_RELATIVE);
	restart_prompt_->setParameter("horz_align", "center");
	restart_prompt_->setParameter("vert_align", "top");
	restart_prompt_->setLeft(0);
	restart_prompt_->setTop(500.0f / 768.0f);
	restart_prompt_->setWidth(0.8f);
	restart_prompt_->setHeight(0.08f);
	restart_prompt_->setParameter("font_name", "BlueHighway");
	restart_prompt_->setParameter("char_height", "0.04");  // 4% of screen height
	restart_prompt_->setParameter("alignment", "center");
	restart_prompt_->setColour(Ogre::ColourValue(0.8f, 0.8f, 0.8f, 1.0f));
	restart_prompt_->setCaption("Press ENTER to Restart");
	
	// Create load prompt shadow
	load_shadow_ = overlay_manager_->createOverlayElement("TextArea", "GameOverLoadShadow");
	load_shadow_->setMetricsMode(Ogre::GMM_RELATIVE);
	load_shadow_->setParameter("horz_align", "center");
	load_shadow_->setParameter("vert_align", "top");
	load_shadow_->setLeft(2.0f / 1024.0f);
	load_shadow_->setTop(542.0f / 768.0f);
	load_shadow_->setWidth(0.8f);
	load_shadow_->setHeight(0.08f);
	load_shadow_->setParameter("font_name", "BlueHighway");
	load_shadow_->setParameter("char_height", "0.04");
	load_shadow_->setParameter("alignment", "center");
	load_shadow_->setColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 0.8f));
	load_shadow_->setCaption("Press F8 to Load Save");
	
	// Create load prompt
	load_prompt_ = overlay_manager_->createOverlayElement("TextArea", "GameOverLoad");
	load_prompt_->setMetricsMode(Ogre::GMM_RELATIVE);
	load_prompt_->setParameter("horz_align", "center");
	load_prompt_->setParameter("vert_align", "top");
	load_prompt_->setLeft(0);
	load_prompt_->setTop(540.0f / 768.0f);
	load_prompt_->setWidth(0.8f);
	load_prompt_->setHeight(0.08f);
	load_prompt_->setParameter("font_name", "BlueHighway");
	load_prompt_->setParameter("char_height", "0.04");  // Match font native size
	load_prompt_->setParameter("alignment", "center");
	load_prompt_->setColour(Ogre::ColourValue(0.8f, 0.8f, 0.8f, 1.0f));
	load_prompt_->setCaption("Press F8 to Load Save");
	
	// Create exit prompt shadow
	exit_shadow_ = overlay_manager_->createOverlayElement("TextArea", "GameOverExitShadow");
	exit_shadow_->setMetricsMode(Ogre::GMM_RELATIVE);
	exit_shadow_->setParameter("horz_align", "center");
	exit_shadow_->setParameter("vert_align", "top");
	exit_shadow_->setLeft(2.0f / 1024.0f);
	exit_shadow_->setTop(582.0f / 768.0f);
	exit_shadow_->setWidth(0.8f);
	exit_shadow_->setHeight(0.08f);
	exit_shadow_->setParameter("font_name", "BlueHighway");
	exit_shadow_->setParameter("char_height", "0.04");
	exit_shadow_->setParameter("alignment", "center");
	exit_shadow_->setColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 0.8f));
	exit_shadow_->setCaption("Press ESC to Exit");
	
	// Create exit prompt
	exit_prompt_ = overlay_manager_->createOverlayElement("TextArea", "GameOverExit");
	exit_prompt_->setMetricsMode(Ogre::GMM_RELATIVE);
	exit_prompt_->setParameter("horz_align", "center");
	exit_prompt_->setParameter("vert_align", "top");
	exit_prompt_->setLeft(0);
	exit_prompt_->setTop(580.0f / 768.0f);
	exit_prompt_->setWidth(0.8f);
	exit_prompt_->setHeight(0.08f);
	exit_prompt_->setParameter("font_name", "BlueHighway");
	exit_prompt_->setParameter("char_height", "0.04");  // Match font native size
	exit_prompt_->setParameter("alignment", "center");
	exit_prompt_->setColour(Ogre::ColourValue(0.8f, 0.8f, 0.8f, 1.0f));
	exit_prompt_->setCaption("Press ESC to Exit");
	
	// Add shadows to shadow container
	shadow_container->addChild(title_shadow_);
	shadow_container->addChild(score_shadow_);
	shadow_container->addChild(restart_shadow_);
	shadow_container->addChild(load_shadow_);
	shadow_container->addChild(exit_shadow_);
	
	// Add text elements to text container
	text_container->addChild(title_text_);
	text_container->addChild(score_text_);
	text_container->addChild(restart_prompt_);
	text_container->addChild(load_prompt_);
	text_container->addChild(exit_prompt_);
	
	// Add containers to panel - shadow container first, then text container
	panel_->addChild(shadow_container);
	panel_->addChild(text_container);
	overlay_->add2D(panel_);
	
	// Initially hide the overlay
	overlay_->hide();
}

void GameOverScreen::show_death_screen(int score) {
	show_death_screen(score, false);  // Default: no save available
}

void GameOverScreen::show_death_screen(int score, bool has_save) {
	if (!overlay_) {
		create_overlay();
	}
	if (!overlay_) {
		return;
	}
	show_screen("GAME OVER", score, has_save);
	title_text_->setColour(Ogre::ColourValue(1.0f, 0.0f, 0.0f, 1.0f)); // Red
}

void GameOverScreen::show_victory_screen(int score) {
	show_victory_screen(score, false);  // Default: no save available
}

void GameOverScreen::show_victory_screen(int score, bool has_save) {
	if (!overlay_) {
		create_overlay();
	}
	if (!overlay_) {
		return;
	}
	show_screen("VICTORY!", score, has_save);
	title_text_->setColour(Ogre::ColourValue(0.0f, 1.0f, 0.0f, 1.0f)); // Green
}

void GameOverScreen::show_screen(const std::string& title, int score) {
	show_screen(title, score, false);  // Default: no save available
}

void GameOverScreen::show_screen(const std::string& title, int score, bool has_save) {
	title_text_->setCaption(title);
	title_shadow_->setCaption(title);  // Update shadow text too
	
	std::string score_caption = "Final Score: " + std::to_string(score);
	score_text_->setCaption(score_caption);
	score_shadow_->setCaption(score_caption);  // Update shadow text too
	
	// Show/hide load prompt based on whether a save exists
	if (load_prompt_) {
		if (has_save) {
			load_shadow_->show();
			load_prompt_->show();
		}
		else {
			load_shadow_->hide();
			load_prompt_->hide();
		}
	}
	
	visible_ = true;
	overlay_->show();
}

void GameOverScreen::hide() {
	if (!overlay_) {
		return;
	}
	visible_ = false;
	overlay_->hide();
}
