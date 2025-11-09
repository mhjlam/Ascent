#pragma once

#include <Ogre.h>

#include "Constants.hpp"

struct MapInfo {
    Ogre::String name;
    Ogre::ColourValue ambientLight;
    Ogre::ColourValue fogColour;
    Ogre::Real fogStart;
    Ogre::Real fogEnd;
    bool fogEnabled;
};

struct EntityInfo {
    Ogre::String id;    
    Ogre::Vector3 position;
    Ogre::Vector3 direction; // facing direction
    EntityType type;
};

struct LightInfo : EntityInfo {
    Ogre::ColourValue colour;
};

struct BrushInfo : EntityInfo {
    Ogre::String materialName;
    Ogre::Vector3 dimensions;
    BrushType brushType; // e.g. wall/ceiling/floor
    bool castShadows;
};

enum CubePassThrough {  
    CUBE_NONE,      // leave no sides out (default)
    CUBE_X_AXIS,    // leave the left and right walls
    CUBE_Y_AXIS,    // leave the ceiling and floor
    CUBE_Z_AXIS     // leave the front and back walls
};

   
class Map {
public:
    Map();
    virtual ~Map();

public: // setters
    size_t num_lights() const { 
        return lights_.size();
    } 
    size_t num_brushes() const { 
        return brushes_.size();
    }
    size_t num_entities() const {
        return entities_.size();
    }

public: // getters
    const std::vector<LightInfo*>& get_lights() {
        return lights_;
    }
    const std::vector<BrushInfo*>& get_brushes() {
        return brushes_;
    }
    const std::vector<EntityInfo*>& get_entities() {
        return entities_;
    }
    const MapInfo& get_map_info() { 
        return map_info_;
    }

protected:
    void set_map_name(const Ogre::String& mapName);
    void set_map_ambient_light(Ogre::Real r, Ogre::Real g, Ogre::Real b);
    void set_map_fog_enable(bool enableFog);
    void set_map_fog_color(Ogre::Real r, Ogre::Real g, Ogre::Real b);
    void set_map_fog_range(Ogre::Real start, Ogre::Real end);
    
    // brushes
    void create_brush(const Ogre::String& name, BrushType type);
    void set_brush_location(Ogre::Real x, Ogre::Real y, Ogre::Real z);
    void set_brush_size(Ogre::Real width, Ogre::Real height, Ogre::Real depth);
    void set_brush_shadows_enabled(bool enableShadowCasting);
    void set_brush_material(const Ogre::String& materialName);
    
    // 'prefabs'
    void build_cube_brush(const Ogre::String& cubeName, 
                        Ogre::Real x, Ogre::Real y, Ogre::Real z, 
                        Ogre::Real width, Ogre::Real height, Ogre::Real depth, 
                        CubePassThrough passThrough = CUBE_NONE);
    void build_column(const Ogre::String& name, 
                      const Ogre::String& materialName, 
                      Ogre::Real x, Ogre::Real y, Ogre::Real z, 
                      Ogre::Real width, Ogre::Real height, Ogre::Real depth);
    void build_launchpad(const Ogre::String& name, 
                         Ogre::Real x, Ogre::Real y, Ogre::Real z, 
                         bool isExit = false);
    void set_cube_brush_thickness(Ogre::Real thickness);
    void set_floor_material(const Ogre::String& materialName); 
    void set_ceiling_material(const Ogre::String& materialName); 
    void set_wall_material(const Ogre::String& materialName);  
    
    // lights
    void create_light(const Ogre::String& name);
    void set_light_location(Ogre::Real x, Ogre::Real y, Ogre::Real z);
    void set_light_color(Ogre::Real r, Ogre::Real g, Ogre::Real b);
    
    // enemies
    void create_entity(const Ogre::String& name);    
    void set_entity_location(Ogre::Real x, Ogre::Real y, Ogre::Real z);
    void set_entity_orientation(Ogre::Real x, Ogre::Real y, Ogre::Real z);
    void set_entity_type(EntityType type);

private:
    void set_color_member(Ogre::ColourValue& member, Ogre::Real r, Ogre::Real g, Ogre::Real b);
    void set_vector_member(Ogre::Vector3& member, Ogre::Real x, Ogre::Real y, Ogre::Real z);

private:
    MapInfo map_info_;
    
    std::vector<LightInfo*> lights_;
    std::vector<BrushInfo*> brushes_;
    std::vector<EntityInfo*> entities_;
    
    Ogre::String floor_material_;
    Ogre::String ceiling_material_;
    Ogre::String wall_material_;
    Ogre::Real cube_thickness_;
    
    BrushInfo* active_brush_;
    LightInfo* active_light_;
    EntityInfo* active_entity_;
};
