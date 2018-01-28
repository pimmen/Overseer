#ifndef ChokePoint_h
#define ChokePoint_h

#include "Region.h"
#include "Graph.h"

#include <utility>

namespace Overseer{
    class Graph;

    /**
    * \class ChokePoint ChokePoint.h "ChokePoint.h"
    * \brief Class that is used as a chokepoint container with size and positioning on the map.
    */
    class ChokePoint {
        public:

            /**
            * \brief class Constructor
            *
            * \param graph The graphical representation of the game map.
            * \param region1 is a adjecent region to region2
            * \param region2 is a adjecent region to region1
            * \param tilePositions is the "limits" between region1 and region2
            */
            ChokePoint(Graph *graph, const Region* region1, const Region* region2, std::vector<TilePosition> tilePositions){
                p_graph = graph;
                
                m_regions.first = region1;
                m_regions.second = region2;
                m_tilePositions = tilePositions;

                std::vector<TilePosition>::iterator midTilePosition = std::max_element(m_tilePositions.begin(), m_tilePositions.end(),
                    [](TilePosition a, TilePosition b){ return a.second->getDistNearestUnpathable() < b.second->getDistNearestUnpathable(); });

                m_center = *midTilePosition;
            }

            /**
            * \brief Gets the position of Neutral units (according to Blizzard's sc2api)
            *
            * \return A vector containing the found neutralunitpositions
            */
            std::vector<UnitPosition> getNeutralUnitPositions() const {return m_neutralUnitPositions;}
            
            /**
            * \brief Gets the regions where a chokepoint exists between them.
            *
            * \return pair containing adjecent regions with chokepoint between them
            */
            std::pair<const Region *, const Region *> & getRegions() {return m_regions;}
            
            /**
            * \brief Get the size of the found chokepoint
            *
            * \return The size of the chokepoint
            */
            size_t Size() const { return m_tilePositions.size(); }
            
            /**
            * \brief Gets the mid point of the chokepoint
            *
            * \return The mid point of the chokepoint
            */
            sc2::Point2D getMidPoint() const {
                
                return m_center.first;
            }

            /**
            * \brief Gets the point position of the chokepoint.
            *
            * \return vector containing chokepoint positions
            */
            std::vector<sc2::Point2D> getPoints() {
                std::vector<sc2::Point2D> points;
                
                for(auto const & tilePosition : m_tilePositions) {
                    points.push_back(tilePosition.first);
                }
                
                return points;
            }

            

        private:
            /**
            * \brief Get the graph of the chokepoint
            *
            * \return chokepoint graph
            */
            Graph * GetGraph(){ return p_graph; }

            std::pair<const Region *, const Region *> m_regions;
            std::vector<UnitPosition> m_neutralUnitPositions;
            std::vector<TilePosition> m_tilePositions;
            TilePosition m_center;

            Graph *p_graph;
    };
}

#endif /* ChokePoint_h */
