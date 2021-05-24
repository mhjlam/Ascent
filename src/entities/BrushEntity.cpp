#include "stdafx.h"
#include "BrushEntity.h"

BrushEntity::BrushEntity(Ogre::Entity* const entity, Ogre::SceneNode* const node, BrushType type) : GameEntity(entity, node), mBrushType(type)
{

}