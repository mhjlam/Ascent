#include "stdafx.h"
#include "GameEntity.h"

#include "../Common.h"
#include "../map/GameWorld.h"

GameEntity::GameEntity(Ogre::Entity* const entity, Ogre::SceneNode* const node) : mEntity(entity), mMainNode(node), mIsAlive(true)
{
    // create a default bounding box for GameEntities with no Ogre::Entity (such as Player)
    mBoundingBox.setMinimum(mMainNode->getPosition() - Ogre::Vector3(100.0f, 100.0f, 100.0f));
    mBoundingBox.setMaximum(mMainNode->getPosition() + Ogre::Vector3(100.0f, 100.0f, 100.0f));
}

GameEntity::~GameEntity()
{  
    // destroying this GameEntity also means destroying its associated Ogre::Entity and Ogre::SceneNode
    
    if (mEntity)
        Common::mSceneManager->destroyEntity(mEntity);

    if (mMainNode)
    {
        mMainNode->removeAndDestroyAllChildren();
        Common::mSceneManager->destroySceneNode(mMainNode);
    }
}

void GameEntity::translate(const Ogre::Vector3& translation)
{
    getNode()->translate(translation, Ogre::Node::TS_PARENT);
}

void GameEntity::moveTo(const Ogre::Vector3& newLocation)
{
    setPosition(newLocation);
}

void GameEntity::destroyMe()
{
    mIsAlive = false; // setting this to false will cause GameWorld to destroy this entity next time it updates

    // removes the overview equivalent of this entity
    if (Common::mSceneManagerOverview->hasSceneNode(getNode()->getName()))
        Common::mSceneManagerOverview->destroySceneNode(getNode()->getName());
}