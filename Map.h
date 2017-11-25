#ifndef Map_h
#define Map_h
#include "ChokePoint.h"
#include <memory>





class Map {
public:
    Map(){}
    Map(sc2::Agent* bot){
        m_bot = bot;
        m_width  = m_bot->Observation()->GetGameInfo().width;
        m_height = m_bot->Observation()->GetGameInfo().height;
    }
    
    struct GreaterTile {
        bool operator()(std::shared_ptr<TilePosition> &a, std::shared_ptr<TilePosition> &b) const { return a->second->getDistNearestUnpathable() > b->second->getDistNearestUnpathable(); }
    };
    
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
    
    //Pushes tile into the k-d tree
    void addTile(sc2::Point2D pos, std::shared_ptr<Tile> tile){
        m_tilePositions.insert(std::make_pair(pos, tile));
    }
    
    size_t size() { return m_tilePositions.size(); }
    
    TilePositionContainer getTilePositions() { return m_tilePositions; }
    
    void setBot(sc2::Agent* bot){m_bot = bot;
        m_width  = m_bot->Observation()->GetGameInfo().width;
        m_height = m_bot->Observation()->GetGameInfo().height;}
    
protected:
    sc2::Agent* m_bot;
    static std::unique_ptr<Map> m_gInstance;
    UnitPositionContainer m_unitPositions;
    TilePositionContainer m_tilePositions;
    std::vector<std::shared_ptr<TilePosition>> m_buildableTiles;
    
    sc2::Point2D m_maxPlayable;
    sc2::Point2D m_minPlayable;
    sc2::Point2D m_centerPlayable;
    
    size_t m_width;
    size_t m_height;
};

#endif /* Map_h */
