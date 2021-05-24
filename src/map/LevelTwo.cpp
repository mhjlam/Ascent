#include "stdafx.h"
#include "LevelTwo.h"

LevelTwo::LevelTwo() : Map()
{
    // map settings
    setMapName("Armory");
    setMapAmbientLight(0.35f, 0.35f, 0.35f);
    setMapFogEnabled(true);
    setMapFogColour(0.0f, 0.0f, 0.0f);
    setMapFogRange(2500.0f, 5000.0f);

    // player settings
     createEntity("_player");
        setEntityLocation(0.0f, 500.0f, 0.0f);
        setEntityFacingDirection(0.0f, 0.0f, 1.0f);
        setEntityType(ENT_PLAYER);
    
    // build the map
    setFloorMaterial("Ascent/MetalSmall");
    setCeilMaterial("Ascent/SeamlessConcrete2");
    setWallMaterial("Ascent/SeamlessConcrete");
    setCubeBrushThickness(500.0f);
    buildCubeBrush("startArea", -3000.0f, -500.0f, -3000.0f, 6000.0f, 3000.0f, 6000.0f, CUBE_Z_AXIS);
    buildCubeBrush("middleArea", -3000.0f, -500.0f, 3000.0f, 6000.0f, 3000.0f, 6000.0f, CUBE_Z_AXIS);
    buildCubeBrush("endArea", -3000.0f, -500.0f, 9000.0f, 6000.0f, 3000.0f, 6000.0f, CUBE_Z_AXIS);

    createBrush("startAreaBack", BRUSH_WALL);
        setBrushLocation(-2500.0f, 0.0f, -3000.0f);
        setBrushSize(5000.0f, 2000.0f, 500.0f);
        setBrushMaterial("Ascent/SeamlessConcrete");

    createBrush("endAreaFront", BRUSH_WALL);
        setBrushLocation(-2500.0f, 0.0f, 14500.0f);
        setBrushSize(5000.0f, 2000.0f, 500.0f);
        setBrushMaterial("Ascent/SeamlessConcrete");

    buildColumn("pilar0", "Ascent/SeamlessMetal", -1800.0f, 0.0f, 1000.0f, 400.0f, 2000.0f, 400.0f);
    buildColumn("pilar1", "Ascent/SeamlessMetal", 1600.0f, 0.0f, 1000.0f, 400.0f, 2000.0f, 400.0f);
    buildColumn("pilar2", "Ascent/SeamlessMetal", -1800.0f, 0.0f, 12000.0f, 400.0f, 2000.0f, 400.0f);
    buildColumn("pilar3", "Ascent/SeamlessMetal", 1600.0f, 0.0f, 12000.0f, 400.0f, 2000.0f, 400.0f);
    
    // these pilars will have enemies on them
    buildColumn("pilar4", "Ascent/SeamlessMetal", -1800.0f, 0.0f, 6000.0f, 400.0f, 1000.0f, 400.0f);
    buildColumn("pilar5", "Ascent/SeamlessMetal", 1600.0f, 0.0f, 6000.0f, 400.0f, 1000.0f, 400.0f);
    buildColumn("pilar6", "Ascent/SeamlessMetal", -1800.0f, 0.0f, 9000.0f, 400.0f, 1000.0f, 400.0f);
    buildColumn("pilar7", "Ascent/SeamlessMetal", 1600.0f, 0.0f, 9000.0f, 400.0f, 1000.0f, 400.0f);
    
    buildLaunchPad("startArea", -400.0f, 0.0f, -400.0f);
    buildLaunchPad("endArea", -400.0f, 0.0f, 13000.0f, true);

    createLight("basicLight");
        setLightLocation(0.0f, 1000.0f, 5000.0f);
        setLightColour(0.8f, 0.8f, 0.5f);

    // enemies
    createEntity("Harold"); // pilar4
        setEntityLocation(-1600.0f, 1000.0f, 6200.0f);
        setEntityType(ENT_ENEMY_STATIONARY);

    createEntity("James"); // pilar 5
        setEntityLocation(1800.0f, 1000.0f, 6200.0f);
        setEntityType(ENT_ENEMY_STATIONARY);

    createEntity("Rofle"); // pilar6
        setEntityLocation(-1600.0f, 1000.0f, 9200.0f);
        setEntityType(ENT_ENEMY_STATIONARY);

    createEntity("Gerald"); // pilar 7
        setEntityLocation(1800.0f, 1000.0f, 9200.0f);
        setEntityType(ENT_ENEMY_STATIONARY);

    // flying enemies
    createEntity("Rodney");
        setEntityLocation(0.0f, 250.0f, 3000.0f);
        setEntityType(ENT_ENEMY_FLYING);

    createEntity("Haris");
        setEntityLocation(0.0f, 750.0f, 4500.0f);
        setEntityType(ENT_ENEMY_FLYING);

    createEntity("George");
        setEntityLocation(0.0f, 1000.0f, 6000.0f);
        setEntityType(ENT_ENEMY_FLYING);

    createEntity("Fred");
        setEntityLocation(0.0f, 450.0f, 9000.0f);
        setEntityType(ENT_ENEMY_FLYING);
     
    createEntity("Cyril");
        setEntityLocation(-250.0f, 205.0f, 13500.0f);
        setEntityType(ENT_ENEMY_PATROL);
}