#ifndef OverseerMap_h
#define OverseerMap_h
#include "Region.h"
#include "ChokePoint.h"
#include <memory>

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
    
    //Returns the safest path in choke-points from start to end
    const CPPath & getSafestPath(sc2::Point2D start, sc2::Point2D end);
private:
    static std::unique_ptr<Map> m_gInstance;
};

#endif /* OverseerMap_h */
