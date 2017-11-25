#ifndef Region_h
#define Region_h
#include <vector>
#include <queue>
#include <map>
#include <iostream>
#include "spatial/box_multimap.hpp"
#include "spatial/neighbor_iterator.hpp"
#include "spatial/ordered_iterator.hpp"
#include "sc2api/sc2_api.h"



class Tile;
class RegionEdge;

using namespace spatial;

//Used to access arbitrary points in the region
struct point2d_accessor {
    int operator() (dimension_type dim, const sc2::Point2D p) const {
        switch(dim) {
            case 0: return p.x;
            case 1: return p.y;
            default: throw std::out_of_range("dim");
        }
    }
};

typedef std::pair<sc2::Point2D, sc2::Unit *> UnitPosition;
typedef std::pair<sc2::Point2D, std::shared_ptr<Tile>> TilePosition;
typedef box_multimap<2, sc2::Point2D, std::shared_ptr<Tile>, accessor_less<point2d_accessor, sc2::Point2D>> TilePositionContainer;
typedef box_multimap<2, sc2::Point2D, sc2::Unit*, accessor_less<point2d_accessor, sc2::Point2D>> UnitPositionContainer;

class Region {
public:
    //Gets the number of points within the region, which corresponds to it's area
    int getArea() {return m_points.size();}
    
    //Returns the edges of the region
    std::vector<RegionEdge> getEdges(){return m_edges;}
    
    //Returns the units and positions occupying the region
    const std::vector<UnitPosition> getNeutralUnitPositions(){return m_neutralUnitPositions;}
    
    size_t getId(){return m_id;}
    
private:
    std::vector<sc2::Point2D> m_points;
    std::vector<RegionEdge> m_edges;
    std::vector<UnitPosition> m_neutralUnitPositions;
    
    size_t m_id;
};

enum EdgeType {
    wall,
    drop,
    impassible
};

class RegionEdge {
public:
    //Returns the regions this edge separates
    const std::pair<const Region *, const Region *> & getRegions(){return m_regions;}
    const std::vector<sc2::Point2D> getPoints(){return m_points;}
    EdgeType getEdgeType(){return m_edgeType;}
private:
    std::pair<const Region *, const Region *> m_regions;
    std::vector<sc2::Point2D> m_points;
    EdgeType m_edgeType;
};

#endif /* Region_h */
