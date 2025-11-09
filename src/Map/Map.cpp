#include "Map.hpp"


Map::Map()
: active_brush_(nullptr)
, active_light_(nullptr)
, active_entity_(nullptr) {}

Map::~Map() {
    // fastest way of deleting pointers in three different vectors
    size_t max = std::max(std::max(brushes_.size(), lights_.size()), entities_.size()); // determine largest vector
 
    for (size_t i = 0; i < max; ++i) {
        if (i < brushes_.size()) {
            delete brushes_.at(i);
        }
        if (i < lights_.size()) {
            delete lights_.at(i);
        }
        if (i < entities_.size()) {
            delete entities_.at(i);
        }
    }

    brushes_.clear();
    lights_.clear();
    entities_.clear();
}

// helpers to prevent duplicate code (a little)
void Map::set_color_member(Ogre::ColourValue& member, Ogre::Real r, Ogre::Real g, Ogre::Real b) {
    member.r = r;
    member.g = g;
    member.b = b;
    member.a = 1.0f;
}
 
void Map::set_vector_member(Ogre::Vector3& member, Ogre::Real x, Ogre::Real y, Ogre::Real z) {
    member.x = x;
    member.y = y;
    member.z = z;
}

void Map::set_map_name(const Ogre::String& mapName) {
    map_info_.name = mapName;
}

void Map::set_map_ambient_light(Ogre::Real r, Ogre::Real g, Ogre::Real b) {
    set_color_member(map_info_.ambientLight, r, g, b);
}

void Map::set_map_fog_enable(bool enableFog) {
    map_info_.fogEnabled = enableFog;
}

void Map::set_map_fog_color(Ogre::Real r, Ogre::Real g, Ogre::Real b) {
    set_color_member(map_info_.fogColour, r, g, b);
}

void Map::set_map_fog_range(Ogre::Real start, Ogre::Real end) {
    map_info_.fogStart = start;
    map_info_.fogEnd = end;
}

void Map::create_brush(const Ogre::String& name, BrushType type) {
    active_brush_ = new BrushInfo();
    active_brush_->id = name;
    active_brush_->type = EntityType::Brush;
    active_brush_->brushType = type;
    active_brush_->castShadows = true; //otherwise light will go right through it, but it can be disabled if neccesary
    brushes_.push_back(active_brush_);    
}

void Map::set_brush_location(Ogre::Real x, Ogre::Real y, Ogre::Real z) {
    set_vector_member(active_brush_->position, x, y, z);
}

void Map::set_brush_size(Ogre::Real width, Ogre::Real height, Ogre::Real depth) {
    set_vector_member(active_brush_->dimensions, width, height, depth);
}

void Map::set_brush_shadows_enabled(bool enableShadowCasting) {
    active_brush_->castShadows = enableShadowCasting;
}

void Map::set_brush_material(const Ogre::String& materialName) {
    active_brush_->materialName = materialName;
}

void Map::build_cube_brush(const Ogre::String& cubeName, 
                         Ogre::Real x, Ogre::Real y, Ogre::Real z, 
                         Ogre::Real width, Ogre::Real height, Ogre::Real depth, 
                         CubePassThrough passThrough) {
    const Ogre::Real thickness = cube_thickness_;
    Ogre::Real wallHeight = height  - (2* thickness);

    if (passThrough == CUBE_Y_AXIS) {
        wallHeight = height;
        y -= thickness;
        height += thickness;
    }

    if (passThrough != CUBE_Y_AXIS) {
        create_brush(cubeName + "_floor", BrushType::Floor);
            set_brush_location(x, y, z);
            set_brush_size(width, thickness, depth);
            set_brush_material(floor_material_);
        create_brush(cubeName + "_ceil", BrushType::Ceiling);
            set_brush_location(x, y + height - thickness, z);
            set_brush_size(width, thickness, depth);
            set_brush_material(ceiling_material_);
    }

    if (passThrough != CUBE_X_AXIS) {
        create_brush(cubeName + "_leftwall", BrushType::Wall);
            set_brush_location(x, y + thickness, z);
            set_brush_size(thickness, wallHeight, depth);
            set_brush_material(wall_material_);
        
        create_brush(cubeName + "_rightwall", BrushType::Wall);
            set_brush_location(x + width - thickness, y + thickness, z);
            set_brush_size(thickness, wallHeight, depth);
            set_brush_material(wall_material_);
    }

    if (passThrough != CUBE_Z_AXIS) {
        /*
         *  The walls are not all of equal width. Left and Right walls are wider and the Front and Back walls fill up the 'gap'.
         *  However, because the Left and Right walls may might be omitted (if passThrough == CUBE_X_AXIS),
         *  the Front and Back walls must be stretched a little to make up for lost space.
         */

        create_brush(cubeName + "_backwall", BrushType::Wall);
            if (passThrough == CUBE_X_AXIS) {
                set_brush_location(x, y + thickness, z);
                set_brush_size(width, wallHeight, thickness); // compensate width
            }
            else {
                set_brush_location(x + thickness, y + thickness, z);
                set_brush_size(width - (2* thickness), wallHeight, thickness);
            }
            set_brush_material(wall_material_);

        create_brush(cubeName + "_frontwall", BrushType::Wall);
            if (passThrough == CUBE_X_AXIS) {
                set_brush_location(x, y + thickness, z + depth - thickness);
                set_brush_size(width, wallHeight, thickness);
            }
            else {
                set_brush_location(x + thickness, y + thickness, z + depth - thickness);
                set_brush_size(width - (2* thickness), wallHeight, thickness);
            }
            set_brush_material(wall_material_);
    }
}

