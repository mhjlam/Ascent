#include "GameWorld.hpp"

#include <iostream>
#include <set>
#include "Common.hpp"

#include "Entities/BrushEntity.hpp"
#include "Entities/Player/Player.hpp"
#include "Entities/Enemies/EnemyFactory.hpp"

/*
 *  Helper function to create primitive meshes programmatically.
 *  This replaces the deprecated PT_CUBE and PT_SPHERE prefabs.
 */
namespace {
    void create_primitive_meshes(Ogre::SceneManager* scene_mgr) {
        // FORCE remove old meshes to ensure we get clean ones
        if (Ogre::MeshManager::getSingleton().resourceExists("ProgrammaticCube")) {
            Ogre::MeshManager::getSingleton().remove("ProgrammaticCube");
        }
        if (Ogre::MeshManager::getSingleton().resourceExists("ProgrammaticSphere")) {
            Ogre::MeshManager::getSingleton().remove("ProgrammaticSphere");
        }
        
        // Create unit cube mesh
        {
            Ogre::ManualObject* manual = scene_mgr->createManualObject();
            // Use BaseWhite material which can be overridden
            manual->begin("BaseWhite", Ogre::RenderOperation::OT_TRIANGLE_LIST);
            
            // Define vertices for a unit cube (1x1x1 centered at origin)
            const float s = 0.5f; // half size
            
            // Front face (z = s)
            manual->position(-s, -s, s); manual->normal(0, 0, 1); manual->textureCoord(0, 1);
            manual->position(s, -s, s); manual->normal(0, 0, 1); manual->textureCoord(1, 1);
            manual->position(s, s, s); manual->normal(0, 0, 1); manual->textureCoord(1, 0);
            manual->position(-s, s, s); manual->normal(0, 0, 1); manual->textureCoord(0, 0);
            
            // Back face (z = -s)
            manual->position(s, -s, -s); manual->normal(0, 0, -1); manual->textureCoord(0, 1);
            manual->position(-s, -s, -s); manual->normal(0, 0, -1); manual->textureCoord(1, 1);
            manual->position(-s, s, -s); manual->normal(0, 0, -1); manual->textureCoord(1, 0);
            manual->position(s, s, -s); manual->normal(0, 0, -1); manual->textureCoord(0, 0);
            
            // Left face (x = -s)
            manual->position(-s, -s, -s); manual->normal(-1, 0, 0); manual->textureCoord(0, 1);
            manual->position(-s, -s, s); manual->normal(-1, 0, 0); manual->textureCoord(1, 1);
            manual->position(-s, s, s); manual->normal(-1, 0, 0); manual->textureCoord(1, 0);
            manual->position(-s, s, -s); manual->normal(-1, 0, 0); manual->textureCoord(0, 0);
            
            // Right face (x = s)
            manual->position(s, -s, s); manual->normal(1, 0, 0); manual->textureCoord(0, 1);
            manual->position(s, -s, -s); manual->normal(1, 0, 0); manual->textureCoord(1, 1);
            manual->position(s, s, -s); manual->normal(1, 0, 0); manual->textureCoord(1, 0);
            manual->position(s, s, s); manual->normal(1, 0, 0); manual->textureCoord(0, 0);
            
            // Top face (y = s)
            manual->position(-s, s, s); manual->normal(0, 1, 0); manual->textureCoord(0, 1);
            manual->position(s, s, s); manual->normal(0, 1, 0); manual->textureCoord(1, 1);
            manual->position(s, s, -s); manual->normal(0, 1, 0); manual->textureCoord(1, 0);
            manual->position(-s, s, -s); manual->normal(0, 1, 0); manual->textureCoord(0, 0);
            
            // Bottom face (y = -s)
            manual->position(-s, -s, -s); manual->normal(0, -1, 0); manual->textureCoord(0, 1);
            manual->position(s, -s, -s); manual->normal(0, -1, 0); manual->textureCoord(1, 1);
            manual->position(s, -s, s); manual->normal(0, -1, 0); manual->textureCoord(1, 0);
            manual->position(-s, -s, s); manual->normal(0, -1, 0); manual->textureCoord(0, 0);
            
            // Define indices (2 triangles per face = 6 faces * 2 * 3 = 36 indices)
            for (int face = 0; face < 6; ++face) {
                const int base = face * 4;
                manual->triangle(base, base + 1, base + 2);
                manual->triangle(base, base + 2, base + 3);
            }
            
            manual->end();
            
            // Convert to mesh - this creates a shared mesh resource
            Ogre::MeshPtr mesh = manual->convertToMesh("ProgrammaticCube", 
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
            
            // The mesh needs proper bounding box
            mesh->_setBounds(Ogre::AxisAlignedBox(-s, -s, -s, s, s, s));
            mesh->_setBoundingSphereRadius(s * Ogre::Math::Sqrt(3.0f)); // diagonal of cube
            
            scene_mgr->destroyManualObject(manual);
        }

        // Create unit sphere mesh using UV sphere algorithm
        {
            const int rings = 16;
            const int segments = 16;
            const float radius = 0.5f;
            
            Ogre::ManualObject* manual = scene_mgr->createManualObject();
            // Use BaseWhite material which can be overridden
            manual->begin("BaseWhite", Ogre::RenderOperation::OT_TRIANGLE_LIST);
            
            // Generate sphere vertices
            for (int ring = 0; ring <= rings; ++ring) {
                const float theta = ring * Ogre::Math::PI / rings;
                const float sin_theta = Ogre::Math::Sin(theta);
                const float cos_theta = Ogre::Math::Cos(theta);
                
                for (int seg = 0; seg <= segments; ++seg) {
                    const float phi = seg * 2.0f * Ogre::Math::PI / segments;
                    const float sin_phi = Ogre::Math::Sin(phi);
                    const float cos_phi = Ogre::Math::Cos(phi);
                    
                    const float x = radius * sin_theta * cos_phi;
                    const float y = radius * cos_theta;
                    const float z = radius * sin_theta * sin_phi;
                    
                    manual->position(x, y, z);
                    manual->normal(x / radius, y / radius, z / radius); // normalized position is the normal
                    manual->textureCoord(static_cast<float>(seg) / segments, static_cast<float>(ring) / rings);
                }
            }
            
            // Generate sphere indices
            for (int ring = 0; ring < rings; ++ring) {
                for (int seg = 0; seg < segments; ++seg) {
                    const int current = ring * (segments + 1) + seg;
                    const int next = current + segments + 1;
                    
                    manual->triangle(current, next, current + 1);
                    manual->triangle(current + 1, next, next + 1);
                }
            }
            
            manual->end();
            
            // Convert to mesh - this creates a shared mesh resource
            Ogre::MeshPtr mesh = manual->convertToMesh("ProgrammaticSphere", 
                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
            
            // The mesh needs proper bounding box
            mesh->_setBounds(Ogre::AxisAlignedBox(-radius, -radius, -radius, radius, radius, radius));
            mesh->_setBoundingSphereRadius(radius);
            
            scene_mgr->destroyManualObject(manual);
        }
    }
}

GameWorld& GameWorld::instance() {
    static GameWorld instance;  // Meyer's singleton - thread-safe since C++11
    return instance;
}

GameWorld::GameWorld()
: abort_updates_(false)
, current_map_index_(0) 
, test_spotlight_node_(nullptr) { 
}

void GameWorld::create_scene() {
    // Create primitive meshes FIRST before anything tries to use them
    create_primitive_meshes(Common::scene_manager);
    
    // Disable all debug visualization globally
    Common::scene_manager->setDisplaySceneNodes(false);
    Common::scene_manager->showBoundingBoxes(false);
    
    // Set up dramatic lighting system to compensate for fixed function pipeline
    Common::scene_manager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
    Common::overview_scene_manager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);

    // Create a rotated root node for the overview scene (180 degrees around Y axis)
    // This flips the up/down direction to match player's initial facing
    if (!Common::overview_scene_manager->hasSceneNode("OverviewRotatedRoot")) {
        auto* rotated_root = Common::overview_scene_manager->getRootSceneNode()->createChildSceneNode("OverviewRotatedRoot");
        rotated_root->setOrientation(Ogre::Quaternion(Ogre::Degree(180), Ogre::Vector3::UNIT_Y));
    }

    // read map data and set up dramatic lighting
    const MapInfo& map_data = maps_[current_map_index_]->get_map_info();
    
    // Much lower ambient light for darker atmosphere
    Common::scene_manager->setAmbientLight(Ogre::ColourValue(0.1f, 0.1f, 0.15f));
    Common::overview_scene_manager->setAmbientLight(Ogre::ColourValue::White);
    
    if (map_data.fog_enabled) {
        Common::render_window->getViewport(0)->setBackgroundColour(map_data.fog_color);
        Common::scene_manager->setFog(Ogre::FOG_LINEAR, map_data.fog_color, 0.0, map_data.fog_start, map_data.fog_end);
    } 
    else {
        Common::render_window->getViewport(0)->setBackgroundColour(Ogre::ColourValue::Black);
        Common::scene_manager->setFog(); // sufficient to reset properties (defaults to FOG_NONE)
    }
    
    // load details from Map file and start creating
    const auto& brushes = maps_[current_map_index_]->get_brushes();
    const auto& lights = maps_[current_map_index_]->get_lights();
    const auto& entities = maps_[current_map_index_]->get_entities();

    // Create brushes
    for (const auto& brush : brushes) {
        create_brush(brush.get());
    }
    
    // Create lights
    for (const auto& light : lights) {
        create_light(light.get());
    }
    
    // Create entities
    for (const auto& entity : entities) {
        switch (entity->type) {
            case (EntityType::PlayerEntity):
                create_player(entity.get());
                break;

            case (EntityType::EnemyFlying):
            case (EntityType::EnemyStationary):
            case (EntityType::EnemyPatrol):
                create_enemy(entity.get());
                break;

            default:
                break;
        }
    }
    
    // Create spotlight that follows the player camera
    Ogre::Light* flashlight = Common::scene_manager->createLight("Flashlight");
    flashlight->setType(Ogre::Light::LT_SPOTLIGHT);
    
    // VERY bright center like a powerful headlight
    flashlight->setDiffuseColour(Ogre::ColourValue(3.0f, 2.8f, 2.4f));  // Extremely bright warm white
    flashlight->setSpecularColour(Ogre::ColourValue(2.0f, 1.9f, 1.6f)); // Strong specular highlights
    
    // Wide outer cone to fill viewport, tighter inner cone for bright center
    // Inner angle = bright center (like headlight beam)
    // Outer angle = soft glow that fills entire view
    flashlight->setSpotlightRange(Ogre::Degree(30), Ogre::Degree(60));  // 30° bright center, 60° soft outer glow
    
    // Much smoother distance falloff - primarily linear with minimal quadratic
    // Attenuation formula: intensity = 1 / (constant + linear*distance + quadratic*distance²)
    // Higher range, very small linear coefficient, almost no quadratic = smooth gradual falloff
    flashlight->setAttenuation(10000, 1.0f, 0.00007f, 0.0f);  // 10000 range, smooth linear falloff, no quadratic
    
    // Create scene node to control the flashlight position and direction
    test_spotlight_node_ = Common::scene_manager->getRootSceneNode()->createChildSceneNode("FlashlightNode");
    test_spotlight_node_->attachObject(flashlight);
    
    // Set initial position and direction (will be updated each frame to follow camera)
    test_spotlight_node_->setPosition(0, 0, 0);
    test_spotlight_node_->setDirection(0, 0, -1, Ogre::Node::TS_WORLD);
}

void GameWorld::create_light(const LightInfo* const light) {
    Ogre::Light* point_light = Common::scene_manager->createLight(light->id);
    point_light->setType(Ogre::Light::LT_POINT);
    point_light->setDiffuseColour(light->color);
    point_light->setSpecularColour(light->color);
    
    Ogre::SceneNode* node = Common::scene_manager->getRootSceneNode()->createChildSceneNode();
    node->setPosition(light->position);
    node->attachObject(point_light);
}

/*
 *  Creates a 'Brush' which is basically a prefab cube mesh scaled and textured. 
 *  A BrushEntity class is set as the UserAny of the scene node.
 *  Be careful not to use 'type', use 'brush_type'.
 */
void GameWorld::create_brush(const BrushInfo* const brush) {
    Ogre::Entity* brush_entity = Common::scene_manager->createEntity(brush->id, "ProgrammaticCube");
    
    // Use original materials for all brushes - no custom shaders for now
    brush_entity->setMaterialName(brush->material_name);
    
    brush_entity->setCastShadows(brush->cast_shadows);
    brush_entity->setQueryFlags(to_ogre_flags(QueryFlags::Wall));
    
    // create SceneNode
    Ogre::SceneNode* node = Common::scene_manager->getRootSceneNode()->createChildSceneNode();
    node->attachObject(brush_entity);
    
    // the brush's position is its 'start' point, so we need to compute the center of the brush
    node->setPosition(Ogre::Vector3(brush->position.x + brush->dimensions.x / 2.0f,
                                    brush->position.y + brush->dimensions.y / 2.0f,
                                    brush->position.z + brush->dimensions.z / 2.0f));
    // unit cube is 1x1x1, so we need to scale it to match the requested dimensions
    node->setScale(brush->dimensions);

    auto brush_user = std::make_unique<BrushEntity>(brush_entity, node, brush->brush_type);
    brush_user->set_type(EntityType::Brush);
    node->setUserAny(Ogre::Any(brush_user.get()));
    
    // we put brushes in a separate list because we don't want to iterate over them every time the game is updated
    // however, BrushEntities are needed and therefor must be created (and destroyed!) so we do keep track of them
    brushes_.push_back(std::move(brush_user));
    
    // overview: Create simple colored boxes to represent level geometry
    // Don't use the same materials to avoid conflicts with main scene
    if (brush->brush_type != BrushType::Ceiling && brush->brush_type != BrushType::Floor) {
        Ogre::Entity* brush_overview = Common::overview_scene_manager->createEntity(brush->id + "_overview", "ProgrammaticCube");
        
        // All materials are now in General resource group (examples.material file)
        std::string overview_material = "GameOverview/Gray";
        
        if (brush->brush_type == BrushType::Start) {
            overview_material = "Examples/Green";
        }
        else if (brush->brush_type == BrushType::Exit) {
            overview_material = "Examples/Blue";
        }
        
        brush_overview->setMaterialName(overview_material, "General");
        brush_overview->setCastShadows(false);
        brush_overview->setVisible(true); // Ensure visibility
        
        // Attach to rotated root so entire scene is flipped
        auto* rotated_root = Common::overview_scene_manager->getSceneNode("OverviewRotatedRoot");
        Ogre::SceneNode* node_overview = rotated_root->createChildSceneNode(brush->id + "_OverviewNode");
        node_overview->attachObject(brush_overview);
        node_overview->setVisible(true); // Ensure node visibility
        
        node_overview->setPosition(Ogre::Vector3(brush->position.x + brush->dimensions.x / 2.0f,
                                                brush->position.y + brush->dimensions.y / 2.0f,
                                                brush->position.z + brush->dimensions.z / 2.0f));
        
        // Scale down by factor of 1.0 (was 0.01 for old 100-unit cube.mesh, now using 1-unit ProgrammaticCube)
        float overview_scale = 1.0f;  // Changed from 0.01f to account for unit cube vs 100-unit mesh
        float min_visible_size = 2.0f; // Minimum size for narrow objects like columns
        
        // For start/exit areas, make them taller so they're more visible
        bool is_marker = (brush->brush_type == BrushType::Start || brush->brush_type == BrushType::Exit);
        if (is_marker) {
            node_overview->setScale(Ogre::Vector3(brush->dimensions.x * overview_scale, 1.0f, brush->dimensions.z * overview_scale));
        }
        else {
            // Regular walls - flat in XZ plane, but ensure minimum visibility for narrow objects (columns/pillars)
            float scale_x = std::max(brush->dimensions.x * overview_scale, min_visible_size);
            float scale_z = std::max(brush->dimensions.z * overview_scale, min_visible_size);
            node_overview->setScale(Ogre::Vector3(scale_x, 0.01f, scale_z));
        }
        
        // Check if this is a pillar/column part (base, top, or middle)
        bool is_pillar = (brush->id.find("pilar") != Ogre::String::npos);
        
        // Store overview brush info for vertical filtering
        overview_brushes_.push_back({
            node_overview,
            brush->position.y,
            brush->position.y + brush->dimensions.y,
            is_marker,  // Start/Exit markers should always be visible
            is_pillar   // Pillar parts should all be shown together
        });
    }
}

/*
 *  Creates an Enemy by calling the EnemyFactory. 
 *  The Enemy created is automatically registered (factory calls our register_entity function)
 */
void GameWorld::create_enemy(const EntityInfo* const enemy) {
	if (!Common::game_state.is_dead(enemy->id)) { // when loading a game, this enemy should not be created if it was dead
        try {
            EnemyFactory::instance().create_enemy(enemy->id, enemy->type, enemy->position);
        }
        catch (std::exception& e) {
            std::cerr << e.what() << '\n';
        }
    }
}

void GameWorld::create_player(const EntityInfo* player) {
    Ogre::SceneNode* const player_node = Common::scene_manager->getRootSceneNode()->createChildSceneNode(player->id + "_node");
    
    auto player_ptr = std::make_unique<Player>(player_node);
    player_ptr->set_type(EntityType::PlayerEntity);
    player_ptr->set_position(player->position);
	player_ptr->set_start_position(player->position);
    player_ptr->set_direction(player->direction);
    
    // Create sphere indicator for player on overview map
    Ogre::Entity* player_indicator = Common::overview_scene_manager->createEntity("PlayerIndicator", "ProgrammaticSphere");
    player_indicator->setMaterialName("Examples/Red", "General"); // Use examples.material from General group
    player_indicator->setCastShadows(false);
    
    // Attach to rotated root so player indicator is also flipped
    auto* rotated_root = Common::overview_scene_manager->getSceneNode("OverviewRotatedRoot");
    Ogre::SceneNode* player_overview_node = rotated_root->createChildSceneNode("PlayerOverviewNode");
    player_overview_node->attachObject(player_indicator);
    // Scale player indicator to be visible on the overview map (increased from 3.0 to 300.0 for unit sphere)
    player_overview_node->setScale(Ogre::Vector3(300.0f, 300.0f, 300.0f)); 
    player_overview_node->setPosition(player->position);
    
    // Keep raw pointer in Common for global access (we still own it via unique_ptr)
    Common::player = player_ptr.get();
    register_entity(std::move(player_ptr));
}

void GameWorld::destroy_scene() {
    abort_update(); // halt updating
    
    // Clear Common::player BEFORE clearing entities to prevent dangling pointer
    Common::player = nullptr;
    
    brushes_.clear();
    entities_.clear();
    overview_brushes_.clear(); // Clear overview brush tracking
    
    // tell Ogre to destroy all entities, lights and scenenodes
    Common::scene_manager->clearScene(); 
    Common::overview_scene_manager->clearScene();
    
    // Recreate ambient light for overview scene (cleared by clearScene)
    Common::overview_scene_manager->setAmbientLight(Ogre::ColourValue(1.0f, 1.0f, 1.0f));
    
    // Recreate the overview camera node that was destroyed by clearScene()
    if (Common::overview_camera && !Common::overview_scene_manager->hasSceneNode("OverviewCameraNode")) {
        auto* camera_node = Common::overview_scene_manager->getRootSceneNode()->createChildSceneNode("OverviewCameraNode");
        camera_node->attachObject(Common::overview_camera);
        camera_node->setOrientation(Ogre::Quaternion(Ogre::Degree(-90), Ogre::Vector3::UNIT_X));
        camera_node->setPosition(0, 10000, 0);
    }
}

void GameWorld::register_map(std::unique_ptr<Map> map) {
    maps_.push_back(std::move(map));
}

void GameWorld::load_map_by_index(size_t index) {
    if (index < maps_.size()) {
        destroy_scene();
        current_map_index_ = index;
        create_scene();
        ++current_map_index_; // Keep index pointing to next map for load_next_map() consistency
    }
}

void GameWorld::load_next_map() {
    if (current_map_index_ < maps_.size()) {
        destroy_scene();
        create_scene();
        ++current_map_index_;
    }
}

void GameWorld::reload_current_map() {
    if (current_map_index_ > 0 && current_map_index_ <= maps_.size()) {
        // Reload the map at index (current_map_index_ - 1) since we incremented after loading
        --current_map_index_;
        destroy_scene();
        create_scene();
        ++current_map_index_;
    }
}

void GameWorld::clear_map() {
    destroy_scene();
}

void GameWorld::register_entity(std::unique_ptr<GameEntity> entity) {
    entities_.push_back(std::move(entity));
}

void GameWorld::destroy_entity(GameEntity* const entity) {
    std::erase_if(entities_, [entity](const auto& e) {
        return e.get() == entity;
    });
}

void GameWorld::update(const Ogre::Real& elapsed) {
    if (abort_updates_) {
        abort_updates_ = false; // reset
        return;
    }
    
    // Update spotlight position AND orientation to follow camera exactly every frame
    if (test_spotlight_node_) {
        // Update position to camera position
        test_spotlight_node_->setPosition(Common::camera->getDerivedPosition());
        
        // Update orientation to match camera direction exactly
        // Camera looks down -Z axis, spotlight also looks down -Z axis by default
        test_spotlight_node_->setOrientation(Common::camera->getDerivedOrientation());
    }

    if (current_map_index_ > 0 && current_map_index_ <= maps_.size()) {
        // Process entities - remove dead/null entities and update alive ones
        std::erase_if(entities_, [this, elapsed](auto& entity) {
            if (!entity || !entity->is_alive()) {
                return true; // Remove dead or null entities
            }
            
            entity->update(elapsed);
            
            // If update triggered abort, stop processing but don't remove this entity
            if (abort_updates_) {
                return false;
            }
            
            return false; // Keep alive entities
        });
        
        // Early return if updates were aborted during entity processing
        if (abort_updates_) {
            return;
        }
    }
}

void GameWorld::abort_update() {
    abort_updates_ = true;
}

void GameWorld::update_overview_visibility(float player_y) {
    // Multi-pass algorithm:
    // Pass 1: Find the nearest floor below the player
    // Pass 2: Determine which pillars should be visible
    // Pass 3: Show walls and pillar parts based on floor level
    
    float nearest_floor_below = player_y - 10000.0f; // Start very low
    
    // Pass 1: Find the floor the player is standing on
    for (const auto& brush : overview_brushes_) {
        // Check if this brush's top surface could be a floor under the player
        if (brush.max_y <= player_y && brush.max_y > nearest_floor_below) {
            nearest_floor_below = brush.max_y;
        }
    }
    
    // If no floor found below, use player's Y position
    if (nearest_floor_below < player_y - 9000.0f) {
        nearest_floor_below = player_y;
    }
    
    // Pass 2: Determine which pillars touch the current floor
    // (so we can show all parts of those pillars)
    const float floor_snap_tolerance = 100.0f;
    const float ceiling_height = 3000.0f;
    std::set<Ogre::String> visible_pillars; // Track pillar names that should be fully visible
    
    for (const auto& brush : overview_brushes_) {
        if (!brush.is_pillar) {
            continue;
        }
        
        // Check if any part of this pillar is on the current floor
        bool starts_at_floor = std::abs(brush.min_y - nearest_floor_below) < floor_snap_tolerance;
        bool spans_floor = (brush.min_y <= nearest_floor_below + floor_snap_tolerance) 
                        && (brush.max_y >= nearest_floor_below);
        bool is_on_current_floor = (starts_at_floor || spans_floor) 
                                && (brush.max_y <= nearest_floor_below + ceiling_height);
        
        if (is_on_current_floor) {
            // Extract pillar base name (remove _pilar_base, _pilar_top suffixes)
            Ogre::String node_name = brush.node->getName();
            size_t pillar_pos = node_name.find("pilar");
            if (pillar_pos != Ogre::String::npos) {
                // Find the end of the pillar number (pilar0, pilar1, etc.)
                size_t end_pos = pillar_pos + 5; // "pilar" length
                while (end_pos < node_name.length() && isdigit(node_name[end_pos])) {
                    end_pos++;
                }
                Ogre::String pillar_base_name = node_name.substr(0, end_pos);
                visible_pillars.insert(pillar_base_name);
            }
        }
    }
    
    // Pass 3: Show walls and pillar parts based on floor level
    for (auto& brush : overview_brushes_) {
        // Always show Start/Exit markers regardless of floor
        if (brush.is_marker) {
            brush.node->setVisible(true);
            continue;
        }
        
        // For pillars: show all parts if any part is on current floor
        if (brush.is_pillar) {
            Ogre::String node_name = brush.node->getName();
            bool should_show = false;
            
            for (const auto& visible_pillar : visible_pillars) {
                if (node_name.find(visible_pillar) != Ogre::String::npos) {
                    should_show = true;
                    break;
                }
            }
            
            brush.node->setVisible(should_show);
            continue;
        }
        
        // Regular walls: Check if wall's bottom is at or near the floor level
        bool starts_at_floor = std::abs(brush.min_y - nearest_floor_below) < floor_snap_tolerance;
        
        // Or if wall bottom is slightly below floor (floor-to-floor walls)
        bool spans_floor = 
            (brush.min_y <= nearest_floor_below + floor_snap_tolerance) && 
            (brush.max_y >= nearest_floor_below);
        
        // Show if wall is part of current floor and not too tall above it
        bool is_current_floor_wall = 
            (starts_at_floor || spans_floor) && 
            (brush.max_y <= nearest_floor_below + ceiling_height);
        
        brush.node->setVisible(is_current_floor_wall);
    }
}
