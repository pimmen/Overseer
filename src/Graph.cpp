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

    std::vector<ChokePoint> Graph::getChokePoints() const {
        std::vector<ChokePoint> choke_points;
        std::vector<std::tuple<size_t, size_t, size_t>> ids;
        for (int i = 1; i <= num_regions; ++i) {
            for (int j = i + 1; j <= num_regions; ++j) {
                std::vector<ChokePoint> ij_choke_points = getChokePoints(i, j);
                std::cout << "Number of chokepoints between " << i << ", " << j << ": " << ij_choke_points.size() << std::endl;
                for(auto const& ij_cp : ij_choke_points) {
                    bool id_exists = false;
                    for(auto const& id : ids) {
                        std::tuple<size_t, size_t, size_t> ij_id = ij_cp.getId();

                        size_t id_1 = std::get<0>(id);
                        size_t id_2 = std::get<1>(id);
                        size_t id_3 = std::get<2>(id);
                        size_t ij_id_1 = std::get<0>(ij_id);
                        size_t ij_id_2 = std::get<1>(ij_id);
                        size_t ij_id_3 = std::get<2>(ij_id);

                        if(id == ij_id) {
                            // std::cout << "(" << id_1 << ", " << id_2 << ", " << id_3 << ") == (" 
                            // << ij_id_1 << ", " << ij_id_2 << ", " << ij_id_3 << ")" << std::endl;

                            id_exists = true;
                        } else {
                            // std::cout << "(" << id_1 << ", " << id_2 << ", " << id_3 << ") != (" 
                            // << ij_id_1 << ", " << ij_id_2 << ", " << ij_id_3 << ")" << std::endl;
                        }
                    }
                    if(!id_exists) {
                        choke_points.push_back(ij_cp);
                        ids.push_back(ij_cp.getId());
                    }
                }
                //choke_points.insert(choke_points.end(), ij_choke_points.begin(), ij_choke_points.end());
            }
        }
        return choke_points;
    }

    ChokePoint Graph::getChokePoint(ChokePointId cp_id) const {
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

        m_ChokePointsMatrix.resize(num_regions + 1);

        for(size_t i = 1; i <= num_regions; ++i) {
            m_ChokePointsMatrix[i].resize(i);
        }
        
        std::vector<ChokePoint> chokePoints;
        
        num_chokePoints = 0;
        
        for(auto const & frontierByRegionPair : p_map->getRawFrontier()) {
            //Flag to signify if the frontierPosition was added to cluster

            std::vector<std::deque<TilePosition>> clusters;
            
            size_t region_id_a = frontierByRegionPair.first.first;
            size_t region_id_b = frontierByRegionPair.first.second;

            if (region_id_a > region_id_b) {
                std::swap(region_id_a, region_id_b);
            }

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

            std::cout << "Number of dicsovered chokePoints: " << clusters.size() << std::endl;
            
            size_t num_clusters = 0;
            for(auto cluster : clusters) {
                std::vector<TilePosition> clusterPositions;
                
                while (!cluster.empty()) {
                    TilePosition clusterPosition = cluster.front();
                    clusterPositions.push_back(clusterPosition);
                    cluster.pop_front();
                }
                
                ChokePoint cp(this, p_map->getRegion(region_id_a), p_map->getRegion(region_id_b), num_clusters, clusterPositions);
                sc2::Point2D cp_point = cp.getMidPoint();

                m_ChokePointsMatrix[region_id_b][region_id_a].push_back(cp);

                std::cout << "Added chokepoint at: " << cp_point.x << ", " << cp_point.y << std::endl;
                std::cout << "Region pair: " << region_id_a << ", " << region_id_b << std::endl;
                //chokePoints.emplace_back(cp);

                num_clusters++;
            }
            
        }
        
        //ComputeAdjacencyMatrix(chokePoints);
        //InitializeChokePointsDistanceMatrix(chokePoints);

        std::cout << "Number of chokepoints: " << num_chokePoints << std::endl;
        std::cout << "Number of chokepoints calculated: " << getChokePoints().size() << std::endl;

        // std::tuple<size_t, size_t, size_t> a = std::make_tuple(1, 1, 1);
        // std::tuple<size_t, size_t, size_t> b = std::make_tuple(1, 1, 1);
        // std::cout << "Is a = b? " << (a == b) << std::endl;

        //ComputeRegionPointDistances(p_map->getRegions());
    }

    void Graph::ComputeAdjacencyMatrix(std::vector<ChokePoint> chokePoints) {

        std::cout << "Number of chokePoints: " << chokePoints.size() << std::endl;

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

        std::map<ChokePointId, float> g_score;
        std::map<ChokePointId, float> f_score;
        std::priority_queue<astar_node> cp_pq;

        std::map<ChokePointId, ChokePointId> came_from;



        return total_path;
    }

	/*
	****************************
	*** Priavte members stop ***
	****************************
	*/
}























