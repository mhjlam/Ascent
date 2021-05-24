#include "stdafx.h"
#include "Game.h"

#include "../Common.h"
#include "LoadingBar.h"
#include "TextRenderer.h"

#include "../map/GameWorld.h"
#include "../map/LevelOne.h"
#include "../map/LevelTwo.h"

#define CAMERA_NAME "camera"

Game* Game::mInstance = NULL;

Game* Game::getInstance() 
{
	if (mInstance == NULL)
		mInstance = new Game();

	return mInstance;
}

Game::Game() :
mInputManager(NULL), mKeyboard(NULL), mMouse(NULL), // OIS
mPluginsConfig(Ogre::StringUtil::BLANK), mResourcesConfig(Ogre::StringUtil::BLANK), // config files
mMapMode(false), mShutdown(false), // switches
mTimeRemaining(180.0f), mCurrentMapi(0)  // specifics
{
	
}

Game::~Game()
{
	Common::mSceneManager->destroyQuery(Common::mRaySceneQuery);

	Ogre::WindowEventUtilities::removeWindowEventListener(Common::mWindow, this);
	windowClosed(Common::mWindow);

    // delete maps
	for (Ogre::uint i = 0; i < mMapsToLoad.size(); ++i)
        delete mMapsToLoad.at(i);

    // unload and destroy world
    GameWorld::destroySingleton();

    // perform cleanup
    Common::cleanup();

	if (mInstance)
	{
		delete mInstance;
		mInstance = NULL;
	}
}

bool Game::setup()
{
	if (!configure())
		return false; // configure failed

	setupResources();

	LoadingBar* lb = new LoadingBar();
	lb->start(Common::mWindow, 1, 1, 0.5f); // these magic numbers make sure the loading bar has just the right size for 1024x768

	setupSceneManagers();
	setupCameras();
	
	if (!setupViewport(Common::mSceneManager))
		return false;

	loadResources();
	createFrameListener();

	createOverlay();
    createWorld();
	
	lb->finish();
	delete lb;

    return true;
}

// initialize root
bool Game::configure()
{
	Common::mRoot = new Ogre::Root(mPluginsConfig);
	
	// restore previous config or else show dialog (which returns false if user exists window)
	if (!(Common::mRoot->restoreConfig() || Common::mRoot->showConfigDialog()))
		return false;

	Common::mWindow = Common::mRoot->initialise(true, "Ascent");

	return true;
}

// initialize scene managers for both 'game modes' (default/overview)
void Game::setupSceneManagers()
{
	Common::mSceneManager = Common::mRoot->createSceneManager(Ogre::ST_GENERIC);
	Common::mSceneManagerOverview = Common::mRoot->createSceneManager(Ogre::ST_GENERIC);
}

// initialize cameras for both scene managers
void Game::setupCameras()
{
	// setup playercam
    Common::mCamera = Common::mSceneManager->createCamera(CAMERA_NAME);
	Common::mCamera->setNearClipDistance(5);

    // setup overview cam
	Common::mCameraOverview = Common::mSceneManagerOverview->createCamera(CAMERA_NAME);
    Common::mCameraOverview->pitch(Ogre::Degree(-90.0f)); // face straight down
}

// setup a viewport for the specified scene manager
bool Game::setupViewport(Ogre::SceneManager* smgr)
{
	if (!smgr)
		return false;

	Common::mWindow->removeAllViewports();

	Ogre::Camera* cam = smgr->getCamera(CAMERA_NAME);
	Ogre::Viewport* viewport = Common::mWindow->addViewport(cam);
    viewport->setBackgroundColour(Ogre::ColourValue::Black);

	// set aspect ration to match viewport
	cam->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight()));

	return true;
}

// setup all the resources referenced to in the resources file so they can be used
void Game::setupResources()
{
	// Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load(mResourcesConfig);

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator sit = cf.getSectionIterator();

    Ogre::String secName, typeName, archName;
    while (sit.hasMoreElements())
    {
        secName = sit.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = sit.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator it;
        
		for (it = settings->begin(); it != settings->end(); ++it)
        {
            typeName = it->first;
            archName = it->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
        }
    }
}


