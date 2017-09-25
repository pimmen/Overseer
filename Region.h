#ifndef Region_h
#define Region_h
#include <vector>
#include "sc2api/sc2_api.h"
#include "Neutral.h"
#include "spatial/box_multiset.hpp"
#include "spatial/box_multimap.hpp"

//Used to keep track of units in the region
struct UnitPosition {
    sc2::Point2D position;
    sc2::Unit *unit;
};

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


//Used to access UnitPositions in the region
struct unitPosition_accessor {
    int operator() (spatial::dimension_type dim, const UnitPosition up) const {
        switch(dim) {
            case 0: return up.position.x;
            case 1: return up.position.y;
            default: throw std::out_of_range("dim");
        }
    }
};

class Region {
public:
    //Gets the number of points within the region, which corresponds to it's area
    int getArea() {return m_points.size();}
    
    //Inserts unit to the region
    void insertUnit(sc2::Unit *unit);
    
    //Returns true if region contains the unit
    bool contains(sc2::Unit *unit);
    
    //Get nearest unit to the point
    sc2::Unit * getNearestUnit(sc2::Point2D point);
    
private:
    spatial::box_multiset<2, sc2::Point2D, spatial::accessor_less<point2d_accessor, sc2::Point2D>> m_points;
    spatial::box_multimap<2, UnitPosition, spatial::accessor_less<unitPosition_accessor, UnitPosition>> m_unitPositions;
    int m_id;
};
#endif /* Region_h */