void Map::build_column(const Ogre::String& name, 
                      const Ogre::String& materialName, 
                      Ogre::Real x, Ogre::Real y, Ogre::Real z, 
                      Ogre::Real width, Ogre::Real height, Ogre::Real depth) {
    Ogre::Real baseHeight = 200.0f;

    create_brush(name + "_pilar_base", BrushType::Wall);
        set_brush_location(x, y, z); 
        set_brush_size(width, baseHeight, depth);   
        set_brush_material(materialName);

    create_brush(name + "_pilar_top", BrushType::Wall);
        set_brush_location(x, y + height - baseHeight, z); 
        set_brush_size(width, baseHeight, depth);   
        set_brush_material(materialName);

    create_brush(name, BrushType::Wall);
        set_brush_location(x + (width * 0.25f), y + baseHeight, z + (depth * 0.25f)); 
        set_brush_size(width * 0.5f, height - (2* baseHeight), depth * 0.5f);   
        set_brush_material(materialName);
}

void Map::build_launchpad(const Ogre::String& name, 
                         Ogre::Real x, Ogre::Real y, Ogre::Real z, 
                         bool isExit) {
    // Note: dimensions and materials are hardcoded
    create_brush(name, (isExit? BrushType::Exit : BrushType::Start));
        set_brush_location(x, y + 200.0f, z); 
        set_brush_size(800.0f, 5.0f, 800.0f);   
        set_brush_material("Ascent/Caution");

    create_brush(name + "_base", BrushType::Floor);
        set_brush_location(x, y, z); 
        set_brush_size(800.0f, 200.0f, 800.0f);   
        set_brush_material("Ascent/SeamlessConcrete2");
}

void Map::set_cube_brush_thickness(Ogre::Real thickness) {
    cube_thickness_ = thickness;
}

void Map::set_floor_material(const Ogre::String& materialName) {
    floor_material_ = materialName;
}

void Map::set_ceiling_material(const Ogre::String& materialName) {
    ceiling_material_ = materialName;
}

void Map::set_wall_material(const Ogre::String& materialName) {
    wall_material_ = materialName;
}

void Map::create_light(const Ogre::String& name) {
    active_light_ = new LightInfo();
    active_light_->id = name;
    lights_.push_back(active_light_);
}

void Map::set_light_location(Ogre::Real x, Ogre::Real y, Ogre::Real z) {
   set_vector_member(active_light_->position, x, y, z);
}

void Map::set_light_color(Ogre::Real r, Ogre::Real g, Ogre::Real b) {
    set_color_member(active_light_->colour, r, g, b);
}

void Map::create_entity(const Ogre::String& name) {
    active_entity_ = new EntityInfo();
    active_entity_->id = name;
    entities_.push_back(active_entity_);
}

void Map::set_entity_location(Ogre::Real x, Ogre::Real y, Ogre::Real z) {
    set_vector_member(active_entity_->position, x, y, z);
}

void Map::set_entity_orientation(Ogre::Real x, Ogre::Real y, Ogre::Real z) {
    set_vector_member(active_entity_->direction, x, y, z);
}

void Map::set_entity_type(EntityType type) {
    active_entity_->type = type;
}
