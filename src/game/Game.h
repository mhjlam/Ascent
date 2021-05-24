#ifndef _GAME_H_
#define _GAME_H_

#include "../stdafx.h"
#include "../map/Map.h"
#include "../GameState.h"

/*
 *  Game forms the base for Ogre to talk to. Contains frameRenderingQueued and most of the things from BaseApplication
 */

class Game : public Ogre::FrameListener, public Ogre::WindowEventListener, public OIS::KeyListener, public OIS::MouseListener
{
private: // singleton
	static Game* mInstance;
	Game();
	~Game();

private:
	// OIS input devices
	OIS::InputManager*	mInputManager;
	OIS::Keyboard*		mKeyboard;
	OIS::Mouse*			mMouse;
	
	// config files
	Ogre::String		mPluginsConfig;
	Ogre::String		mResourcesConfig;

	// GAME SPECIFIC
	bool				mMapMode;
	bool				mShutdown;
	Ogre::Real			mTimeRemaining;

	std::vector<Map*>	mMapsToLoad;
    Ogre::uint			mCurrentMapi;

private: // procedures
	void updateOverlay();
	void swapView();
	void save();
	void load();

private: // functions
	bool setup();
    bool configure();
    void setupSceneManagers();
    void setupCameras();
	bool setupViewport(Ogre::SceneManager* smgr);
	void setupResources();
	void loadResources();
    void createFrameListener();
	void createOverlay();
    void createWorld();

    // derived from Ogre::FrameListener
	bool frameRenderingQueued(const Ogre::FrameEvent& evt);

	// derived from Ogre::WindowEventListener
	void windowResized(Ogre::RenderWindow* rw);
	void windowClosed(Ogre::RenderWindow* rw);

	// derived from OIS::KeyListener
    bool keyPressed(const OIS::KeyEvent &evt);
    bool keyReleased(const OIS::KeyEvent &evt);

	// derived from OIS::MouseListener
    bool mouseMoved(const OIS::MouseEvent &evt);
    bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

public:
    static Game* getInstance();
	
	void loadMap(int mapId);
	void setTimeLeft(float timeLeft);
	void triggerMapEnd();
	void triggerMapRestart();

    void run();
};

#endif
