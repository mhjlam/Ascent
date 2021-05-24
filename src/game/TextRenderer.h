#ifndef _TEXTRENDERER_H_
#define _TEXTRENDERER_H_

#include "../stdafx.h"

/*
 *  Manages the overlay text and crosshair
 */

class TextRenderer
{
private:
	static TextRenderer* mInstance;
	TextRenderer();
	~TextRenderer();

private:
	Ogre::OverlayManager*	mOverlayManager;
	Ogre::Overlay*			mOverlay;
	Ogre::Overlay*			mCrosshairOverlay;
	Ogre::OverlayContainer*	mPanel;

public:
	static TextRenderer* getInstance();

	void addCrosshair();
	void hideCrosshair();
	void showCrosshair();

	void addTextBox(const std::string& id, const std::string& text, Ogre::Real x, Ogre::Real y, Ogre::Real width, Ogre::Real height, const Ogre::ColourValue& color = Ogre::ColourValue(1.0f, 1.0f, 1.0f));
	void removeTextBox(const std::string& id);
	
	void setText(const std::string& id, const std::string& text, const Ogre::ColourValue& color = Ogre::ColourValue(1.0f, 1.0f, 1.0f));
	const std::string getText(const std::string& id);
};

#endif
