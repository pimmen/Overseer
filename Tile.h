//
//  Tile.h
//  martin
//
//  Created by Pontus Ekholm on 2017-10-05.
//

#ifndef Tile_h
#define Tile_h

class Tile {
public:
    bool Buildable(){return m_tileInfo.buildable;}
    
    int GroundHeight(){return m_tileInfo.groundHeight;}
    
    bool Doodad(){return m_tileInfo.doodad;}
    
    void setBuildable(bool buildable){m_tileInfo.buildable = buildable;}
    
    void setDistNearestUnpathable(float dist){m_distNearestUnpathable = dist;}
    
    float getDistNearestUnpathable() const {return m_distNearestUnpathable;}
    
    void setRegionId(size_t regionId) {
        m_regionId = regionId;
    }
    
    size_t getRegionId(){return m_regionId;}
    
private:
    struct TileInfo {
        TileInfo() : buildable(false), groundHeight(0), doodad(false) {}
        bool            buildable:true;
        size_t          groundHeight:2;
        bool            doodad:true;
    };
    
    size_t m_regionId;
    float m_distNearestUnpathable;
    TileInfo m_tileInfo;
};

#endif /* Tile_h */
