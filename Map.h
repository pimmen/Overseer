#ifndef Map_h
#define Map_h
#include "ChokePoint.h"
#include "spatial/box_multimap.hpp"
#include <memory>

//Used to access arbitrary points in the region
struct point2d_accessor {
    int operator() (spatial::dimension_type dim, const sc2::Point2D p) const {
        switch(dim) {
            case 0: return p.x;
            case 1: return p.y;
            default: throw std::out_of_range("dim");
        }
    }
};

typedef spatial::box_multimap<2, sc2::Point2D, Tile, spatial::accessor_less<point2d_accessor, sc2::Point2D>> TileContainer;
typedef spatial::box_multimap<2, sc2::Point2D, sc2::Unit*, spatial::accessor_less<point2d_accessor, sc2::Point2D>> UnitPositionContainer;

class Map {
public:
    //Gets the map instance running right now
    const Map & getInstance();
    
    //Returns all regions on map
    const std::vector<Region> & getRegions();
    
    //Returns region with the id
    const Region * getRegion(int id);
    
    //Returns the region containing the point if the point is pathable
    //Otherwise returns null_ptr
    const Region * getRegion(sc2::Point2D point);
    
    //Returns the region closest to the point
    //Returns null_ptr if getRegions.empty()
    const Region * getNearestRegion(sc2::Point2D point);
    
    //Returns the shortest path in choke-points from start to end
    const CPPath & getShortestPath(sc2::Point2D start, sc2::Point2D end);
    
    void addTile(sc2::Point2D pos, Tile& tile){
        //std::cout << "Adding tile at " << pos.x << ", " << pos.y << std::endl;
        m_tilePositions.insert(std::make_pair(pos, tile));
        //m_tilePositions.insert(pos);
    }
    
    size_t size(){return m_tilePositions.size();}
    
protected:
    static std::unique_ptr<Map> m_gInstance;
    UnitPositionContainer m_unitPositions;
    TileContainer m_tilePositions;
    
    sc2::Point2D m_maxPlayable;
    sc2::Point2D m_minPlayable;
    sc2::Point2D m_centerPlayable;
    
    size_t m_width;
    size_t m_height;
};

#endif /* Map_h */

