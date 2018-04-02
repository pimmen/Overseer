#ifndef _OVERSEER_GRAPH_H_
#define _OVERSEER_GRAPH_H_

#include "Map.h"
#include "ChokePoint.h"
#include "Region.h"

namespace Overseer{

    class ChokePoint;
    class Map;

    typedef std::vector<ChokePoint> CPPath;

    struct astar_node {
        ChokePoint cp;
        float f_score;
    };

    bool operator<(const astar_node& lhs, const astar_node& rhs) {
        return lhs.f_score < rhs.f_score;
    }
    
    /**
    * \class Graph Graph.h "Graph.h"
    * \brief Is a mathematical "graphmap" for analysing.
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

            ChokePoint getChokePoint(std::tuple<size_t, size_t, size_t> cp_id) const;

            float EuclideanDistance(sc2::Point2D p1, sc2::Point2D p2) const {
                float x_dist = std::abs(p1.x - p2.x);
                float y_dist = std::abs(p1.y - p2.y);

                return std::sqrt(std::pow(x_dist, 2) + std::pow(y_dist, 2));
            }

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

            void ComputeRegionPointDistances(std::vector<std::shared_ptr<Region>> regions);

            void InitializeChokePointsDistanceMatrix(std::vector<ChokePoint> chokePoints);

            /**
            * \brief set the sc2 map to this class.
            *
            * \param map is a pointer to the map.
            */
            void setMap(Map *map);

        private:
            size_t num_regions;
            size_t num_chokePoints;
            Map *p_map;
            std::vector<std::vector<std::vector<ChokePoint>>> m_ChokePointsMatrix;
            std::vector<std::vector<float>> m_ChokePointsDistanceMatrix;
            std::map<size_t, std::vector<ChokePoint>> m_RegionChokePoints;
            
            bool ValidId(size_t id_arg) const {
                return (id_arg >= 1) && (id_arg <= (num_regions + 1));
            }
            
            void SetRegionDistance(size_t region_id_a, size_t region_id_b, float dist) {
                if (region_id_a > region_id_b) {
                    std::swap(region_id_a, region_id_b);
                }
                m_ChokePointsDistanceMatrix[region_id_b][region_id_a] = dist;
            }
            
            float GetDistance(size_t region_id_a, size_t region_id_b) {
                if (region_id_a > region_id_b) {
                    std::swap(region_id_a, region_id_b);
                }
                return m_ChokePointsDistanceMatrix[region_id_b][region_id_a];
            }

            CPPath Astar(ChokePoint cp_a, ChokePoint cp_b);

            static const size_t min_cluster_distance = 17;
    };
}

#endif /* _GRAPH_H_ */