void Game::loadResources()
{
	// default mipmap level
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

	// make resources ready for use
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

// setup and enable input listeners
void Game::createFrameListener()
{
	Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");

    OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;

    Common::mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

    mInputManager = OIS::InputManager::createInputSystem(pl);

    mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));
    mMouse	  = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));

    mKeyboard->setEventCallback(this);
	mMouse->setEventCallback(this);

    //Set initial mouse clipping size
    windowResized(Common::mWindow);

    //Register as a Window listener
    Ogre::WindowEventUtilities::addWindowEventListener(Common::mWindow, this);
    Common::mRoot->addFrameListener(this);

	// set up Ray Query
	Common::mRaySceneQuery = Common::mSceneManager->createRayQuery(Ogre::Ray());
}

// initialize hud overlay
void Game::createOverlay()
{
	TextRenderer::getInstance()->addCrosshair();
	TextRenderer::getInstance()->addTextBox("hud_health", "", 10, 10, 100, 20);
	TextRenderer::getInstance()->addTextBox("hud_timer", "", 10, 30, 100, 20);
	TextRenderer::getInstance()->addTextBox("hud_score", "", 10, 50, 100, 20);
}

// initialize maps
void Game::createWorld()
{
    mMapsToLoad.push_back(new LevelOne());
    mMapsToLoad.push_back(new LevelTwo());

    triggerMapEnd(); // will load first map
}

// restart current map
void Game::triggerMapRestart()
{
    mTimeRemaining = 180.0f;
    
    if (mMapMode)
        swapView();
    
	GameWorld::getInstance()->loadMap(mMapsToLoad.at(mCurrentMapi-1));
}

// end the current map
void Game::triggerMapEnd()
{
	Common::mState.clearDeadEnemies();
	
    mTimeRemaining = 180.0f;
    
    if (mMapMode)
        swapView();
    
    if (mCurrentMapi < mMapsToLoad.size())
        GameWorld::getInstance()->loadMap(mMapsToLoad.at(mCurrentMapi));
    else
        mShutdown = true;

    ++mCurrentMapi;
}

// updated per frame
bool Game::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    if (Common::mWindow)
	    if (Common::mWindow->isClosed())
		    return false;

	if (mShutdown)
		return false;

	// end-game conditions
	if (mTimeRemaining < -2.0f)
		mShutdown = true;
    if (Common::mPlayer)
	    if (Common::mPlayer->getHealth() <= 0)
        {
            triggerMapRestart();
            return true;
        }

	// capture input devices
	mKeyboard->capture();
	mMouse->capture();

    // update game world and timer (only when not in mapmode)
    if (!mMapMode)
    {
        GameWorld::getInstance()->update(evt.timeSinceLastFrame);
        mTimeRemaining -= evt.timeSinceLastFrame;
    }

    updateOverlay();

    return true;
}

// swap 'game mode' between default/overview
void Game::swapView()
{
	if (mMapMode)
	{
	    Common::mWindow->getViewport(0)->setCamera(Common::mCameraOverview);
		TextRenderer::getInstance()->hideCrosshair();
	}
    else
	{
	    Common::mWindow->getViewport(0)->setCamera(Common::mCamera); // show default SceneManager
		TextRenderer::getInstance()->showCrosshair();
	}
}

// update overlay
void Game::updateOverlay()
{
	std::stringstream health, timeleft, credits;
	timeleft.precision(3);
	health   << "HEALTH:  " << (Common::mPlayer ? Common::mPlayer->getHealth() : '\0');
	timeleft << "TIME:    " << mTimeRemaining;
    credits  << "SCORE:   " << (Common::mPlayer ? Common::mPlayer->getCredits() : '\0');

	TextRenderer::getInstance()->setText("hud_health", health.str());
	TextRenderer::getInstance()->setText("hud_timer", timeleft.str());
	TextRenderer::getInstance()->setText("hud_score", credits.str());

	if (mTimeRemaining <= 0.0f)
		TextRenderer::getInstance()->setText("hud_timer",  "TIME:    0", Ogre::ColourValue(1.0f, 0.0f, 0.0f)); // set color to red

    if (Common::mPlayer)
	{
		Ogre::uint playerHealth = Common::mPlayer->getHealth();
		Ogre::ColourValue indicatorColor = Ogre::ColourValue(1.0f, 1.0f, 1.0f);

		std::stringstream lifebar;
		lifebar << "HEALTH:  " << playerHealth;

		if (playerHealth <= 30) // critical
			indicatorColor = Ogre::ColourValue(1.0f, 0.0f, 0.0f);
		else if (playerHealth <= 60) // damaged
			indicatorColor = Ogre::ColourValue(1.0f, 1.0f, 0.0f);
		else //(playerHealth > 50)
			indicatorColor = Ogre::ColourValue(0.1f, 1.0f, 0.1f);

		TextRenderer::getInstance()->setText("hud_health", lifebar.str(), indicatorColor);
	}
}

