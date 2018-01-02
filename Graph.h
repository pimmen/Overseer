#ifndef Graph_h
#define Graph_h
#include "Map.h"

class Graph {
private:
    Map *p_map;
    std::vector<ChokePoint> m_chokePoints;
public:
    Graph(){}
    Graph(Map *map):p_map(map){}
    
    std::vector<ChokePoint> getChokePoints() { return m_chokePoints; }
    
    void setMap(Map *map){p_map = map;}
    
    void CreateChokePoints() {
        for(auto const & frontierByRegionPair : p_map->getRawFrontier()) {
            size_t regionIdA = frontierByRegionPair.first.first;
            size_t regionIdB = frontierByRegionPair.first.second;
            std::vector<TilePosition> frontierPositions = frontierByRegionPair.second;
            
            m_chokePoints.emplace_back(this, p_map->getRegion(regionIdA), p_map->getRegion(regionIdB),frontierPositions);
        }
    }
};

#endif /* Graph_h */
