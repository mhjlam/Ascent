#ifndef _MAP_H_
#define _MAP_H_

#include "../stdafx.h"
#include "../Constants.h"

/*
 *  MapInfo contains all the details related to the map itself, such as fog and ambient light
 */
struct MapInfo
{
    Ogre::String name;
    Ogre::ColourValue ambientLight;
    Ogre::ColourValue fogColour;
    Ogre::Real fogStart;
    Ogre::Real fogEnd;
    bool fogEnabled;
};


/*
 *  EntityInfo forms the base for any entities that will be placed in the map
 */
struct EntityInfo
{
    Ogre::String id;    
    Ogre::Vector3 position;
    Ogre::Vector3 direction; // facing direction
    EntityType type;
    // other common things
};


/*
 *  Self-explanitory, though 'direction' is not used since the default light is a PointLight
 */
struct LightInfo : EntityInfo
{
    Ogre::ColourValue colour;
};


/*
 *  BrushInfo contains info about the solid it will be become
 */
struct BrushInfo : EntityInfo
{
    Ogre::String materialName;
    Ogre::Vector3 dimensions;
    BrushType brushType; // e.g. wall/ceiling/floor
    bool castShadows;
};


// used to determine which sides of a cube brush to leave out
enum CubePassThrough 
{  
    CUBE_NONE,      // leave no sides out (default)
    CUBE_X_AXIS,    // leave the left and right walls
    CUBE_Y_AXIS,    // leave the ceiling and floor
    CUBE_Z_AXIS     // leave the front and back walls
};

/*
 *  Please note that this class was built mainly for clarity/intuitivity and readability, hence
 *  the structure is not optimal from an OO-perspective. There are some private functions that help prevent
 *  some duplicate code, but that's about it. The main purpose is readability and intuitivity for creating the maps.
 */
   
class Map
{
private:
    // map and player information is stored in separate members
    MapInfo mMapInfo;
    
    // entities the map will contain
    std::vector<LightInfo*> mLights;
    std::vector<BrushInfo*> mGeometry;
    std::vector<EntityInfo*> mEntities;
    
    // the materials set are used in buildCubeBrush
    Ogre::String mFloorMaterial;
    Ogre::String mCeilMaterial;
    Ogre::String mWallMaterial;
    Ogre::Real mCubeThickness;
    
    // which entity we are currently setting up
    BrushInfo* mActiveBrush;
    LightInfo* mActiveLight;
    EntityInfo* mActiveEntity;
    
    // shortcuts to prevent the same 3 lines of code whenever a colour or vector member is to be set
    void setColourMember(Ogre::ColourValue& member, Ogre::Real r, Ogre::Real g, Ogre::Real b);
    void setVectorMember(Ogre::Vector3& member, Ogre::Real x, Ogre::Real y, Ogre::Real z);

protected:
    /*
     *  These functions are used to build the map in inheritted classes. 
     *  By making these public (or befriend this class) one could use a script or parse the map from an external file.
     *  Note that they are not fool-proof, though crashes should only occur when calling a 
     *  setXXX function when no createXXX function has been called earlier (at all).
     */

    void setMapName(const Ogre::String& mapName);
    void setMapAmbientLight(Ogre::Real r, Ogre::Real g, Ogre::Real b);
    void setMapFogEnabled(bool enableFog);
    void setMapFogColour(Ogre::Real r, Ogre::Real g, Ogre::Real b);
    void setMapFogRange(Ogre::Real start, Ogre::Real end);
    
    // brushes
    void createBrush(const Ogre::String& name, BrushType type);
    void setBrushLocation(Ogre::Real x, Ogre::Real y, Ogre::Real z);
    void setBrushSize(Ogre::Real width, Ogre::Real height, Ogre::Real depth);
    void setBrushShadowsEnabled(bool enableShadowCasting);
    void setBrushMaterial(const Ogre::String& materialName);
    
    // 'prefabs'
    void buildCubeBrush(const Ogre::String& cubeName, Ogre::Real x, Ogre::Real y, Ogre::Real z, Ogre::Real width, Ogre::Real height, Ogre::Real depth, CubePassThrough passThrough = CUBE_NONE);
    void buildColumn(const Ogre::String& name, const Ogre::String& materialName, Ogre::Real x, Ogre::Real y, Ogre::Real z, Ogre::Real width, Ogre::Real height, Ogre::Real depth);
    void buildLaunchPad(const Ogre::String& name, Ogre::Real x, Ogre::Real y, Ogre::Real z, bool isExit = false);
    void setCubeBrushThickness(Ogre::Real thickness);
    void setFloorMaterial(const Ogre::String& materialName); 
    void setCeilMaterial(const Ogre::String& materialName); 
    void setWallMaterial(const Ogre::String& materialName);  
    
    // lights
    void createLight(const Ogre::String& name);
    void setLightLocation(Ogre::Real x, Ogre::Real y, Ogre::Real z);
    void setLightColour(Ogre::Real r, Ogre::Real g, Ogre::Real b);
    
    // enemies
    void createEntity(const Ogre::String& name);    
    void setEntityLocation(Ogre::Real x, Ogre::Real y, Ogre::Real z);
    void setEntityFacingDirection(Ogre::Real x, Ogre::Real y, Ogre::Real z);
    void setEntityType(EntityType type);

public:
    Map();
    virtual ~Map();

    size_t numLights() const        { return mLights.size();    } 
    size_t numBrushes() const       { return mGeometry.size();  }
    size_t numEntities() const      { return mEntities.size();   }

    const std::vector<LightInfo*>& getLights()      { return mLights;       }
    const std::vector<BrushInfo*>& getBrushes()     { return mGeometry;     }
    const std::vector<EntityInfo*>& getEntities()   { return mEntities;      }
    const MapInfo& getMapInfo()                     { return mMapInfo;      }
    
};

#endif