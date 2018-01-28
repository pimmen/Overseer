#ifndef Map_h
#define Map_h

#include "ChokePoint.h"
#include "Graph.h"
#include "spatial/box_multimap.hpp"
#include "spatial/neighbor_iterator.hpp"
#include "spatial/ordered_iterator.hpp"

#include <memory>


namespace Overseer{

    /**
    * \struct point2d_accessor Region.h "Region.h"
    *
    * \brief Fix indexing in spatial.
    */
    struct point2d_accessor {
        int operator() (spatial::dimension_type dim, const sc2::Point2D p) const {
            switch(dim) {
                case 0: return p.x;
                case 1: return p.y;
                default: throw std::out_of_range("dim");
            }
        }
    };

    typedef spatial::box_multimap<2, sc2::Point2D, std::shared_ptr<Tile>, spatial::accessor_less<point2d_accessor, sc2::Point2D>> TilePositionContainer;
    typedef spatial::box_multimap<2, sc2::Point2D, sc2::Unit*, spatial::accessor_less<point2d_accessor, sc2::Point2D>> UnitPositionContainer;
    typedef std::map<size_t,std::shared_ptr<Region>> RegionMap;
    typedef std::map<std::pair<size_t,size_t>, std::vector<TilePosition>> RawFrontier;

    class Graph;

    /**
    * \class Map Map.h "Map.h"
    * \brief is the overseer map that holds the most "important" functionality for a outside user.
    */
    class Map {
        public:

            Map(){}
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
            * \Brief Gets the map height.
            *
            * \return the height.
            */
            size_t getHeight() const { return m_height; }
            
            /**
            * \brief Gets the map width.
            *
            * \return the width.
            */
            size_t getWidth() const { return m_width; }
            
            /**
            * \brief Gets all the regions found.
            *
            * \return vector of region pointers.
            */
            std::vector<std::shared_ptr<Region>> getRegions() {
                std::vector<std::shared_ptr<Region>> regions;
                for(RegionMap::iterator it = m_regions.begin(); it != m_regions.end(); it++) {
                    regions.push_back(it->second);
                }
                
                return regions;
            }
            
            /**
            * \brief Get a specific region.
            *
            * \param id The id of the specific region.
            * \return 
            */
            Region *getRegion(size_t id){
                return (m_regions.find(id) != m_regions.end()) ? m_regions[id].get() : nullptr;
            }
            
            /**
            * \brief Gets closest region.
            *
            * \param pos Is the position point to find region from.
            * \return pointer to the found region. 
            */
            const Region *getNearestRegion(sc2::Point2D pos){
                Region *region = nullptr;
                
                for(spatial::neighbor_iterator<TilePositionContainer> iter = neighbor_begin(m_tilePositions, pos); iter != neighbor_end(m_tilePositions, pos); iter++) {
                    if(iter->second->getRegionId()){
                        return getRegion(iter->second->getRegionId());
                    }
                }
                
                return region;
            }
            
            /**
            * \brief Apends a tile to container.
            *
            * \param pos The tile position.
            * \param tile The tile to add.
            */
            void addTile(sc2::Point2D& pos, std::shared_ptr<Tile> tile){
                m_tilePositions.insert(std::make_pair(pos, tile));
            }
            
            /**
            * \brief Check if a position is on map
            *
            * \return True if position is within the map, false otherwise.
            */
            bool Valid(sc2::Point2D pos) const {
                return ((0 <= pos.x) && (pos.x <= m_width) && (0 <= pos.y) && (pos.y <= m_height));
            }
            
            /**
            * \brief Get the closest tile position.
            *
            * \param pos of the tile position to find.
            * \return the found tile position.
            */
            TilePosition getClosestTilePosition(sc2::Point2D pos) {
                spatial::neighbor_iterator<TilePositionContainer> iter = neighbor_begin(m_tilePositions, pos);
                iter++;
                return *iter;
            }
            
            /**
            * \brief Apends a region to the container.
            *
            * \param region The Region to apend.
            */
            void addRegion(Region region) { m_regions[region.getId()] = std::make_shared<Region>(region); }
            
            /**
            * \brief Gets a tile based on the position.
            *
            * \param pos The position of the tile.
            * \return the found tile.
            */
            std::shared_ptr<Tile> GetTile(sc2::Point2D pos) { return m_tilePositions.find(pos)->second; }
            
            /**
            * \brief Gets the size of the tile position container.
            *
            * \return the size of container.
            */
            size_t size() { return m_tilePositions.size(); }
            
            /**
            * \brief Get all tile positions
            * 
            * \return the tile position container.
            */
            TilePositionContainer getTilePositions() { return m_tilePositions; }
            
            /**
            * \brief set the bot into overseer
            *
            * \param bot The bot witch overseer will use.
            */
            void setBot(sc2::Agent* bot){
                m_bot = bot;
                m_width  = m_bot->Observation()->GetGameInfo().width;
                m_height = m_bot->Observation()->GetGameInfo().height;
            }
            
            /**
            * \brief Get tiles that is between two regions.
            *
            * \return vector with tile position pointers.
            */
            std::vector<std::shared_ptr<TilePosition>> getFrontierPositions(){ return m_frontierPositions; }
            
            /**
            * \brief region pair and frontier map.
            *
            * \return vector of rawfrontier.
            */
            RawFrontier getRawFrontier() { return m_rawFrontier; }
            
        protected:
            
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
