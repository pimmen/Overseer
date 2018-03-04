#ifndef _OVERSEER_GRAPH_H_
#define _OVERSEER_GRAPH_H_

#include "Map.h"
#include "ChokePoint.h"
#include "Region.h"

namespace Overseer{

    class ChokePoint;
    class Map;
    
    /**
    * \class Graph Graph.h "Graph.h"
    * \brief Is a mathiimatical "graphmap" of for analising.
    */
    class Graph {

        public:

            /**
            * \brief empty constructor
            */
            Graph();
            /**
            * \brief object constructor.
            *
            * \param map The map to make the graph on.
            */
            Graph(Map* map);

            /**
            * \brief Gets the chokepoints been descovered between two adjacent regions.
            *
            * \param region_id_a a region which is adjacent to region_id_b
            * \param region_id_b a region which is adjacent to region_id_a
            * \return A vector containing all the found chokepoint between the two regions.
            */
            std::vector<ChokePoint> getChokePoints(size_t region_id_a, size_t region_id_b) const;

            /**
            * \brief find and create the chokepoint on the map.
            */
            void CreateChokePoints();

            /**
            * \brief Build triagular matrix to store region adjacency.
            *
            * \param chokePoints A vector that contains the found chokepoints.
            */
            void ComputeAdjacencyMatrix(std::vector<ChokePoint> chokePoints);

            /**
            * \brief set the sc2 map to this class.
            *
            * \param map is a pointer to the map.
            */
            void setMap(Map *map);

        private:
            bool ValidId(size_t id_arg) const;
            
            size_t num_regions;
            Map *p_map;
            std::vector<std::vector<std::vector<ChokePoint>>> m_ChokePointsMatrix;

            static const size_t min_cluster_distance = 17;
    };
}

#endif /* _GRAPH_H_ */
