#include "stdafx.h"
#include "Map.h"

Map::Map() : mActiveBrush(NULL), mActiveLight(NULL), mActiveEntity(NULL)
{}

Map::~Map()
{
    // fastest way of deleting pointers in three different vectors
    size_t max = std::max(std::max(mGeometry.size(), mLights.size()), mEntities.size()); // determine largest vector
 
    for (size_t i = 0; i < max; ++i)
    {
        if (i < mGeometry.size())
            delete mGeometry.at(i);

        if (i < mLights.size())
            delete mLights.at(i);

        if (i < mEntities.size())
            delete mEntities.at(i);
    }

    mGeometry.clear();
    mLights.clear();
    mEntities.clear();
}

// helpers to prevent duplicate code (a little)
void Map::setColourMember(Ogre::ColourValue& member, Ogre::Real r, Ogre::Real g, Ogre::Real b)
{
    member.r = r;
    member.g = g;
    member.b = b;
    member.a = 1.0f;
}
 
void Map::setVectorMember(Ogre::Vector3& member, Ogre::Real x, Ogre::Real y, Ogre::Real z)
{
    member.x = x;
    member.y = y;
    member.z = z;
}


// map related settings
void Map::setMapName(const Ogre::String& mapName)
{
    mMapInfo.name = mapName;
}

void Map::setMapAmbientLight(Ogre::Real r, Ogre::Real g, Ogre::Real b)
{
    setColourMember(mMapInfo.ambientLight, r, g, b);
}

void Map::setMapFogEnabled(bool enableFog)
{
    mMapInfo.fogEnabled = enableFog;
}

void Map::setMapFogColour(Ogre::Real r, Ogre::Real g, Ogre::Real b)
{
    setColourMember(mMapInfo.fogColour, r, g, b);
}

void Map::setMapFogRange(Ogre::Real start, Ogre::Real end)
{
    mMapInfo.fogStart = start;
    mMapInfo.fogEnd = end;
}


// brushes
void Map::createBrush(const Ogre::String& name, BrushType type)
{
    mActiveBrush = new BrushInfo();
    mActiveBrush->id = name;
    mActiveBrush->type = ENT_BRUSH;
    mActiveBrush->brushType = type;
    mActiveBrush->castShadows = true; //otherwise light will go right through it, but it can be disabled if neccesary
    mGeometry.push_back(mActiveBrush);    
}

void Map::setBrushLocation(Ogre::Real x, Ogre::Real y, Ogre::Real z)
{
    setVectorMember(mActiveBrush->position, x, y, z);
}

void Map::setBrushSize(Ogre::Real width, Ogre::Real height, Ogre::Real depth)
{
    setVectorMember(mActiveBrush->dimensions, width, height, depth);
}

void Map::setBrushShadowsEnabled(bool enableShadowCasting)
{
    mActiveBrush->castShadows = enableShadowCasting;
}

void Map::setBrushMaterial(const Ogre::String& materialName)
{
    mActiveBrush->materialName = materialName;
}

void Map::buildCubeBrush(const Ogre::String& cubeName, Ogre::Real x, Ogre::Real y, Ogre::Real z, Ogre::Real width, Ogre::Real height, Ogre::Real depth,  CubePassThrough passThrough)
{
    const Ogre::Real thickness = mCubeThickness;
    Ogre::Real wallHeight = height  - (2* thickness);

    if (passThrough == CUBE_Y_AXIS)
    {
        wallHeight = height;
        y -= thickness;
        height += thickness;
    }

    if (passThrough != CUBE_Y_AXIS)
    {
        createBrush(cubeName + "_floor", BRUSH_FLOOR);
            setBrushLocation(x, y, z);
            setBrushSize(width, thickness, depth);
            setBrushMaterial(mFloorMaterial);
        createBrush(cubeName + "_ceil", BRUSH_CEIL);
            setBrushLocation(x, y + height - thickness, z);
            setBrushSize(width, thickness, depth);
            setBrushMaterial(mCeilMaterial);
    }

    if (passThrough != CUBE_X_AXIS)
    {
        createBrush(cubeName + "_leftwall", BRUSH_WALL);
            setBrushLocation(x, y + thickness, z);
            setBrushSize(thickness, wallHeight, depth);
            setBrushMaterial(mWallMaterial);
        createBrush(cubeName + "_rightwall", BRUSH_WALL);
            setBrushLocation(x + width - thickness, y + thickness, z);
            setBrushSize(thickness, wallHeight, depth);
            setBrushMaterial(mWallMaterial);
    }

    if (passThrough != CUBE_Z_AXIS)
    {
        /*
         *  The walls are not all of equal width. Left and Right walls are wider and the Front and Back walls fill up the 'gap'.
         *  However, because the Left and Right walls may might be omitted (if passThrough == CUBE_X_AXIS),
         *  the Front and Back walls must be stretched a little to make up for lost space.
         */

        createBrush(cubeName + "_backwall", BRUSH_WALL);
            if (passThrough == CUBE_X_AXIS)
            {
                setBrushLocation(x, y + thickness, z);
                setBrushSize(width, wallHeight, thickness); // compensate width
            }
            else
            {
                setBrushLocation(x + thickness, y + thickness, z);
                setBrushSize(width - (2* thickness), wallHeight, thickness);
            }
                        
            setBrushMaterial(mWallMaterial);

        createBrush(cubeName + "_frontwall", BRUSH_WALL);
            if (passThrough == CUBE_X_AXIS)
            {
                setBrushLocation(x, y + thickness, z + depth - thickness);
                setBrushSize(width, wallHeight, thickness);
            }
            else
            {
                setBrushLocation(x + thickness, y + thickness, z + depth - thickness);
                setBrushSize(width - (2* thickness), wallHeight, thickness);
            }
            
            setBrushMaterial(mWallMaterial);
    }
}

