#ifndef _OVERSEER_REGION_H_
#define _OVERSEER_REGION_H_

#include <vector>
#include <queue>
#include <map>
#include <iostream>
#include <algorithm>

#include "spatial/box_multimap.hpp"
#include "spatial/neighbor_iterator.hpp"
#include "spatial/ordered_iterator.hpp"
#include "sc2api/sc2_api.h"
#include "Tile.h"
#include "Region.h"

namespace Overseer{

    class RegionEdge;

    typedef std::pair<sc2::Point2D, std::shared_ptr<Tile>> TilePosition;
    typedef std::pair<sc2::Point2D, sc2::Unit *> UnitPosition;

    /**
    * \struct GreaterTile Region.h "Region.h"
    *
    * \brief sort on distance to nerearest unpathable.
    */
    struct GreaterTile {
        bool operator()(std::shared_ptr<TilePosition> &a, std::shared_ptr<TilePosition> &b) const;
    };

    /**
    * \struct GreaterTileInstance Region.h "Region.h"
    * \brief sort on distance to nerearest unpathable.
    */
    struct GreaterTileInstance {
        bool operator()(TilePosition &a, TilePosition &b) const;
    };


    /**
    * \enum EdgeType Region.h "Region.h"
    *
    * \brief Work in progress.
    */
    enum EdgeType {
        cliff,
        impassible
    };

    /**
    * \class Region Region.h "Region.h"
    *
    * \brief A region handler.
    */
    class Region {
        public:

            /**
            * \brief default constructor.
            */
            Region();
            
            /**
            * \brief constructor.
            *
            * \param regionId The id a region
            * \param tilePosition A tileposition within the region.
            */
            Region(size_t regionId, std::shared_ptr<TilePosition> tilePosition);
            
            /**
            * \brief Gets the number of points within the region, which corresponds to it's area
            *
            * \return The number of tile positiions within the region.
            */
            size_t getArea() const;
            
            /**
            * \brief Returns the edges of the region
            *
            * \return vector with edges.
            */
            std::vector<RegionEdge> getEdges();
            
            /**
            * \brief Returns the units and positions occupying the region
            *
            * \return const vector with unit positions.
            */
            const std::vector<UnitPosition> getNeutralUnitPositions();
            
            /**
            * \brief return region id.
            *
            * \return This region id
            */
            size_t getId() const;
            
            /**
            * \brief set the region id
            *
            * \param the id to set
            */
            void setId(size_t regionId);
            
            /**
            * \brief Get the tile positions within the region
            *
            * \return vector with tileposition.
            */
            std::vector<TilePosition> getTilePositions() const;
            
            /**
            * \brief Gets all mid points from all tiles within this region.
            *
            * \return vector the sc2::Point2D
            */
            std::vector<sc2::Point2D> getPoints();
            
            /**
            * \brief Add tile position to region.
            *
            * \param the tile position to add.
            */
            void AddTilePosition(std::shared_ptr<TilePosition> tilePosition);
            
            /**
            * \brief Add tile position to region.
            *
            * \param the tile position to add.
            */
            void AddTilePosition(TilePosition& tilePosition);
            
            /**
            * \brief get the longest path to a unpathable.
            *
            * \return the path.
            */
            double getLargestDistanceToUnpathable() const;
            
            /**
            * \brief Get the mid point of this region
            *
            * \return region mid point
            */
            sc2::Point2D getMidPoint() const;
            
            /**
            * \brief Merges two regions.
            *
            * \param region The region to merge with THIS region.
            */
            void Merge(Region region);
            
            /**
            * \brief clear this region of its tile positions.
            */
            void Clear();
            
        private:
            std::vector<TilePosition> m_tilePositions;
            std::vector<RegionEdge> m_edges;
            std::vector<UnitPosition> m_neutralUnitPositions;
            
            float m_largestDistUnpathable;
            sc2::Point2D m_midPoint;
            size_t m_id;
    };


    /**
    * \class RegionEdge Region.h "Region.h"
    * \brief Edge handliing for a region. --inprogress.
    */
    class RegionEdge {
        public:
            //Returns the regions this edge separates
            const std::pair<const Region *, const Region *> & getRegions(){return m_regions;}
            const std::vector<TilePosition> getPoints(){return m_points;}
            EdgeType getEdgeType(){return m_edgeType;}
        private:
            std::pair<const Region *, const Region *> m_regions;
            std::vector<TilePosition> m_points;
            EdgeType m_edgeType;
    };
}

#endif /* _OVERSEER_REGION_H_ */
