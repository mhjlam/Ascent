#include "stdafx.h"
#include "LevelOne.h"

LevelOne::LevelOne() : Map()
{
    // map settings
    setMapName("Factory");
    setMapAmbientLight(0.4f, 0.4f, 0.4f);
    setMapFogEnabled(true);
    setMapFogColour(0.0f, 0.0f, 0.0f);
    setMapFogRange(200.0f, 7500.0f);
   
    // player settings
    createEntity("_player");
        setEntityLocation(0.0f, 500.0f, 0.0f);
        setEntityFacingDirection(0.0f, 0.0f, 1.0f);
        setEntityType(ENT_PLAYER);
    
    // build the map
    setFloorMaterial("Ascent/SeamlessMetal");
    setCeilMaterial("Ascent/SeamlessConcrete2");
    setWallMaterial("Ascent/SeamlessConcrete");
    setCubeBrushThickness(500.0f);
    buildCubeBrush("startArea", -3000.0f, -500.0f, -3000.0f, 6000.0f, 3000.0f, 6000.0f, CUBE_Z_AXIS);
    
    createBrush("startAreaBackWall", BRUSH_WALL);
        setBrushLocation(-2500.0f, 0.0f, -3000.0f); // -2500 = -3000 + thickness
        setBrushSize(5000.0f, 2000.0f, 500.0f);   // 5000 = 2 * 2500; 2000 = 3000 - 2 * thickness
        setBrushMaterial("Ascent/SeamlessConcrete");

    // create a tunnel
    setWallMaterial("Ascent/SeamlessMetal");
    setCubeBrushThickness(2000.0f);
    buildCubeBrush("tunnel0", -3000.0f, -5000.0f, 3000.0f, 6000.0f, 5000.0f, 6000.0f, CUBE_Y_AXIS);

    createBrush("startAreaXtendedCeil", BRUSH_CEIL);
        setBrushLocation(-3000.0f, 2000.0f, 3000.0f); 
        setBrushSize(6000.0f, 500.0f, 6000.0f);   
        setBrushMaterial("Ascent/SeamlessConcrete2");

    createBrush("startAreaXtendedFront", BRUSH_WALL);
        setBrushLocation(-3000.0f, 0.0f, 9000.0f); 
        setBrushSize(6000.0f, 2000.0f, 6000.0f);   
        setBrushMaterial("Ascent/SeamlessConcrete");

    createBrush("startAreaXtendedLeft", BRUSH_WALL);
        setBrushLocation(-3000.0f, 0.0f, 3000.0f); 
        setBrushSize(500.0f, 2000.0f, 6000.0f);   
        setBrushMaterial("Ascent/SeamlessConcrete");

    createBrush("startAreaXtendedRight", BRUSH_WALL);
        setBrushLocation(2500.0f, 0.0f, 3000.0f); 
        setBrushSize(500.0f, 2000.0f, 6000.0f);   
        setBrushMaterial("Ascent/SeamlessConcrete");

    // create the lower area
    createBrush("lowerAreaBottom", BRUSH_FLOOR);
        setBrushLocation(-1500.0f, -6500.0f, 3000.0f); 
        setBrushSize(3000.0f, 500.0f, 9000.0f);   
        setBrushMaterial("Ascent/SeamlessCircuit");

    createBrush("lowerAreaBack", BRUSH_WALL);
        setBrushLocation(-1000.0f, -6000.0f, 3000.0f); 
        setBrushSize(2000.0f, 1000.0f, 500.0f);   
        setBrushMaterial("Ascent/SeamlessConcrete2");

    createBrush("lowerAreaFront", BRUSH_WALL);
        setBrushLocation(-1000.0f, -6000.0f, 11500.0f); 
        setBrushSize(2000.0f, 1000.0f, 500.0f);   
        setBrushMaterial("Ascent/SeamlessConcrete2");

    createBrush("lowerAreaRight", BRUSH_WALL);
        setBrushLocation(1000.0f, -6000.0f, 3000.0f); 
        setBrushSize(500.0f, 1000.0f, 9000.0f);   
        setBrushMaterial("Ascent/SeamlessConcrete2");

    createBrush("lowerAreaExtra", BRUSH_WALL);
        setBrushLocation(980.0f, -5500.0f, 3000.0f); 
        setBrushSize(20.0f, 100.0f, 9000.0f);   
        setBrushMaterial("Ascent/Caution");

    createBrush("lowerAreaLeft", BRUSH_WALL);
        setBrushLocation(-1500.0f, -6000.0f, 3000.0f); 
        setBrushSize(500.0f, 1000.0f, 9000.0f);   
        setBrushMaterial("Ascent/SeamlessConcrete2");

   createBrush("lowerAreaTop", BRUSH_CEIL);
        setBrushLocation(-1500.0f, -5000.0f, 9000.0f); 
        setBrushSize(3000.0f, 500.0f, 3000.0f);   
        setBrushMaterial("Ascent/SeamlessMetal");

    // extras
    buildColumn("pilar0", "Ascent/SeamlessConcrete", -1800.0f, 0.0f, 1000.0f, 400.0f, 2000.0f, 400.0f);
    buildColumn("pilar1", "Ascent/SeamlessConcrete", 1600.0f, 0.0f, 1000.0f, 400.0f, 2000.0f, 400.0f);

    buildLaunchPad("startArea", -400.0f, 0.0f, -400.0f);
    buildLaunchPad("endArea", -400.0f, -6000.0f, 9000.0f, true);

    // add some lights
    createLight("orangeLight");
        setLightLocation(0.0f, 1000.0f, 0.0f);
        setLightColour(0.8f, 0.4f, 0.0f);

    createLight("greenLight");
        setLightLocation(0.0f, -5500.0f, 7500.0f);
        setLightColour(0.0f, 0.7f, 0.0f);

    // add some enemies
    createEntity("Gerald");
        setEntityLocation(-1700.0f, 0.0f, 7000.0f);
        setEntityType(ENT_ENEMY_PATROL);

    createEntity("Marvin");
        setEntityLocation(1700.0f, 0.0f, 7000.0f);
        setEntityType(ENT_ENEMY_PATROL);

    createEntity("Matt");
        setEntityLocation(500.0f, -6000.0f, 7800.0f);
        setEntityType(ENT_ENEMY_STATIONARY);
    
    createEntity("Cheryl");
        setEntityLocation(-250.0f, -5795.0f, 9500.0f);
        setEntityType(ENT_ENEMY_PATROL);    
}

