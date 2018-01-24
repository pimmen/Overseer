#ifndef ChokePoint_h
#define ChokePoint_h
#include "Region.h"
#include <utility>

class Graph;

class ChokePoint {
public:
    typedef std::vector<const ChokePoint *> Path;
    
    //Returns the units occupying the choke point
    std::vector<UnitPosition> getNeutralUnitPositions() const {return m_neutralUnitPositions;}
    
    //Returns the regions this choke point connects
    std::pair<const Region *, const Region *> & getRegions() {return m_regions;}
    
    ChokePoint(Graph *graph, const Region * region1, const Region * region2, std::vector<TilePosition> tilePositions){
        p_graph = graph;
        
        m_regions.first = region1;
        m_regions.second = region2;
        m_tilePositions = tilePositions;
        
        std::vector<TilePosition>::iterator midTilePosition = std::max_element(m_tilePositions.begin(), m_tilePositions.end(),
                                                                               [](TilePosition a, TilePosition b){ return a.second->getDistNearestUnpathable() < b.second->getDistNearestUnpathable(); });
        
        m_center = *midTilePosition;
        
    }
    
    size_t Size() const { return m_tilePositions.size(); }
    
    sc2::Point2D getMidPoint() const {
        return m_center.first;
    }
    
    std::vector<sc2::Point2D> getPoints() {
        std::vector<sc2::Point2D> points;
        
        for(auto const & tilePosition : m_tilePositions) {
            points.push_back(tilePosition.first);
        }
        
        return points;
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
