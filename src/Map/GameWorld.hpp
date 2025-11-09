#pragma once

#include <memory>
#include <list>
#include <vector>

#include "Map/Map.hpp"
#include "Entities/GameEntity.hpp"


class GameWorld {
public:
    static GameWorld& instance();
    
    // Delete copy and move operations
    GameWorld(const GameWorld&) = delete;
    GameWorld& operator=(const GameWorld&) = delete;
    GameWorld(GameWorld&&) = delete;
    GameWorld& operator=(GameWorld&&) = delete;

public:
    void register_entity(std::unique_ptr<GameEntity> entity);
    void destroy_entity(GameEntity* const entity);

    void register_map(std::unique_ptr<Map> map);
    void load_map_by_index(size_t index);
    void load_next_map();
    void reload_current_map();
    void clear_map();
    void update(const Ogre::Real& elapsed);
    void abort_update();
    
    [[nodiscard]] size_t get_current_map_index() const { 
        // current_map_index_ is incremented after loading, so it points to the next map
        // Subtract 1 to get the actual current map index
        return current_map_index_ > 0 ? current_map_index_ - 1 : 0; 
    }
    [[nodiscard]] bool has_more_maps() const { return current_map_index_ < maps_.size(); }
    
private:
    // creates the scene by parsing the map
    void create_scene();
    void create_brush(const BrushInfo* const brush);
    void create_light(const LightInfo* const light);
    void create_enemy(const EntityInfo* const enemy);
    void create_player(const EntityInfo* const player);

    // destroys all GameEntities and clears the scene
    void destroy_scene();
    
public:
    void update_overview_visibility(float playerY);

private:
    GameWorld();
    ~GameWorld() = default;
    
private:
    bool abort_updates_; // stop the update functions

    std::vector<std::unique_ptr<Map>> maps_;  // All maps owned by GameWorld
    size_t current_map_index_;                // Index of currently loaded map
    std::list<std::unique_ptr<GameEntity>> entities_;   // entities that require updating
    std::list<std::unique_ptr<GameEntity>> brushes_;    // brushes don't require updating
    
    // Track overview nodes with their Y positions for vertical filtering
    struct OverviewBrush {
        Ogre::SceneNode* node;
        float minY;
        float maxY;
        bool isMarker; // True for Start/Exit areas which should always be visible
        bool isPillar; // True for pillar/column parts which should all be shown together
    };
    std::vector<OverviewBrush> overview_brushes_;
    
    // TEST: Track test spotlight for updating orientation
    Ogre::SceneNode* test_spotlight_node_;
};
