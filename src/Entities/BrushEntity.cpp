#include "BrushEntity.hpp"

BrushEntity::BrushEntity(Ogre::Entity* const entity, 
                         Ogre::SceneNode* const node, 
                         BrushType type) 
: GameEntity(entity, node)
, brush_type_(type) {}
