#include "Projectile.hpp"

#include "Common.hpp"
#include "Enemies/Enemy.hpp"


Projectile::Projectile(Ogre::Entity* const entity, 
                       Ogre::SceneNode* const node, 
                       const Ogre::Vector3& direction, 
                       const Ogre::Quaternion& orientation)
: GameEntity(entity, node)
, orientation_(orientation)
, damage_(0)
, speed_(0)
, range_(0) {
    set_direction(direction);
}

void Projectile::update(const Ogre::Real& elapsed) {
    // check for collision
    Ogre::Ray front_ray(get_position(), get_direction());

    Common::ray_scene_query->setRay(front_ray);
    Common::ray_scene_query->setSortByDistance(true, 5);
    Common::ray_scene_query->setQueryMask(to_ogre_flags(QueryFlags::Wall | QueryFlags::Enemy));
    
    const auto& result = Common::ray_scene_query->execute();

    for (const auto& query_result : result) {
        const auto flags = query_result.movable->getQueryFlags();
        
        if (flags == to_ogre_flags(QueryFlags::Wall) && query_result.distance <= 100.0f) {
            destroy();
            return;
        }
        
        if (flags == to_ogre_flags(QueryFlags::Enemy) && 
            query_result.distance <= 300.0f && 
            is_entity_type(EntityType::PlayerProjectileType)) {
            
            auto* parent_node = query_result.movable->getParentNode();
            if (parent_node && !parent_node->getUserAny().isEmpty()) {
                if (auto* enemy = Ogre::any_cast<Enemy*>(parent_node->getUserAny())) {
                    enemy->hit(damage_);
                }
            }
            
            destroy();
            return;
        }
    }

    // move in the specified direction
    main_node_->translate(get_direction() * (speed_ * elapsed), Ogre::Node::TS_WORLD);

    // check if max range reached
    if (main_node_->getPosition().squaredDistance(start_position_) > range_ * range_) {
        destroy();
    }
}
