#pragma once

#include "Constants.hpp"
#include "Entities/GameEntity.hpp"


class BrushEntity : public GameEntity {
public:
    BrushEntity(Ogre::Entity* const entity, Ogre::SceneNode* const node, BrushType type);

public:
    bool is_brush_type(const BrushType& type) const { 
        return brush_type_ == type;
    }
    const BrushType& get_brush_type() const { 
        return brush_type_;
    }

private:
    BrushType brush_type_;
};
