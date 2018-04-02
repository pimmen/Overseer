#ifndef _OVERSEER_CHOKEPOINT_H_
#define _OVERSEER_CHOKEPOINT_H_

#include "Region.h"
#include "Graph.h"
#include <tuple>
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
            ChokePoint(Graph* graph, const Region* region1, const Region* region2, size_t cp_id, std::vector<TilePosition> tilePositions);

            /**
            * \brief Gets the position of Neutral units (according to Blizzard's sc2api)
            *
            * \return A vector containing the found neutralunitpositions
            */
            std::vector<UnitPosition> getNeutralUnitPositions() const;
            
            /**
            * \brief Gets the regions where a chokepoint exists between them.
            *
            * \return pair containing adjecent regions with chokepoint between them
            */
            std::pair<const Region *, const Region *> & getRegions();
            
            /**
            * \brief Get the size of the found chokepoint
            *
            * \return The size of the chokepoint
            */
            size_t Size() const;
            
            /**
            * \brief Gets the mid point of the chokepoint
            *
            * \return The mid point of the chokepoint
            */
            sc2::Point2D getMidPoint() const;

            /**
            * \brief Gets the point position of the chokepoint.
            *
            * \return vector containing chokepoint positions
            */
            std::vector<sc2::Point2D> getPoints();

            std::tuple<size_t, size_t, size_t> getId() const { return m_id; }

            

        private:
            /**
            * \brief Get the graph of the chokepoint
            *
            * \return chokepoint graph
            */
            Graph* GetGraph();

            std::tuple<size_t, size_t, size_t> m_id;
            std::pair<const Region *, const Region *> m_regions;
            std::vector<UnitPosition> m_neutralUnitPositions;
            std::vector<TilePosition> m_tilePositions;
            TilePosition m_center;

            Graph *p_graph;
    };
}

#endif /* _OVERSEER_CHOKEPOINT_H_ */
