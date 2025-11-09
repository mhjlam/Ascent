#pragma once

#include <Ogre.h>
#include <OgreOverlay.h>
#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>
#include <OgreOverlayElement.h>
#include <string>

/**
 * Displays a game over screen with score and restart/exit prompts
 * Used for both player death and level completion
 */
class GameOverScreen {
public:
	static GameOverScreen& instance();
	
	// Delete copy and move operations
	GameOverScreen(const GameOverScreen&) = delete;
	GameOverScreen& operator=(const GameOverScreen&) = delete;
	GameOverScreen(GameOverScreen&&) = delete;
	GameOverScreen& operator=(GameOverScreen&&) = delete;

public:
	// Show game over screen for different scenarios
	void show_death_screen(int score);
	void show_victory_screen(int score);
	
	// Show game over screen with option to show/hide load prompt
	void show_death_screen(int score, bool has_save);
	void show_victory_screen(int score, bool has_save);
	
	// Hide the game over screen
	void hide();
	
	// Check if screen is currently visible
	bool is_visible() const { return visible_; }

private:
	GameOverScreen();
	~GameOverScreen();
	
	void create_overlay();
	void show_screen(const std::string& title, int score);
	void show_screen(const std::string& title, int score, bool has_save);

private:
	bool visible_;

	Ogre::Overlay* overlay_;
	Ogre::OverlayContainer* panel_;
	Ogre::OverlayManager* overlay_manager_;

	Ogre::OverlayElement* title_text_;
	Ogre::OverlayElement* score_text_;
	Ogre::OverlayElement* restart_prompt_;
	Ogre::OverlayElement* load_prompt_;
	Ogre::OverlayElement* exit_prompt_;

	Ogre::OverlayElement* title_shadow_;
	Ogre::OverlayElement* score_shadow_;
	Ogre::OverlayElement* restart_shadow_;
	Ogre::OverlayElement* load_shadow_;
	Ogre::OverlayElement* exit_shadow_;
};
