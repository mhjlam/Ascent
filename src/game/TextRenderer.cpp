#include "stdafx.h"
#include "TextRenderer.h"

TextRenderer* TextRenderer::mInstance = NULL;

TextRenderer* TextRenderer::getInstance() 
{
	if (mInstance == NULL)
		mInstance = new TextRenderer();

	return mInstance;
}

TextRenderer::TextRenderer()
{
	mOverlayManager = Ogre::OverlayManager::getSingletonPtr();

	mOverlay = mOverlayManager->create("HUD/Text");
	mPanel = static_cast<Ogre::OverlayContainer*>(mOverlayManager->createOverlayElement("Panel", "container"));
	mPanel->setDimensions(1, 1);
	mPanel->setPosition(0, 0);

	mOverlay->add2D(mPanel);
	mOverlay->show();
}

TextRenderer::~TextRenderer()
{
	if (mInstance)
	{
		delete mInstance;
		mInstance = NULL;
	}
}

void TextRenderer::addCrosshair()
{
	mCrosshairOverlay = Ogre::OverlayManager::getSingleton().getByName("HUD/CrossHair");
	showCrosshair();
}

void TextRenderer::showCrosshair()
{
	if (mCrosshairOverlay)
		mCrosshairOverlay->show();
}

void TextRenderer::hideCrosshair()
{
	if (mCrosshairOverlay)
		mCrosshairOverlay->hide();
}

void TextRenderer::addTextBox(const std::string& id, const std::string& text, Ogre::Real x, Ogre::Real y, Ogre::Real width, Ogre::Real height, const Ogre::ColourValue& color)
{
	Ogre::OverlayElement* textBox = mOverlayManager->createOverlayElement("TextArea", id);
    textBox->setDimensions(width, height);
    textBox->setMetricsMode(Ogre::GMM_PIXELS);
    textBox->setPosition(x, y);
    textBox->setWidth(width);
    textBox->setHeight(height);
    textBox->setParameter("font_name", "BlueHighway");
    textBox->setParameter("char_height", "32");
    textBox->setColour(color);

    textBox->setCaption(text);

	mPanel->addChild(textBox);
}

void TextRenderer::removeTextBox(const std::string& id)
{
	mPanel->removeChild(id);
	mOverlayManager->destroyOverlayElement(id);
}
	
void TextRenderer::setText(const std::string& id, const std::string& text, const Ogre::ColourValue& color)
{
	Ogre::OverlayElement* textBox = mOverlayManager->getOverlayElement(id);
	textBox->setColour(color);
	textBox->setCaption(text);
}

const std::string TextRenderer::getText(const std::string& id)
{
	Ogre::OverlayElement* textBox = mOverlayManager->getOverlayElement(id);
	return textBox->getCaption();
}
