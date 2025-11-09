
#include "LevelTwo.hpp"

LevelTwo::LevelTwo() : Map() {
    // map settings
    set_map_name("Armory");
    set_map_ambient_light(0.05f, 0.05f, 0.05f);
    set_map_fog_enable(true);
    set_map_fog_color(0.0f, 0.0f, 0.0f);
    set_map_fog_range(2500.0f, 5000.0f);

    // player settings
     create_entity("_player");
        set_entity_location(0.0f, 500.0f, 0.0f);
        set_entity_orientation(0.0f, 0.0f, 1.0f);
        set_entity_type(EntityType::PlayerEntity);
    
    // build the map
    set_floor_material("Ascent/MetalSmall");
    set_ceiling_material("Ascent/SeamlessConcrete2");
    set_wall_material("Ascent/SeamlessConcrete");
    set_cube_brush_thickness(500.0f);
    build_cube_brush("startArea", -3000.0f, -500.0f, -3000.0f, 6000.0f, 3000.0f, 6000.0f, CubePassThrough::AxisZ);
    build_cube_brush("middleArea", -3000.0f, -500.0f, 3000.0f, 6000.0f, 3000.0f, 6000.0f, CubePassThrough::AxisZ);
    build_cube_brush("endArea", -3000.0f, -500.0f, 9000.0f, 6000.0f, 3000.0f, 6000.0f, CubePassThrough::AxisZ);

    create_brush("startAreaBack", BrushType::Wall);
        set_brush_location(-2500.0f, 0.0f, -3000.0f);
        set_brush_size(5000.0f, 2000.0f, 500.0f);
        set_brush_material("Ascent/SeamlessConcrete");

    create_brush("endAreaFront", BrushType::Wall);
        set_brush_location(-2500.0f, 0.0f, 14500.0f);
        set_brush_size(5000.0f, 2000.0f, 500.0f);
        set_brush_material("Ascent/SeamlessConcrete");

    build_column("pilar0", "Ascent/SeamlessMetal", -1800.0f, 0.0f, 1000.0f, 400.0f, 2000.0f, 400.0f);
    build_column("pilar1", "Ascent/SeamlessMetal", 1600.0f, 0.0f, 1000.0f, 400.0f, 2000.0f, 400.0f);
    build_column("pilar2", "Ascent/SeamlessMetal", -1800.0f, 0.0f, 12000.0f, 400.0f, 2000.0f, 400.0f);
    build_column("pilar3", "Ascent/SeamlessMetal", 1600.0f, 0.0f, 12000.0f, 400.0f, 2000.0f, 400.0f);
    
    // these pilars will have enemies on them
    build_column("pilar4", "Ascent/SeamlessMetal", -1800.0f, 0.0f, 6000.0f, 400.0f, 1000.0f, 400.0f);
    build_column("pilar5", "Ascent/SeamlessMetal", 1600.0f, 0.0f, 6000.0f, 400.0f, 1000.0f, 400.0f);
    build_column("pilar6", "Ascent/SeamlessMetal", -1800.0f, 0.0f, 9000.0f, 400.0f, 1000.0f, 400.0f);
    build_column("pilar7", "Ascent/SeamlessMetal", 1600.0f, 0.0f, 9000.0f, 400.0f, 1000.0f, 400.0f);
    
    build_launchpad("startArea", -400.0f, 0.0f, -400.0f);
    build_launchpad("endArea", -400.0f, 0.0f, 13000.0f, true);

    create_light("basicLight");
        set_light_location(0.0f, 1000.0f, 5000.0f);
        set_light_color(0.8f, 0.8f, 0.5f);

    // enemies
    create_entity("Harold"); // pilar4
        set_entity_location(-1600.0f, 1000.0f, 6200.0f);
        set_entity_type(EntityType::EnemyStationary);

    create_entity("James"); // pilar 5
        set_entity_location(1800.0f, 1000.0f, 6200.0f);
        set_entity_type(EntityType::EnemyStationary);

    create_entity("Rofle"); // pilar6
        set_entity_location(-1600.0f, 1000.0f, 9200.0f);
        set_entity_type(EntityType::EnemyStationary);

    create_entity("Gerald"); // pilar 7
        set_entity_location(1800.0f, 1000.0f, 9200.0f);
        set_entity_type(EntityType::EnemyStationary);

    // flying enemies
    create_entity("Rodney");
        set_entity_location(0.0f, 250.0f, 3000.0f);
        set_entity_type(EntityType::EnemyFlying);

    create_entity("Haris");
        set_entity_location(0.0f, 750.0f, 4500.0f);
        set_entity_type(EntityType::EnemyFlying);

    create_entity("George");
        set_entity_location(0.0f, 1000.0f, 6000.0f);
        set_entity_type(EntityType::EnemyFlying);

    create_entity("Fred");
        set_entity_location(0.0f, 450.0f, 9000.0f);
        set_entity_type(EntityType::EnemyFlying);
     
    create_entity("Cyril");
        set_entity_location(-250.0f, 205.0f, 13500.0f);
        set_entity_type(EntityType::EnemyPatrol);
}
