//
//  Tile.h
//  martin
//
//  Created by Pontus Ekholm on 2017-10-05.
//

#ifndef Tile_h
#define Tile_h
#include "Region.h"

class Tile {
public:
    bool Buildable(){return m_tileInfo.buildable;}
    
    int GroundHeight(){return m_tileInfo.groundHeight;}
    
    bool Doodad(){return m_tileInfo.doodad;}
    
    void setBuildable(bool buildable){m_tileInfo.buildable = buildable;}
    
    void setDistNearestUnpathable(size_t dist){m_distNearestUnpathable = dist;}
    
private:
    struct TileInfo {
        TileInfo() : buildable(false), groundHeight(0), doodad(false) {}
        bool            buildable:true;
        size_t          groundHeight:2;
        bool            doodad:true;
    };
    
    size_t m_areaId;
    size_t m_distNearestUnpathable;
    TileInfo m_tileInfo;
};

#endif /* Tile_h */