void Map::buildColumn(const Ogre::String& name, const Ogre::String& materialName, Ogre::Real x, Ogre::Real y, Ogre::Real z, Ogre::Real width, Ogre::Real height, Ogre::Real depth)
{
    Ogre::Real baseHeight = 200.0f;

    createBrush(name + "_pilar_base", BRUSH_WALL);
        setBrushLocation(x, y, z); 
        setBrushSize(width, baseHeight, depth);   
        setBrushMaterial(materialName);

    createBrush(name + "_pilar_top", BRUSH_WALL);
        setBrushLocation(x, y + height - baseHeight, z); 
        setBrushSize(width, baseHeight, depth);   
        setBrushMaterial(materialName);

    createBrush(name, BRUSH_WALL);
        setBrushLocation(x + (width * 0.25f), y + baseHeight, z + (depth * 0.25f)); 
        setBrushSize(width * 0.5f, height - (2* baseHeight), depth * 0.5f);   
        setBrushMaterial(materialName);
}

void Map::buildLaunchPad(const Ogre::String& name, Ogre::Real x, Ogre::Real y, Ogre::Real z, bool isExit)
{
    // Note: dimensions and materials are hardcoded
    createBrush(name, (isExit? BRUSH_EXIT : BRUSH_START));
        setBrushLocation(x, y + 200.0f, z); 
        setBrushSize(800.0f, 5.0f, 800.0f);   
        setBrushMaterial("Ascent/Caution");

    createBrush(name + "_base", BRUSH_FLOOR);
        setBrushLocation(x, y, z); 
        setBrushSize(800.0f, 200.0f, 800.0f);   
        setBrushMaterial("Ascent/SeamlessConcrete2");
}

void Map::setCubeBrushThickness(Ogre::Real thickness)
{
    mCubeThickness = thickness;
}

void Map::setFloorMaterial(const Ogre::String& materialName)
{
    mFloorMaterial = materialName;
}

void Map::setCeilMaterial(const Ogre::String& materialName)
{
    mCeilMaterial = materialName;
}

void Map::setWallMaterial(const Ogre::String& materialName)
{
    mWallMaterial = materialName;
}

    
// lights
void Map::createLight(const Ogre::String& name)
{
    mActiveLight = new LightInfo();
    mActiveLight->id = name;
    mLights.push_back(mActiveLight);
}


void Map::setLightLocation(Ogre::Real x, Ogre::Real y, Ogre::Real z)
{
   setVectorMember(mActiveLight->position, x, y, z);
}

void Map::setLightColour(Ogre::Real r, Ogre::Real g, Ogre::Real b)
{
    setColourMember(mActiveLight->colour, r, g, b);
}


// enemies
void Map::createEntity(const Ogre::String& name)
{
    mActiveEntity = new EntityInfo();
    mActiveEntity->id = name;
    mEntities.push_back(mActiveEntity);
}

void Map::setEntityLocation(Ogre::Real x, Ogre::Real y, Ogre::Real z)
{
    setVectorMember(mActiveEntity->position, x, y, z);
}

void Map::setEntityFacingDirection(Ogre::Real x, Ogre::Real y, Ogre::Real z)
{
    setVectorMember(mActiveEntity->direction, x, y, z);
}

void Map::setEntityType(EntityType type)
{
    mActiveEntity->type = type;
}


