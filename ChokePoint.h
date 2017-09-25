#ifndef ChokePoint_h
#define ChokePoint_h
#include "Region.h"
#include <utility>

class ChokePoint {
public:
    typedef std::vector<const ChokePoint *> Path;
    
    //Returns the units occupying the choke point, returns null_ptr if it's not obstructed
    const std::vector<sc2::Unit *> getUnits();
    
    //Returns the regions this choke point connects
    const std::pair<const Region *, const Region *> & getRegions(){return m_regions;}
    
    ChokePoint(const Region * region1, const Region * region2, sc2::Point2D center){m_regions.first = region1; m_regions.second = region2; m_center = center;}
private:
    std::pair<const Region *, const Region *> m_regions;
    sc2::Point2D m_center;
};


typedef ChokePoint::Path CPPath;

#endif /* ChokePoint_h */
