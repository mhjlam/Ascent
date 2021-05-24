#ifndef _COMMON_H_
#define _COMMON_H_

#include "stdafx.h"
#include "entities/player/Player.h"

#include "GameState.h"

/*
 *  Common functions as a hub for all classes who want to get access to certain Ogre or other popular objects, such as player.
 *  It also provides a final cleanup, minimizing any memory leaks.
 */

class Common
{
public:
	static Ogre::Root*			mRoot;
	static Ogre::RenderWindow*	mWindow;
	static Ogre::SceneManager*	mSceneManager;
	static Ogre::SceneManager*	mSceneManagerOverview;
	static Ogre::Camera*		mCamera;
	static Ogre::Camera*		mCameraOverview;
	static Ogre::RaySceneQuery*	mRaySceneQuery;
	
	static Player* mPlayer;
	static GameState mState;

	static void cleanup();
};

#endif