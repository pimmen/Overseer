#ifndef Region_h
#define Region_h
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

namespace Overseer{

    class RegionEdge;

    using namespace spatial;

    /**
    * \struct point2d_accessor Region.h "Region.h"
    *
    * \brief -.-.... encapsulation time.
    */
    //Used to access arbitrary points in the region
    struct point2d_accessor {
        int operator() (dimension_type dim, const sc2::Point2D p) const {
            switch(dim) {
                case 0: return p.x;
                case 1: return p.y;
                default: throw std::out_of_range("dim");
            }
        }
    };

    typedef std::pair<sc2::Point2D, sc2::Unit *> UnitPosition;
    typedef std::pair<sc2::Point2D, std::shared_ptr<Tile>> TilePosition;
    typedef box_multimap<2, sc2::Point2D, std::shared_ptr<Tile>, accessor_less<point2d_accessor, sc2::Point2D>> TilePositionContainer;
    typedef box_multimap<2, sc2::Point2D, sc2::Unit*, accessor_less<point2d_accessor, sc2::Point2D>> UnitPositionContainer;

    /**
    * \struct GreaterTile Region.h "Region.h"
    *
    * \brief sort on distance to nerearest unpathable.
    */
    struct GreaterTile {
        bool operator()(std::shared_ptr<TilePosition> &a, std::shared_ptr<TilePosition> &b) const { return a->second->getDistNearestUnpathable() > b->second->getDistNearestUnpathable(); }
    };

    /**
    * \struct GreaterTile Region.h "Region.h"
    *
    * \brief sort on distance to nerearest unpathable.
    */
    struct GreaterTileInstance {
        bool operator()(TilePosition &a, TilePosition &b) const { return a.second->getDistNearestUnpathable() > b.second->getDistNearestUnpathable(); }
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
            Region(){
                m_largestDistUnpathable = 0;
            }
            
            /**
            * \brief constructor.
            *
            * \param regionId The id a region
            * \param tilePosition A tileposition within the region.
            */
            Region(size_t regionId, std::shared_ptr<TilePosition> tilePosition){
                m_id = regionId;
                m_largestDistUnpathable = tilePosition->second->getDistNearestUnpathable();
                m_midPoint = tilePosition->first;
                
                AddTilePosition(tilePosition);
            }
            
            /**
            * \brief Gets the number of points within the region, which corresponds to it's area
            *
            * \return The number of tile positiions within the region.
            */
            size_t getArea() const {return m_tilePositions.size();}
            
            /**
            * \brief Returns the edges of the region
            *
            * \return vector with edges.
            */
            std::vector<RegionEdge> getEdges(){return m_edges;}
            
            /**
            * \brief Returns the units and positions occupying the region
            *
            * \return const vector with unit positions.
            */
            const std::vector<UnitPosition> getNeutralUnitPositions(){return m_neutralUnitPositions;}
            
            /**
            * \brief return region id.
            *
            * \return This region id
            */
            size_t getId() const {
                return m_id;
            }
            
            /**
            * \brief set the region id
            *
            * \param the id to set
            */
            void setId(size_t regionId) {
                m_id = regionId;
                for(auto & tilePosition : m_tilePositions) {
                    tilePosition.second->setRegionId(regionId);
                }
            }
            
            /**
            * \brief Get the tile positions within the region
            *
            * \return vector with tileposition.
            */
            std::vector<TilePosition> getTilePositions() const { return m_tilePositions; }
            
            std::vector<sc2::Point2D> getPoints() {
                std::vector<sc2::Point2D> points;
                
                for(auto const & tilePosition : m_tilePositions) {
                    points.push_back(tilePosition.first);
                }
                
                return points;
            }
            
            /**
            * \brief Add tile position to region.
            *
            * \param the tile position to add.
            */
            void AddTilePosition(std::shared_ptr<TilePosition> tilePosition) {
                tilePosition->second->setRegionId(m_id);
                
                float tileDistNearestUnpathable = tilePosition->second->getDistNearestUnpathable();
                
                if(m_largestDistUnpathable < tileDistNearestUnpathable) {
                    m_largestDistUnpathable = tileDistNearestUnpathable;
                    m_midPoint = tilePosition->first;
                }
                
                m_tilePositions.push_back(std::pair<sc2::Point2D, std::shared_ptr<Tile>>(tilePosition->first, tilePosition->second));
            }
            
            /**
            * \brief Add tile position to region.
            *
            * \param the tile position to add.
            */
            void AddTilePosition(TilePosition tilePosition) {
                tilePosition.second->setRegionId(m_id);
                
                float tileDistNearestUnpathable = tilePosition.second->getDistNearestUnpathable();
                
                if(m_largestDistUnpathable < tileDistNearestUnpathable) {
                    m_largestDistUnpathable = tileDistNearestUnpathable;
                    m_midPoint = tilePosition.first;
                }
                
                m_tilePositions.push_back(tilePosition);
            }
            
            /**
            * \brief get the longest path to a unpathable.
            *
            * \return the path.
            */
            double getLargestDistanceToUnpathable() const {
                return m_largestDistUnpathable;
            }
            
            /**
            * \brief Get the mid point of this region
            *
            * \return region mid point
            */
            sc2::Point2D getMidPoint() const {
                return m_midPoint;
            }
            
            /**
            * \brief Merges two regions.
            *
            * \param region The region to merge with THIS region.
            */
            void Merge(Region region) {
                for (auto& tilePosition : region.getTilePositions()) {
                    AddTilePosition(tilePosition);
                }
            }
            
            /**
            * \brief clear this region of its tile positions.
            */
            void Clear() {
                m_tilePositions.clear();
            }
            
        private:
            std::vector<TilePosition> m_tilePositions;
            std::vector<RegionEdge> m_edges;
            std::vector<UnitPosition> m_neutralUnitPositions;
            
            float m_largestDistUnpathable;
            sc2::Point2D m_midPoint;
            
            size_t m_id;
    };

    /**
    * \enum EdgeTyppe Region.h "Region.h"
    */
    enum EdgeType {
        cliff,
        impassible
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

#endif /* Region_h */
