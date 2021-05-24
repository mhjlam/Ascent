#ifndef _GAME_ENTITY_H_
#define _GAME_ENTITY_H_

#include "../stdafx.h"
#include "../Constants.h"

/*
 *  GameEntity forms the base controller for every created object, with the exception of lights.
 *  It holds an Ogre::Entity and Ogre::SceneNode as well as its startPosition and facing direction.
 *  Main keypoints are the update and collide functions and the EntityType member.
 */

class GameEntity
{
protected:
	// the start position allows us to reset this entity to its original position
    Ogre::Vector3 mStartPosition;
    
    /*  
     *  Ogre::Nodes don't have a getDirection(), so we use this one for that purpose
     *  however, it is not updated if the mainNode is rotated or otherwise modified.
     *  This means its use is limited and is only used by projectiles
     */ 
    Ogre::Vector3 mDirection; 

    // the mesh or any other shape
    Ogre::Entity* mEntity;  

    // the main scenenode this entity is attached to
    Ogre::SceneNode* mMainNode; 

    // a custom bounding box, for when mEntity is NULL (currently unused)
    Ogre::AxisAlignedBox mBoundingBox;  

    // what type of entity this is
    EntityType mType;

    // self-explanitory
    bool mIsAlive;
    
public:
    GameEntity() {};
    GameEntity(Ogre::Entity* const entity, Ogre::SceneNode* const node);
	virtual ~GameEntity();

public:
    virtual void update(const Ogre::Real& elapsed)		{ /* override in subclass */ }
    virtual void destroyMe();

    // convenience functions
    void translate(const Ogre::Vector3& translation);
    void moveTo(const Ogre::Vector3& newLocation);

    // setters
    virtual const Ogre::AxisAlignedBox getBoundingBox()     { return (mEntity? mEntity->getBoundingBox() : mBoundingBox); }
	
    void setNode(Ogre::SceneNode* const newNode)        { mMainNode = newNode;  }
    void setEntity(Ogre::Entity* const newEntity)       { mEntity = newEntity;  }

    void setStartPosition(const Ogre::Vector3& position){ mStartPosition = position;		    }
    void setPosition(const Ogre::Vector3& position)     { mMainNode->setPosition(position);     }
    void setDirection(const Ogre::Vector3& direction)   { mMainNode->setDirection(direction);   
                                                          mDirection = direction;               }
    void setType(const EntityType& type)                { mType = type;                         } 

    // getters
    Ogre::SceneNode* const getNode()					{ return mMainNode; }
    Ogre::Entity* const getEntity()						{ return mEntity;   }

	const Ogre::Vector3& getStartPosition() const		{ return mStartPosition;		   }
    const Ogre::Vector3& getPosition() const		    { return mMainNode->getPosition(); }
    const Ogre::Vector3& getDirection() const           { return mDirection;               }

    const EntityType& getType() const                   { return mType;         }
    bool isType(const EntityType& type) const           { return mType == type; }

    bool isAlive() const								{ return mIsAlive; }
};

#endif
