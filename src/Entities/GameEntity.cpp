#include "GameEntity.hpp"

#include "Common.hpp"
#include "Map/GameWorld.hpp"

GameEntity::GameEntity(Ogre::Entity* const entity, Ogre::SceneNode* const node) 
: entity_(entity)
, main_node_(node)
, alive_(true) {
    // create a default bounding box for GameEntities with no Ogre::Entity (such as Player)
    bounding_box_.setMinimum(main_node_->getPosition() - Ogre::Vector3(100.0f, 100.0f, 100.0f));
    bounding_box_.setMaximum(main_node_->getPosition() + Ogre::Vector3(100.0f, 100.0f, 100.0f));
}

GameEntity::~GameEntity() {  
    if (entity_) {
        Common::scene_manager->destroyEntity(entity_);
    }

    if (main_node_) {
        main_node_->removeAndDestroyAllChildren();
        Common::scene_manager->destroySceneNode(main_node_);
    }
}

void GameEntity::translate(const Ogre::Vector3& translation) {
    get_node()->translate(translation, Ogre::Node::TS_PARENT);
}

void GameEntity::move_to(const Ogre::Vector3& newLocation) {
    set_position(newLocation);
}

void GameEntity::destroy() {
    alive_ = false; // setting this to false will cause GameWorld to destroy this entity next time it updates

    // removes the overview equivalent of this entity
    if (Common::overview_scene_manager->hasSceneNode(get_node()->getName())) {
        Common::overview_scene_manager->destroySceneNode(get_node()->getName());
    }
}
