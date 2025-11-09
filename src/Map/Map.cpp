#include "Map.hpp"


Map::Map()
: active_brush_(nullptr)
, active_light_(nullptr)
, active_entity_(nullptr) {}

Map::~Map() {
    // fastest way of deleting pointers in three different vectors
    size_t max = std::max(std::max(brushes_.size(), lights_.size()), entities_.size());
 
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
    set_color_member(map_info_.ambient_light, r, g, b);
}

void Map::set_map_fog_enable(bool enable_fog) {
    map_info_.fog_enabled = enable_fog;
}

void Map::set_map_fog_color(Ogre::Real r, Ogre::Real g, Ogre::Real b) {
    set_color_member(map_info_.fog_color, r, g, b);
}

void Map::set_map_fog_range(Ogre::Real start, Ogre::Real end) {
    map_info_.fog_start = start;
    map_info_.fog_end = end;
}

void Map::create_brush(const Ogre::String& name, BrushType type) {
    active_brush_ = new BrushInfo();
    active_brush_->id = name;
    active_brush_->type = EntityType::Brush;
    active_brush_->brush_type = type;
    active_brush_->cast_shadows = true; //otherwise light will go right through it, but it can be disabled if neccesary
    brushes_.push_back(active_brush_);    
}

void Map::set_brush_location(Ogre::Real x, Ogre::Real y, Ogre::Real z) {
    set_vector_member(active_brush_->position, x, y, z);
}

void Map::set_brush_size(Ogre::Real width, Ogre::Real height, Ogre::Real depth) {
    set_vector_member(active_brush_->dimensions, width, height, depth);
}

void Map::set_brush_shadows_enabled(bool enable_shadow_casting) {
    active_brush_->cast_shadows = enable_shadow_casting;
}

void Map::set_brush_material(const Ogre::String& material_name) {
    active_brush_->material_name = material_name;
}

void Map::build_cube_brush(const Ogre::String& cubeName, 
                         Ogre::Real x, Ogre::Real y, Ogre::Real z, 
                         Ogre::Real width, Ogre::Real height, Ogre::Real depth, 
                         CubePassThrough passthrough) {
    const Ogre::Real thickness = cube_thickness_;
    Ogre::Real wall_height = height  - (2* thickness);

    if (passthrough == CubePassThrough::AxisY) {
        wall_height = height;
        y -= thickness;
        height += thickness;
    }

    if (passthrough != CubePassThrough::AxisY) {
        create_brush(cubeName + "_floor", BrushType::Floor);
            set_brush_location(x, y, z);
            set_brush_size(width, thickness, depth);
            set_brush_material(floor_material_);
        create_brush(cubeName + "_ceil", BrushType::Ceiling);
            set_brush_location(x, y + height - thickness, z);
            set_brush_size(width, thickness, depth);
            set_brush_material(ceiling_material_);
    }

    if (passthrough != CubePassThrough::AxisX) {
        create_brush(cubeName + "_leftwall", BrushType::Wall);
            set_brush_location(x, y + thickness, z);
            set_brush_size(thickness, wall_height, depth);
            set_brush_material(wall_material_);
        
        create_brush(cubeName + "_rightwall", BrushType::Wall);
            set_brush_location(x + width - thickness, y + thickness, z);
            set_brush_size(thickness, wall_height, depth);
            set_brush_material(wall_material_);
    }

    if (passthrough != CubePassThrough::AxisZ) {
        /*
         *  The walls are not all of equal width. Left and Right walls are wider and the Front and Back walls fill up the 'gap'.
         *  However, because the Left and Right walls may might be omitted (if passthrough == CubePassThrough::AxisX),
         *  the Front and Back walls must be stretched a little to make up for lost space.
         */

        create_brush(cubeName + "_backwall", BrushType::Wall);
            if (passthrough == CubePassThrough::AxisX) {
                set_brush_location(x, y + thickness, z);
                set_brush_size(width, wall_height, thickness); // compensate width
            }
            else {
                set_brush_location(x + thickness, y + thickness, z);
                set_brush_size(width - (2* thickness), wall_height, thickness);
            }
            set_brush_material(wall_material_);

        create_brush(cubeName + "_frontwall", BrushType::Wall);
            if (passthrough == CubePassThrough::AxisX) {
                set_brush_location(x, y + thickness, z + depth - thickness);
                set_brush_size(width, wall_height, thickness);
            }
            else {
                set_brush_location(x + thickness, y + thickness, z + depth - thickness);
                set_brush_size(width - (2* thickness), wall_height, thickness);
            }
            set_brush_material(wall_material_);
    }
}

void Map::build_column(const Ogre::String& name, 
                      const Ogre::String& material_name, 
                      Ogre::Real x, Ogre::Real y, Ogre::Real z, 
                      Ogre::Real width, Ogre::Real height, Ogre::Real depth) {
    Ogre::Real base_height = 200.0f;

    create_brush(name + "_pilar_base", BrushType::Wall);
        set_brush_location(x, y, z); 
        set_brush_size(width, base_height, depth);   
        set_brush_material(material_name);

    create_brush(name + "_pilar_top", BrushType::Wall);
        set_brush_location(x, y + height - base_height, z); 
        set_brush_size(width, base_height, depth);   
        set_brush_material(material_name);

    create_brush(name, BrushType::Wall);
        set_brush_location(x + (width * 0.25f), y + base_height, z + (depth * 0.25f)); 
        set_brush_size(width * 0.5f, height - (2* base_height), depth * 0.5f);   
        set_brush_material(material_name);
}

void Map::build_launchpad(const Ogre::String& name, Ogre::Real x, Ogre::Real y, Ogre::Real z, bool is_exit) {
    // Note: dimensions and materials are hardcoded
    create_brush(name, (is_exit? BrushType::Exit : BrushType::Start));
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

void Map::set_floor_material(const Ogre::String& material_name) {
    floor_material_ = material_name;
}

void Map::set_ceiling_material(const Ogre::String& material_name) {
    ceiling_material_ = material_name;
}

void Map::set_wall_material(const Ogre::String& material_name) {
    wall_material_ = material_name;
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
    set_color_member(active_light_->color, r, g, b);
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