// adjust mouse clipping area
void Game::windowResized(Ogre::RenderWindow* rw)
{
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState &ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

// detach OIS before window shutdown (especially important under Linux)
void Game::windowClosed(Ogre::RenderWindow* rw)
{
	if (rw == Common::mWindow) // only close for window that created OIS (here: main window)
	{
		if (mInputManager)
		{
			mInputManager->destroyInputObject(mMouse);
			mInputManager->destroyInputObject(mKeyboard);

			OIS::InputManager::destroyInputSystem(mInputManager);
			mInputManager = NULL;
		}
	}
}

void Game::loadMap(int mapId) 
{
	mCurrentMapi = mapId;
	GameWorld::getInstance()->loadMap(mMapsToLoad.at(mCurrentMapi - 1));
}

void Game::setTimeLeft(float timeLeft) 
{
	mTimeRemaining = timeLeft;
}

void Game::save() {
	std::ofstream ofs("save");
	
	boost::archive::text_oarchive archive(ofs);
	
	Common::mState.setCurrentLevel(mCurrentMapi);
	Common::mState.setPlayerHealth(Common::mPlayer->getHealth());
	Common::mState.setPlayerPosition(Common::mPlayer->getPosition());
	Common::mState.setTimeLeft(mTimeRemaining);
	Common::mState.setScore(Common::mPlayer->getCredits());
	Common::mState.setPlayerOrientation(Common::mPlayer->getNode()->getOrientation());

	archive << Common::mState;
}

void Game::load() 
{
	std::ifstream ifs("save");
	
	boost::archive::text_iarchive archive(ifs);
	archive >> Common::mState;
	// restore previously saved state
	Common::mState.load();
}

bool Game::keyPressed(const OIS::KeyEvent &evt)
{
	if (evt.key == OIS::KC_ESCAPE)
		mShutdown = true;
	else if (evt.key == OIS::KC_F1)
		save();
	else if (evt.key == OIS::KC_F2)
		load();
	else if (evt.key == OIS::KC_TAB)
	{
		mMapMode = !mMapMode;

        if (Common::mPlayer)
			Common::mPlayer->stopMovement(); // prevent player from moving while map mode is active
        
        // update cam location
        Common::mCameraOverview->setPosition(Common::mPlayer->getPosition() + (Ogre::Vector3::UNIT_Y * 10000.0f));

		swapView();

    } 
	else if (evt.key == OIS::KC_RETURN)
    {
        triggerMapEnd(); // manual trigger
        return true;
    }
	else if (evt.key == OIS::KC_BACK)
	{
		triggerMapRestart();
		return true;
	}

	if (!mMapMode)
		if (Common::mPlayer)
			Common::mPlayer->injectKeyDown(evt);

	return true;
}

bool Game::keyReleased(const OIS::KeyEvent &evt)
{
	if (!mMapMode)
		if (Common::mPlayer)
			Common::mPlayer->injectKeyUp(evt);
	
    return true;
}

bool Game::mouseMoved(const OIS::MouseEvent &evt)
{
	if (!mMapMode)
		if (Common::mPlayer)
			Common::mPlayer->injectMouseMove(evt);
	
    return true;
}

bool Game::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	if (!mMapMode)
		if (Common::mPlayer)
			Common::mPlayer->injectMouseDown(evt, id);
	
    return true;
}

bool Game::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	if (!mMapMode)
		if (Common::mPlayer)
			Common::mPlayer->injectMouseUp(evt, id);
	
    return true;
}

// main start of this class
void Game::run()
{
#ifdef _DEBUG
	mPluginsConfig = "plugins_d.cfg";
	mResourcesConfig = "resources_d.cfg";
#else
	mPluginsConfig = "plugins.cfg";
	mResourcesConfig = "resources.cfg";
#endif
	
	if (!setup())
		return;

	Common::mRoot->startRendering();
}
