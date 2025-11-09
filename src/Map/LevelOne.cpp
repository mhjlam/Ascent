
#include "LevelOne.hpp"

LevelOne::LevelOne() : Map() {
    // map settings
    set_map_name("Factory");
    set_map_ambient_light(0.05f, 0.05f, 0.05f);
    set_map_fog_enable(true);
    set_map_fog_color(0.0f, 0.0f, 0.0f);
    set_map_fog_range(200.0f, 7500.0f);
   
    // player settings
    create_entity("_player");
        set_entity_location(0.0f, 500.0f, 0.0f);
        set_entity_orientation(0.0f, 0.0f, 1.0f);
        set_entity_type(EntityType::PlayerEntity);
    
    // build the map
    set_floor_material("Ascent/SeamlessMetal");
    set_ceiling_material("Ascent/SeamlessConcrete2");
    set_wall_material("Ascent/SeamlessConcrete");
    set_cube_brush_thickness(500.0f);
    build_cube_brush("startArea", -3000.0f, -500.0f, -3000.0f, 6000.0f, 3000.0f, 6000.0f, CubePassThrough::AxisZ);
    
    create_brush("startAreaBackWall", BrushType::Wall);
        set_brush_location(-2500.0f, 0.0f, -3000.0f); // -2500 = -3000 + thickness
        set_brush_size(5000.0f, 2000.0f, 500.0f);   // 5000 = 2 * 2500; 2000 = 3000 - 2 * thickness
        set_brush_material("Ascent/SeamlessConcrete");

    // create a tunnel
    set_wall_material("Ascent/SeamlessMetal");
    set_cube_brush_thickness(2000.0f);
    build_cube_brush("tunnel0", -3000.0f, -5000.0f, 3000.0f, 6000.0f, 5000.0f, 6000.0f, CubePassThrough::AxisY);

    create_brush("startAreaXtendedCeil", BrushType::Ceiling);
        set_brush_location(-3000.0f, 2000.0f, 3000.0f); 
        set_brush_size(6000.0f, 500.0f, 6000.0f);   
        set_brush_material("Ascent/SeamlessConcrete2");

    create_brush("startAreaXtendedFront", BrushType::Wall);
        set_brush_location(-3000.0f, 0.0f, 9000.0f); 
        set_brush_size(6000.0f, 2000.0f, 6000.0f);   
        set_brush_material("Ascent/SeamlessConcrete");

    create_brush("startAreaXtendedLeft", BrushType::Wall);
        set_brush_location(-3000.0f, 0.0f, 3000.0f); 
        set_brush_size(500.0f, 2000.0f, 6000.0f);   
        set_brush_material("Ascent/SeamlessConcrete");

    create_brush("startAreaXtendedRight", BrushType::Wall);
        set_brush_location(2500.0f, 0.0f, 3000.0f); 
        set_brush_size(500.0f, 2000.0f, 6000.0f);   
        set_brush_material("Ascent/SeamlessConcrete");

    // create the lower area
    create_brush("lowerAreaBottom", BrushType::Floor);
        set_brush_location(-1500.0f, -6500.0f, 3000.0f); 
        set_brush_size(3000.0f, 500.0f, 9000.0f);   
        set_brush_material("Ascent/SeamlessCircuit");

    create_brush("lowerAreaBack", BrushType::Wall);
        set_brush_location(-1000.0f, -6000.0f, 3000.0f); 
        set_brush_size(2000.0f, 1000.0f, 500.0f);   
        set_brush_material("Ascent/SeamlessConcrete2");

    create_brush("lowerAreaFront", BrushType::Wall);
        set_brush_location(-1000.0f, -6000.0f, 11500.0f); 
        set_brush_size(2000.0f, 1000.0f, 500.0f);   
        set_brush_material("Ascent/SeamlessConcrete2");

    create_brush("lowerAreaRight", BrushType::Wall);
        set_brush_location(1000.0f, -6000.0f, 3000.0f); 
        set_brush_size(500.0f, 1000.0f, 9000.0f);   
        set_brush_material("Ascent/SeamlessConcrete2");

    create_brush("lowerAreaExtra", BrushType::Wall);
        set_brush_location(980.0f, -5500.0f, 3000.0f); 
        set_brush_size(20.0f, 100.0f, 9000.0f);   
        set_brush_material("Ascent/Caution");

    create_brush("lowerAreaLeft", BrushType::Wall);
        set_brush_location(-1500.0f, -6000.0f, 3000.0f); 
        set_brush_size(500.0f, 1000.0f, 9000.0f);   
        set_brush_material("Ascent/SeamlessConcrete2");

   create_brush("lowerAreaTop", BrushType::Ceiling);
        set_brush_location(-1500.0f, -5000.0f, 9000.0f); 
        set_brush_size(3000.0f, 500.0f, 3000.0f);   
        set_brush_material("Ascent/SeamlessMetal");

    // extras
    build_column("pilar0", "Ascent/SeamlessConcrete", -1800.0f, 0.0f, 1000.0f, 400.0f, 2000.0f, 400.0f);
    build_column("pilar1", "Ascent/SeamlessConcrete", 1600.0f, 0.0f, 1000.0f, 400.0f, 2000.0f, 400.0f);

    build_launchpad("startArea", -400.0f, 0.0f, -400.0f);
    build_launchpad("endArea", -400.0f, -6000.0f, 9000.0f, true);

    // Lights removed for testing - using only test point light in GameWorld

    // add some enemies
    create_entity("Gerald");
        set_entity_location(-1700.0f, 0.0f, 7000.0f);
        set_entity_type(EntityType::EnemyPatrol);

    create_entity("Marvin");
        set_entity_location(1700.0f, 0.0f, 7000.0f);
        set_entity_type(EntityType::EnemyPatrol);

    create_entity("Matt");
        set_entity_location(500.0f, -6000.0f, 7800.0f);
        set_entity_type(EntityType::EnemyStationary);
    
    create_entity("Cheryl");
        set_entity_location(-250.0f, -5795.0f, 9500.0f);
        set_entity_type(EntityType::EnemyPatrol);    
}
