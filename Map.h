#ifndef Map_h
#define Map_h
#include "ChokePoint.h"
#include <memory>


namespace Overseer{

    typedef std::map<size_t,std::shared_ptr<Region>> RegionMap;
    typedef std::map<std::pair<size_t,size_t>, std::vector<TilePosition>> RawFrontier;

    class Graph;

    /**
    * \class Map Map.h "Map.h"
    * \brief is the overseer map that holds the most "important" functionality for a outside user.
    */
    class Map {
    public:
        /**
        * \brief constructor.
        *
        * \param bot The Starcraft II bot.
        */
        Map(sc2::Agent* bot){
            m_bot = bot;
            m_width  = m_bot->Observation()->GetGameInfo().width;
            m_height = m_bot->Observation()->GetGameInfo().height;
        }
        
        /**
        * 
        */
        size_t getHeight() const { return m_height; }
        
        /**
        *
        */
        size_t getWidth() const { return m_width; }
        
        /**
        *
        */
        std::vector<std::shared_ptr<Region>> getRegions() {
            std::vector<std::shared_ptr<Region>> regions;
            for(RegionMap::iterator it = m_regions.begin(); it != m_regions.end(); it++) {
                regions.push_back(it->second);
            }
            
            return regions;
        }
        
        /**
        *
        */
        Region *getRegion(size_t id) {
            return m_regions[id].get();
        }
        
        /**
        *
        */
        const Region *getNearestRegion(sc2::Point2D pos) {
            Region *region = 0;
            
            for(neighbor_iterator<TilePositionContainer> iter = neighbor_begin(m_tilePositions, pos); iter != neighbor_end(m_tilePositions, pos); iter++) {
                if(iter->second->getRegionId()){
                    return getRegion(iter->second->getRegionId());
                }
            }
            
            return region;
        }
        
        /**
        *
        */
        const CPPath & getShortestPath(sc2::Point2D start, sc2::Point2D end);
        
        /**
        *
        */
        void addTile(sc2::Point2D pos, std::shared_ptr<Tile> tile){
            m_tilePositions.insert(std::make_pair(pos, tile));
        }
        
        /**
        *
        */
        bool Valid(sc2::Point2D pos) const {
            return ((0 <= pos.x) && (pos.x <= m_width) && (0 <= pos.y) && (pos.y <= m_height));
        }
        
        /**
        *
        */
        TilePosition getClosestTilePosition(sc2::Point2D pos) {
            neighbor_iterator<TilePositionContainer> iter = neighbor_begin(m_tilePositions, pos);
            iter++;
            return *iter;
        }
        
        /**
        *
        */
        void addRegion(Region region) { m_regions[region.getId()] = std::make_shared<Region>(region); }
        
        /**
        *
        */
        std::shared_ptr<Tile> GetTile(sc2::Point2D pos) { return m_tilePositions.find(pos)->second; }
        
        /**
        *
        */
        size_t size() { return m_tilePositions.size(); }
        
        /**
        *
        */
        TilePositionContainer getTilePositions() { return m_tilePositions; }
        
        /**
        *
        */
        void setBot(sc2::Agent* bot){
            m_bot = bot;
            m_width  = m_bot->Observation()->GetGameInfo().width;
            m_height = m_bot->Observation()->GetGameInfo().height;
        }
        
        /**
        *
        */
        std::pair<size_t, size_t> findNeighboringRegions(std::shared_ptr<TilePosition> tilePosition) {
            std::pair<size_t, size_t> result(0,0);
            
            for(sc2::Point2D delta: {sc2::Point2D(0,-1), sc2::Point2D(0,1), sc2::Point2D(-1,0), sc2::Point2D(1,0)}) {
                if(Valid(tilePosition->first + delta)) {
                    std::shared_ptr<Tile> deltaTile = GetTile(tilePosition->first + delta);
                    if(deltaTile->Buildable()) {
                        size_t regionId = deltaTile->getRegionId();
                        
                        if(regionId) {
                            if(!result.first) {
                                result.first = regionId;
                            } else if(result.first != regionId) {
                                if(!result.second || regionId < result.second) {
                                    result.second = regionId;
                                }
                            }
                        }
                    }
                }
            }
            
            return result;
        }
        
        /**
        *
        */
        std::vector<std::shared_ptr<TilePosition>> getFrontierPositions(){ return m_frontierPositions; }
        
        /**
        *
        */
        RawFrontier getRawFrontier() { return m_rawFrontier; }
        
    protected:
        sc2::Agent* m_bot;
        static std::unique_ptr<Map> m_gInstance;
        
        UnitPositionContainer m_unitPositions;
        TilePositionContainer m_tilePositions;
        std::vector<std::shared_ptr<TilePosition>> m_buildableTiles;
        RegionMap m_regions;
        std::vector<std::shared_ptr<TilePosition>> m_frontierPositions;
        RawFrontier m_rawFrontier;
        
        sc2::Point2D m_maxPlayable;
        sc2::Point2D m_minPlayable;
        sc2::Point2D m_centerPlayable;
        
        size_t m_width;
        size_t m_height;
    };

}

#endif /* Map_h */
