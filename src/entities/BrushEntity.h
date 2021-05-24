#ifndef _BRUSH_ENTITY_H_
#define _BRUSH_ENTITY_H_

#include "../stdafx.h"
#include "../Constants.h"
#include "GameEntity.h"

/*
 *  BrushEntity was originally planned to allow for easy collisions and to allow GameEntities to interact, but now mainly serves
 *  for determining which instances are to be copied to the overview (BRUSH_WALLs) and which brush determines to end the map (BRUSH_EXIT)
 */

class BrushEntity : public GameEntity
{
private:
    BrushType mBrushType;

public:
    BrushEntity(Ogre::Entity* const entity, Ogre::SceneNode* const node, BrushType type);

    const BrushType& getBrushType() const           { return mBrushType;         }
    bool isType(const BrushType& type) const        { return mBrushType == type; }
};

#endif