#ifndef ChokePoint_h
#define ChokePoint_h
#include "Region.h"
#include <utility>

class Graph;

class ChokePoint {
public:
    typedef std::vector<const ChokePoint *> Path;
    
    //Returns the units occupying the choke point
    const std::vector<UnitPosition> getNeutralUnitPositions(){return m_neutralUnitPositions;}
    
    //Returns the regions this choke point connects
    const std::pair<const Region *, const Region *> & getRegions() const {return m_regions;}
    
    ChokePoint(Graph *graph, const Region * region1, const Region * region2, std::vector<TilePosition> tilePositions){
        p_graph = graph;
        m_regions.first = region1;
        m_regions.second = region2;
        m_tilePositions = tilePositions;
        m_center = tilePositions.front();
    }
    
    const size_t Size() const { return m_tilePositions.size(); }
    
    const sc2::Point2D getMiddlePoint() {
        std::vector<TilePosition>::iterator midTilePosition = std::max_element(m_tilePositions.begin(), m_tilePositions.end(),
                                                          [](TilePosition a, TilePosition b){ return a.second->getDistNearestUnpathable() < b.second->getDistNearestUnpathable(); });
        return midTilePosition->first;
    }
    
    Graph * GetGraph(){return p_graph;}
private:
    std::pair<const Region *, const Region *> m_regions;
    std::vector<UnitPosition> m_neutralUnitPositions;
    std::vector<TilePosition> m_tilePositions;
    TilePosition m_center;
    
    Graph *p_graph;
};


typedef ChokePoint::Path CPPath;

#endif /* ChokePoint_h */
