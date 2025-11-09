#pragma once

#include <Ogre.h>

#include "Constants.hpp"

/*
 *  GameEntity forms the base controller for every created object, with the exception of lights.
 *  It holds an Ogre::Entity and Ogre::SceneNode as well as its startPosition and facing direction.
 *  Main keypoints are the update and collide functions and the EntityType member.
 */

class GameEntity {    
public:
    GameEntity() = default;
    GameEntity(Ogre::Entity* const entity, Ogre::SceneNode* const node);
	virtual ~GameEntity();

public:
    virtual void update([[maybe_unused]] const Ogre::Real& elapsed) {}
    virtual void destroy();

    void translate(const Ogre::Vector3& translation);
    void move_to(const Ogre::Vector3& newLocation);

public: // getters
    virtual const Ogre::AxisAlignedBox get_bounding_box() { 
        return entity_ ? entity_->getBoundingBox() : bounding_box_;
    }

    bool is_alive() const { 
        return alive_;
    }
    bool is_entity_type(const EntityType& type) const { 
        return type_ == type;
    }
    const EntityType& get_type() const { 
        return type_;
    }
    Ogre::Entity* const get_entity() { 
        return entity_;
    }
    Ogre::SceneNode* const get_node() { 
        return main_node_;
    }
	const Ogre::Vector3& get_start_position() const { 
        return start_position_;
    }
    const Ogre::Vector3 get_position() const { 
        return main_node_->getPosition();
    }
    const Ogre::Vector3& get_direction() const { 
        return direction_;
    }

public: // setters
    void set_node(Ogre::SceneNode* const newNode) { 
        main_node_ = newNode;
    }
    void set_entity(Ogre::Entity* const newEntity) { 
        entity_ = newEntity;
    }
    void set_start_position(const Ogre::Vector3& position) { 
        start_position_ = position;
    }
    void set_position(const Ogre::Vector3& position) { 
        main_node_->setPosition(position);
    }
    void set_direction(const Ogre::Vector3& direction) { 
        main_node_->setDirection(direction);
        direction_ = direction;
    }
    void set_type(const EntityType& type) { 
        type_ = type;
    }

protected:
    bool alive_;
    EntityType type_;

    Ogre::Vector3 direction_; 
    Ogre::Vector3 start_position_;

    Ogre::Entity* entity_;
    Ogre::SceneNode* main_node_;
    Ogre::AxisAlignedBox bounding_box_;
};
