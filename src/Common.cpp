#include "stdafx.h"
#include "Common.h"

Ogre::Root*					Common::mRoot = NULL;
Ogre::RenderWindow*			Common::mWindow = NULL;
Ogre::SceneManager*			Common::mSceneManager = NULL;
Ogre::SceneManager*			Common::mSceneManagerOverview = NULL;
Ogre::Camera*				Common::mCamera = NULL;
Ogre::Camera*				Common::mCameraOverview = NULL;
Ogre::RaySceneQuery*		Common::mRaySceneQuery = NULL;

Player* Common::mPlayer = NULL;
GameState Common::mState;

void Common::cleanup()
{
	if (mPlayer)
	{
		delete mPlayer;
		mPlayer = NULL;
	}
	if (mRaySceneQuery)
	{
		delete mRaySceneQuery;
		mRaySceneQuery = NULL;
	}
	if (mSceneManager)
	{
		delete mSceneManager;
		mSceneManager = NULL;
	}
	if (mSceneManagerOverview)
	{
		delete mSceneManagerOverview;
		mSceneManagerOverview = NULL;
	}
	if (mCamera)
	{
		delete mCamera;
		mCamera = NULL;
	}
	if (mCameraOverview)
	{
		delete mCameraOverview;
		mCameraOverview = NULL;
	}
	if (mWindow)
	{
		delete mWindow;
		mWindow = NULL;
	}
	if (mRoot)
	{
		delete mRoot;
		mRoot = NULL;
	}
}
