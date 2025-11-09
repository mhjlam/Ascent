#include "GameWorld.hpp"

#include <iostream>
#include <set>
#include "Common.hpp"

#include "Entities/BrushEntity.hpp"
#include "Entities/Player/Player.hpp"
#include "Entities/Enemies/EnemyFactory.hpp"

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
    // Set up dramatic lighting system to compensate for fixed function pipeline
    Common::scene_manager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
    Common::overview_scene_manager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);

    // Create a rotated root node for the overview scene (180 degrees around Y axis)
    // This flips the up/down direction to match player's initial facing
    if (!Common::overview_scene_manager->hasSceneNode("OverviewRotatedRoot")) {
        auto* rotatedRoot = Common::overview_scene_manager->getRootSceneNode()->createChildSceneNode("OverviewRotatedRoot");
        rotatedRoot->setOrientation(Ogre::Quaternion(Ogre::Degree(180), Ogre::Vector3::UNIT_Y));
    }

    // read map data and set up dramatic lighting
    const MapInfo& mapData = maps_[current_map_index_]->get_map_info();
    
    // Much lower ambient light for darker atmosphere
    Common::scene_manager->setAmbientLight(Ogre::ColourValue(0.1f, 0.1f, 0.15f));
    Common::overview_scene_manager->setAmbientLight(Ogre::ColourValue::White);
    
    if (mapData.fogEnabled) {
        Common::render_window->getViewport(0)->setBackgroundColour(mapData.fogColour);
        Common::scene_manager->setFog(Ogre::FOG_LINEAR, mapData.fogColour, 0.0, mapData.fogStart, mapData.fogEnd);
    } 
    else {
        Common::render_window->getViewport(0)->setBackgroundColour(Ogre::ColourValue::Black);
        Common::scene_manager->setFog(); // sufficient to reset properties (defaults to FOG_NONE)
    }
    
    // load details from Map file and start creating
    const std::vector<BrushInfo*>& brushes = maps_[current_map_index_]->get_brushes();
    const std::vector<LightInfo*>& lights = maps_[current_map_index_]->get_lights();
    const std::vector<EntityInfo*>& entities = maps_[current_map_index_]->get_entities();

    // the order in which they are loaded is not important, so we can do this in O(n) time instead of O(3n) by using:
    size_t max = std::max(std::max(lights.size(), brushes.size()), entities.size());
 
    for (size_t i = 0; i < max; ++i) {
        if (i < brushes.size()) {
            create_brush(brushes.at(i));
        }
        
        if (i < lights.size()) {
            create_light(lights.at(i));
        }

        if (i < entities.size()) {
            switch (entities.at(i)->type) {
                case (EntityType::PlayerEntity):
                    create_player(entities.at(i));
                    break;

                case (EntityType::EnemyFlying):
                case (EntityType::EnemyStationary):
                case (EntityType::EnemyPatrol):
                    create_enemy(entities.at(i));
                    break;

                default:
                    break;
            }
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
    Ogre::Light* pointLight = Common::scene_manager->createLight(light->id);
    pointLight->setType(Ogre::Light::LT_POINT);
    pointLight->setDiffuseColour(light->colour);
    pointLight->setSpecularColour(light->colour);
    
    Ogre::SceneNode* node = Common::scene_manager->getRootSceneNode()->createChildSceneNode();
    node->setPosition(light->position);
    node->attachObject(pointLight);
}

/*
 *  Creates a 'Brush' which is basically a prefab cube mesh scaled and textured. 
 *  A BrushEntity class is set as the UserAny of the scene node.
 *  Be careful not to use 'type', use 'brushType'.
 */
void GameWorld::create_brush(const BrushInfo* const brush) {
    // TEMPORARY: Go back to PT_CUBE but force manual mesh creation with proper normals
    Ogre::Entity* brushEntity = Common::scene_manager->createEntity(brush->id, Ogre::SceneManager::PT_CUBE);
    
    // Use original materials for all brushes - no custom shaders for now
    brushEntity->setMaterialName(brush->materialName);
    
    brushEntity->setCastShadows(brush->castShadows);
    brushEntity->setQueryFlags(static_cast<Ogre::uint32>(QueryFlags::Wall));
    
    // create SceneNode
    Ogre::SceneNode* node = Common::scene_manager->getRootSceneNode()->createChildSceneNode();
    node->attachObject(brushEntity);
    
    // the brush's position is its 'start' point, so we need to compute the center of the brush
    node->setPosition(Ogre::Vector3(brush->position.x + brush->dimensions.x / 2.0f,
                                    brush->position.y + brush->dimensions.y / 2.0f,
                                    brush->position.z + brush->dimensions.z / 2.0f));
    // prefab cube is 100x100x100, so we need to resize it to match the requested coordinates
    node->setScale(brush->dimensions * 0.01f);

    auto brushUser = std::make_unique<BrushEntity>(brushEntity, node, brush->brushType);
    brushUser->set_type(EntityType::Brush);
    node->setUserAny(Ogre::Any(brushUser.get()));
    
    // we put brushes in a separate list because we don't want to iterate over them every time the game is updated
    // however, BrushEntities are needed and therefor must be created (and destroyed!) so we do keep track of them
    brushes_.push_back(std::move(brushUser));
    
    // overview: Create simple colored boxes to represent level geometry
    // Don't use the same materials to avoid conflicts with main scene
    if (brush->brushType != BrushType::Ceiling && brush->brushType != BrushType::Floor) {
        Ogre::Entity* brushOverview = Common::overview_scene_manager->createEntity(brush->id + "_overview", "cube.mesh");
        
        // All materials are now in General resource group (examples.material file)
        std::string overviewMaterial = "GameOverview/Gray";
        
        if (brush->brushType == BrushType::Start) {
            overviewMaterial = "Examples/Green";
        }
        else if (brush->brushType == BrushType::Exit) {
            overviewMaterial = "Examples/Blue";
        }
        
        brushOverview->setMaterialName(overviewMaterial, "General");
        brushOverview->setCastShadows(false);
        brushOverview->setVisible(true); // Ensure visibility
        
        // Attach to rotated root so entire scene is flipped
        auto* rotatedRoot = Common::overview_scene_manager->getSceneNode("OverviewRotatedRoot");
        Ogre::SceneNode* nodeOverview = rotatedRoot->createChildSceneNode(brush->id + "_OverviewNode");
        nodeOverview->attachObject(brushOverview);
        nodeOverview->setVisible(true); // Ensure node visibility
        
        nodeOverview->setPosition(Ogre::Vector3(brush->position.x + brush->dimensions.x / 2.0f,
                                                brush->position.y + brush->dimensions.y / 2.0f,
                                                brush->position.z + brush->dimensions.z / 2.0f));
        
        // Scale down by factor of 0.01 (1%) for much smaller overview size
        float overviewScale = 0.01f;
        float minVisibleSize = 2.0f; // Minimum size for narrow objects like columns
        
        // For start/exit areas, make them taller so they're more visible
        bool isMarker = (brush->brushType == BrushType::Start || brush->brushType == BrushType::Exit);
        if (isMarker) {
            nodeOverview->setScale(Ogre::Vector3(brush->dimensions.x * overviewScale, 1.0f, brush->dimensions.z * overviewScale));
        } else {
            // Regular walls - flat in XZ plane, but ensure minimum visibility for narrow objects (columns/pillars)
            float scaleX = std::max(brush->dimensions.x * overviewScale, minVisibleSize);
            float scaleZ = std::max(brush->dimensions.z * overviewScale, minVisibleSize);
            nodeOverview->setScale(Ogre::Vector3(scaleX, 0.01f, scaleZ));
        }
        
        // Check if this is a pillar/column part (base, top, or middle)
        bool isPillar = (brush->id.find("pilar") != Ogre::String::npos);
        
        // Store overview brush info for vertical filtering
        overview_brushes_.push_back({
            nodeOverview,
            brush->position.y,
            brush->position.y + brush->dimensions.y,
            isMarker,  // Start/Exit markers should always be visible
            isPillar   // Pillar parts should all be shown together
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
        catch (Ogre::Exception& e) {
            std::cerr << e.what() << std::endl;
        }
    }
}

void GameWorld::create_player(const EntityInfo* player) {
    
    Ogre::SceneNode* const playerNode = Common::scene_manager->getRootSceneNode()->createChildSceneNode(player->id + "_node");
    
    auto playerPtr = std::make_unique<Player>(playerNode);
    playerPtr->set_type(EntityType::PlayerEntity);
    playerPtr->set_position(player->position);
	playerPtr->set_start_position(player->position);
    playerPtr->set_direction(player->direction);
    
    // Create sphere indicator for player on overview map
    Ogre::Entity* playerIndicator = Common::overview_scene_manager->createEntity("PlayerIndicator", "sphere.mesh");
    playerIndicator->setMaterialName("Examples/Red", "General"); // Use examples.material from General group
    playerIndicator->setCastShadows(false);
    
    // Attach to rotated root so player indicator is also flipped
    auto* rotatedRoot = Common::overview_scene_manager->getSceneNode("OverviewRotatedRoot");
    Ogre::SceneNode* playerOverviewNode = rotatedRoot->createChildSceneNode("PlayerOverviewNode");
    playerOverviewNode->attachObject(playerIndicator);
    // Scale player indicator to be visible on the overview map
    playerOverviewNode->setScale(Ogre::Vector3(3.0f, 3.0f, 3.0f)); 
    playerOverviewNode->setPosition(player->position);
    
    // Keep raw pointer in Common for global access (we still own it via unique_ptr)
    Common::player = playerPtr.get();
    register_entity(std::move(playerPtr));
}

void GameWorld::destroy_scene() {
    abort_update(); // halt updating
    
    // unique_ptr automatically deletes
    entities_.clear();   
    brushes_.clear();
    overview_brushes_.clear(); // Clear overview brush tracking
     
    // tell Ogre to destroy all entities, lights and scenenodes
    Common::scene_manager->clearScene(); 
    Common::overview_scene_manager->clearScene();
    
    // Recreate ambient light for overview scene (cleared by clearScene)
    Common::overview_scene_manager->setAmbientLight(Ogre::ColourValue(1.0f, 1.0f, 1.0f));
    
    // Recreate the overview camera node that was destroyed by clearScene()
    if (Common::overview_camera && !Common::overview_scene_manager->hasSceneNode("OverviewCameraNode")) {
        auto* camNode = Common::overview_scene_manager->getRootSceneNode()->createChildSceneNode("OverviewCameraNode");
        camNode->attachObject(Common::overview_camera);
        camNode->setOrientation(Ogre::Quaternion(Ogre::Degree(-90), Ogre::Vector3::UNIT_X));
        camNode->setPosition(0, 10000, 0);
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
    auto it = entities_.begin();

    while (it != entities_.end()) {
        if (it->get() == entity) {
            entities_.erase(it); // unique_ptr automatically deletes
            break;
        }
        ++it;
    }
}

void GameWorld::update(const Ogre::Real& elapsed) {
    if (abort_updates_) {
        abort_updates_ = false; // reset
        return;
    }
    
    // Update spotlight position AND orientation to follow camera exactly every frame
    if (test_spotlight_node_ != nullptr) {
        // Update position to camera position
        test_spotlight_node_->setPosition(Common::camera->getDerivedPosition());
        
        // Update orientation to match camera direction exactly
        // Camera looks down -Z axis, spotlight also looks down -Z axis by default
        test_spotlight_node_->setOrientation(Common::camera->getDerivedOrientation());
    }

    if (current_map_index_ > 0 && current_map_index_ <= maps_.size()) {
        auto it = entities_.begin();
        Ogre::uint i = 0; // used for manually resetting the iterator when required

        while (it != entities_.end()) {
            if (*it != nullptr) {
                if ((*it)->is_alive()) { 
                    // might kill the object, so it will be removed the next time our update is called
                    (*it)->update(elapsed);
                    
                    // stop this instant, the iterator is invalid as the scene has been cleared or something similar
                    if (abort_updates_)  {
                        return;
                    }
                    
                    ++it;
                    ++i;

                    // all is fine, continue with the next entity
                    continue;
                } 
            }
            
            // if all is not fine (entity is nullptr or dead), remove it
            destroy_entity(it->get());

            // reset the iterator (see destroy_entity for details)
            it = entities_.begin();

            // the item is removed from the list, so we want to go back to i
            std::advance(it, i);     
        }
    }
}

void GameWorld::abort_update() {
    abort_updates_ = true;
}

void GameWorld::update_overview_visibility(float playerY) {
    // Multi-pass algorithm:
    // Pass 1: Find the nearest floor below the player
    // Pass 2: Determine which pillars should be visible
    // Pass 3: Show walls and pillar parts based on floor level
    
    float nearestFloorBelow = playerY - 10000.0f; // Start very low
    
    // Pass 1: Find the floor the player is standing on
    for (const auto& brush : overview_brushes_) {
        // Check if this brush's top surface could be a floor under the player
        if (brush.maxY <= playerY && brush.maxY > nearestFloorBelow) {
            nearestFloorBelow = brush.maxY;
        }
    }
    
    // If no floor found below, use player's Y position
    if (nearestFloorBelow < playerY - 9000.0f) {
        nearestFloorBelow = playerY;
    }
    
    // Pass 2: Determine which pillars touch the current floor
    // (so we can show all parts of those pillars)
    const float floorSnapTolerance = 100.0f;
    const float ceilingHeight = 3000.0f;
    std::set<Ogre::String> visiblePillars; // Track pillar names that should be fully visible
    
    for (const auto& brush : overview_brushes_) {
        if (!brush.isPillar) continue;
        
        // Check if any part of this pillar is on the current floor
        bool startsAtFloor = std::abs(brush.minY - nearestFloorBelow) < floorSnapTolerance;
        bool spansFloor = (brush.minY <= nearestFloorBelow + floorSnapTolerance) && 
                         (brush.maxY >= nearestFloorBelow);
        bool isOnCurrentFloor = (startsAtFloor || spansFloor) && 
                               (brush.maxY <= nearestFloorBelow + ceilingHeight);
        
        if (isOnCurrentFloor) {
            // Extract pillar base name (remove _pilar_base, _pilar_top suffixes)
            Ogre::String nodeName = brush.node->getName();
            size_t pilarPos = nodeName.find("pilar");
            if (pilarPos != Ogre::String::npos) {
                // Find the end of the pillar number (pilar0, pilar1, etc.)
                size_t endPos = pilarPos + 5; // "pilar" length
                while (endPos < nodeName.length() && isdigit(nodeName[endPos])) {
                    endPos++;
                }
                Ogre::String pillarBaseName = nodeName.substr(0, endPos);
                visiblePillars.insert(pillarBaseName);
            }
        }
    }
    
    // Pass 3: Show walls and pillar parts based on floor level
    for (auto& brush : overview_brushes_) {
        // Always show Start/Exit markers regardless of floor
        if (brush.isMarker) {
            brush.node->setVisible(true);
            continue;
        }
        
        // For pillars: show all parts if any part is on current floor
        if (brush.isPillar) {
            Ogre::String nodeName = brush.node->getName();
            bool shouldShow = false;
            
            for (const auto& visiblePillar : visiblePillars) {
                if (nodeName.find(visiblePillar) != Ogre::String::npos) {
                    shouldShow = true;
                    break;
                }
            }
            
            brush.node->setVisible(shouldShow);
            continue;
        }
        
        // Regular walls: Check if wall's bottom is at or near the floor level
        bool startsAtFloor = std::abs(brush.minY - nearestFloorBelow) < floorSnapTolerance;
        
        // Or if wall bottom is slightly below floor (floor-to-floor walls)
        bool spansFloor = (brush.minY <= nearestFloorBelow + floorSnapTolerance) && 
                         (brush.maxY >= nearestFloorBelow);
        
        // Show if wall is part of current floor and not too tall above it
        bool isCurrentFloorWall = (startsAtFloor || spansFloor) && 
                                  (brush.maxY <= nearestFloorBelow + ceilingHeight);
        
        brush.node->setVisible(isCurrentFloorWall);
    }
}
