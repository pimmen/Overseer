#ifndef _OVERSEER_MAP_H_
#define _OVERSEER_MAP_H_

#include "ChokePoint.h"
#include "Graph.h"
#include "Region.h"

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
        int operator() (spatial::dimension_type dim, const sc2::Point2D p) const;
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

            Map();
            /**
            * \brief constructor.
            *
            * \param bot The Starcraft II bot.
            */
            Map(sc2::Agent* bot);
            
            /**
            * \Brief Gets the map height.
            *
            * \return the height.
            */
            size_t getHeight() const;
            
            /**
            * \brief Gets the map width.
            *
            * \return the width.
            */
            size_t getWidth() const;
            
            /**
            * \brief Gets all the regions found.
            *
            * \return vector of region pointers.
            */
            std::vector<std::shared_ptr<Region>> getRegions();
            
            /**
            * \brief Get a specific region.
            *
            * \param id The id of the specific region.
            * \return 
            */
            Region* getRegion(size_t id);
            
            /**
            * \brief Gets closest region.
            *
            * \param pos Is the position point to find region from.
            * \return pointer to the found region. 
            */
            const Region* getNearestRegion(sc2::Point2D pos);
            
            /**
            * \brief Apends a tile to container.
            *
            * \param pos The tile position.
            * \param tile The tile to add.
            */
            void addTile(sc2::Point2D& pos, std::shared_ptr<Tile> tile);
            
            /**
            * \brief Check if a position is on map
            *
            * \return True if position is within the map, false otherwise.
            */
            bool Valid(sc2::Point2D pos) const;
            
            /**
            * \brief Get the closest tile position.
            *
            * \param pos of the tile position to find.
            * \return the found tile position.
            */
            TilePosition getClosestTilePosition(sc2::Point2D pos);
            
            /**
            * \brief Apends a region to the container.
            *
            * \param region The Region to apend.
            */
            void addRegion(Region region);
            
            /**
            * \brief Gets a tile based on the position.
            *
            * \param pos The position of the tile.
            * \return the found tile.
            */
            std::shared_ptr<Tile> GetTile(sc2::Point2D pos);
            
            /**
            * \brief Gets the size of the tile position container.
            *
            * \return the size of container.
            */
            size_t size();
            
            /**
            * \brief Get all tile positions
            * 
            * \return the tile position container.
            */
            TilePositionContainer getTilePositions();          
            /**
            * \brief set the bot into overseer
            *
            * \param bot The bot witch overseer will use.
            */
            void setBot(sc2::Agent* bot);
            
            /**
            * \brief Get tiles that is between two regions.
            *
            * \return vector with tile position pointers.
            */
            std::vector<std::shared_ptr<TilePosition>> getFrontierPositions();
            
            /**
            * \brief region pair and frontier map.
            *
            * \return vector of rawfrontier.
            */
            RawFrontier getRawFrontier();
            
        protected:
            
            std::pair<size_t, size_t> findNeighboringRegions(std::shared_ptr<TilePosition> tilePosition);

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

#endif /* _OVERSEER_MAP_H_ */
