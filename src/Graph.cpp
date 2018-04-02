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

    ChokePoint Graph::getChokePoint(std::tuple<size_t, size_t, size_t> cp_id) const {
        size_t region_id_a = std::get<0>(cp_id);
        size_t region_id_b = std::get<1>(cp_id);
        size_t cp_pos = std::get<2>(cp_id);

        assert(ValidId(region_id_a) && ValidId(region_id_b));

        if (region_id_a > region_id_b) {
            std::swap(region_id_a, region_id_b);
        }

        return m_ChokePointsMatrix[region_id_b][region_id_a][cp_pos];
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
                    //Record new ChokePoint
                    num_chokePoints++;
                    
                    std::deque<TilePosition> clusterPositions;
                    clusterPositions.push_back(frontierPosition);
                    clusters.push_back(clusterPositions);
                }
            }
            
            size_t num_clusters = 0;
            for(auto cluster : clusters) {
                std::vector<TilePosition> clusterPositions;
                
                while (!cluster.empty()) {
                    TilePosition clusterPosition = cluster.front();
                    clusterPositions.push_back(clusterPosition);
                    cluster.pop_front();
                }
                
                ChokePoint cp(this, p_map->getRegion(regionIdA), p_map->getRegion(regionIdB), num_clusters, clusterPositions);
                chokePoints.emplace_back(cp);

                num_clusters++;
            }
            
        }
        
        ComputeAdjacencyMatrix(chokePoints);
        InitializeChokePointsDistanceMatrix(chokePoints);

        //ComputeRegionPointDistances(p_map->getRegions());
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

    void Graph::InitializeChokePointsDistanceMatrix(std::vector<ChokePoint> chokePoints) {
        m_ChokePointsDistanceMatrix.resize(num_regions + 1);
        
        for (size_t i = 1; i <= num_regions; ++i) {
            m_ChokePointsDistanceMatrix[i].resize(i + 1);
        }
        
        for(size_t i = 1; i <= num_regions; ++i) {
            for(size_t j = 1; j <= i; ++j) {
                if(i == j) {
                    SetRegionDistance(i, j, 0.0);
                } else if(getChokePoints(i, j).empty()) {
                    SetRegionDistance(i, j, std::numeric_limits<float>::infinity());
                } else {
                    SetRegionDistance(i, j, 1);
                }
            }
        }
    }

    void Graph::ComputeRegionPointDistances(std::vector<std::shared_ptr<Region>> regions) {
        size_t region_num = 1;
        for (auto & region : regions)
        {
            std::cout << "Computing distances for region " << region_num << std::endl;
            std::map<PointPair, float, ComparePointPairs> pointDistances;
        
            for (auto & l_tilePosition : region->getTilePositions()) {
                for (auto & r_tilePosition : region->getTilePositions()) {
                    //float dist = p_map->getBot()->Query()->PathingDistance(l_tilePosition.first, r_tilePosition.first);
                    //pointDistances[std::make_pair(l_tilePosition.first, r_tilePosition.first)] = dist;
                }
            }
            
            //region->setPointDistances(pointDistances);
            region_num++;
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

    CPPath Graph::Astar(ChokePoint cp_a, ChokePoint cp_b) {
        std::vector<ChokePoint> total_path;
        std::vector<ChokePoint> visited_cp;
        std::vector<ChokePoint> unreached_cp;

        visited_cp.push_back(cp_a);

        std::map<std::tuple<size_t, size_t, size_t>, float> g_score;
        std::priority_queue<astar_node> f_scores;

        std::map<std::tuple<size_t, size_t, size_t>, ChokePoint> came_from;


        return total_path;
    }

	/*
	****************************
	*** Priavte members stop ***
	****************************
	*/
}























