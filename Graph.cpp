#include "Graph.h"

namespace Overseer{
	/*
	****************************
	*** Public members start ***
	****************************
	*/

	Graph::Graph(){}

	Graph::Graph(Map* map):p_map(map){}

	std::vector<ChokePoint> Graph::getChokePoints(size_t region_id_a, size_t region_id_b) const {
        assert(ValidId(region_id_a) && ValidId(region_id_b));

        if (region_id_a > region_id_b) {
            std::swap(region_id_a, region_id_b);
        }

        return m_ChokePointsMatrix[region_id_b][region_id_a];
    }

    void Graph::CreateChokePoints() {
        num_regions = p_map->getRegions().size();

        std::vector<ChokePoint> chokePoints;
        std::vector<std::deque<TilePosition>> clusters;

        for(auto const & frontierByRegionPair : p_map->getRawFrontier()) {
            //Flag to signify if the frontierPosition was added to cluster
            size_t regionIdA = frontierByRegionPair.first.first;
            size_t regionIdB = frontierByRegionPair.first.second;
            std::vector<TilePosition> frontierPositions = frontierByRegionPair.second;
            std::sort(frontierPositions.begin(), frontierPositions.end(), GreaterTileInstance());

            for(auto frontierPosition : frontierPositions) {
                bool added = false;

                for(auto & cluster : clusters) {
                    float dist_front = sc2::Distance2D(frontierPosition.first, cluster.front().first);
                    float dist_back = sc2::Distance2D(frontierPosition.first, cluster.back().first);

                    if(std::min(dist_front, dist_back) <= min_cluster_distance) {

                        if(dist_front < dist_back) {
                            cluster.push_front(frontierPosition);

                        } else {
                            cluster.push_back(frontierPosition);
                        }
                        added = true;

                        break;
                    }
                }

                if(!added) {
                    std::deque<TilePosition> clusterPositions;
                    clusterPositions.push_back(frontierPosition);
                    clusters.push_back(clusterPositions);
                }
            }

            for(auto cluster : clusters) {
                std::vector<TilePosition> clusterPositions;
                
                while (!cluster.empty()) {
                    TilePosition clusterPosition = cluster.front();
                    clusterPositions.push_back(clusterPosition);
                    cluster.pop_front();
                }
                ChokePoint cp(this, p_map->getRegion(regionIdA), p_map->getRegion(regionIdB),clusterPositions);
                chokePoints.emplace_back(cp);
            }
        }
        ComputeAdjacencyMatrix(chokePoints);
    }

    void Graph::ComputeAdjacencyMatrix(std::vector<ChokePoint> chokePoints) {
        m_ChokePointsMatrix.resize(num_regions + 1);

        for(size_t i = 1; i <= num_regions; ++i) {
            m_ChokePointsMatrix[i].resize(i);
        }

        for(auto & chokePoint : chokePoints) {
            size_t region_id_a = chokePoint.getRegions().first->getId();
            size_t region_id_b = chokePoint.getRegions().second->getId();

            if (region_id_a > region_id_b) {
                std::swap(region_id_a, region_id_b);
            }

            m_ChokePointsMatrix[region_id_b][region_id_a].push_back(chokePoint);
        }
    }

    void Graph::setMap(Map *map) { 
    	p_map = map;
    }

	/*
	***************************
	*** Public members stop ***
	***************************

	***************************
	***************************
	***************************

	*****************************
	*** Priavte members start ***
	*****************************
	*/

    bool Graph::ValidId(size_t id_arg) const {
    
        return (id_arg >= 1) && (id_arg <= (num_regions + 1));
    }

	/*
	****************************
	*** Priavte members stop ***
	****************************
	*/
}