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
        std::vector<ChokePoint> result_vector;
        if(ValidId(region_id_a) && ValidId(region_id_b) && (region_id_a != region_id_b)) {
            if (region_id_a > region_id_b) {
                std::swap(region_id_a, region_id_b);
            }

            result_vector =  m_ChokePointsMatrix[region_id_b][region_id_a];
        }
        return result_vector;
    }

    std::vector<ChokePoint> Graph::getChokePoints() const {
        std::vector<ChokePoint> choke_points;
        std::vector<ChokePointId> ids;
        for (size_t i = 1; i <= num_regions; ++i) {
            for (size_t j = i + 1; j <= num_regions; ++j) {
                std::vector<ChokePoint> ij_choke_points = getChokePoints(i, j);
                for(auto const& ij_cp : ij_choke_points) {
                    bool id_exists = false;
                    for(auto const& id : ids) {
                        ChokePointId ij_id = ij_cp.getId();
                        if(id == ij_id) {
                            id_exists = true;
                        }
                    }
                    if(!id_exists) {
                        choke_points.push_back(ij_cp);
                        ids.push_back(ij_cp.getId());
                    }
                }
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
        
        //std::vector<ChokePoint> chokePoints;
        
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
                
                ChokePoint cp(this, p_map->getRegion(region_id_a), p_map->getRegion(region_id_b), num_clusters, clusterPositions);
                std::cout << "Created chokepoint between: " << region_id_a << ", " << region_id_b << std::endl;
                print_cp_id(cp.getId());

                m_ChokePointsMatrix[region_id_b][region_id_a].push_back(cp);
                m_RegionChokePoints[region_id_a].push_back(cp);
                m_RegionChokePoints[region_id_b].push_back(cp);

                num_clusters++;
            }
        }

        ChokePoint cp_a = getChokePoints()[0];
        ChokePoint cp_b = getChokePoints()[1];
        std::cout << "Number of chokepoints: " << getChokePoints().size();
        CPPath path = Astar(cp_a, cp_b);
    }

    // void Graph::ComputeAdjacencyMatrix(std::vector<ChokePoint> chokePoints) {

    //     std::cout << "Number of chokePoints: " << chokePoints.size() << std::endl;

    //     for(auto & chokePoint : chokePoints) {
    //         size_t region_id_a = chokePoint.getRegions().first->getId();
    //         size_t region_id_b = chokePoint.getRegions().second->getId();

    //         if (region_id_a > region_id_b) {
    //             std::swap(region_id_a, region_id_b);
    //         }

    //         m_ChokePointsMatrix[region_id_b][region_id_a].push_back(chokePoint);
    //     }
    // }

    // void Graph::InitializeChokePointsDistanceMatrix(std::vector<ChokePoint> chokePoints) {
    //     m_ChokePointsDistanceMatrix.resize(num_regions + 1);
        
    //     for (size_t i = 1; i <= num_regions; ++i) {
    //         m_ChokePointsDistanceMatrix[i].resize(i + 1);
    //     }
        
    //     for(size_t i = 1; i <= num_regions; ++i) {
    //         for(size_t j = 1; j <= i; ++j) {
    //             if(i == j) {
    //                 SetRegionDistance(i, j, 0.0);
    //             } else if(getChokePoints(i, j).empty()) {
    //                 SetRegionDistance(i, j, std::numeric_limits<float>::infinity());
    //             } else {
    //                 SetRegionDistance(i, j, 1);
    //             }
    //         }
    //     }
    // }

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
        std::vector<ChokePointId> closed_set;

        std::map<ChokePointId, float> g_score;
        std::map<ChokePointId, float> f_score;
        std::priority_queue<astar_node, std::vector<astar_node>,Compare> open_set;

        std::map<ChokePointId, ChokePointId> came_from;
        std::map<ChokePointId, std::vector<ChokePointId>> cp_neighbors;

        astar_node node_a(cp_a.getId(), EuclideanDistance(cp_a.getMidPoint(), cp_b.getMidPoint()));
        open_set.push(node_a);


        //Initialize scores and neighbors for all nodes
        for (auto const& cp : getChokePoints()) {
            if(cp.getId() == cp_a.getId()) {
                //Start to itself has distance 0, first guess to goal is heuristic

                // std::cout << "Initialize itself to 0" << std::endl;
                // print_cp_id(cp.getId());
                // print_cp_id(cp_a.getId());
                g_score[cp.getId()] = 0;
                f_score[cp.getId()] = EuclideanDistance(cp.getMidPoint(), cp_b.getMidPoint());
            } else {
                //Other nodes relative to start and the goals are initialized as infinity

                // std::cout << "Initialize other to inf" << std::endl;
                // print_cp_id(cp.getId());
                // print_cp_id(cp_a.getId());
                g_score[cp.getId()] = std::numeric_limits<float>::infinity();
                f_score[cp.getId()] = std::numeric_limits<float>::infinity();
            }

            //Get chokepoints from the regions, set them as neighbors
            size_t region_a = cp.getRegions().first->getId();
            size_t region_b = cp.getRegions().second->getId();

            for (auto const& potential_neighbor : m_RegionChokePoints[region_a]) {
                if (potential_neighbor.getId() != cp.getId()) {
                    // std::cout << "Real neighbors" << std::endl;
                    // print_cp_id(potential_neighbor.getId());
                    // print_cp_id(cp.getId());
                    cp_neighbors[cp.getId()].push_back(potential_neighbor.getId());
                }
            }
            for (auto const& potential_neighbor : m_RegionChokePoints[region_b]) {
                if (potential_neighbor.getId() != cp.getId()) {
                    // std::cout << "Real neighbors" << std::endl;
                    // print_cp_id(potential_neighbor.getId());
                    // print_cp_id(cp.getId());
                    cp_neighbors[cp.getId()].push_back(potential_neighbor.getId());
                }
            }
        }

        while(!open_set.empty()) {
            astar_node current = open_set.top();
            ChokePointId current_id = current.choke_point_id;
            closed_set.push_back(current_id);
            open_set.pop();

            for(auto const& neighbor_id : cp_neighbors[current_id]) {
                if(std::find(closed_set.begin(), closed_set.end(), current_id) != closed_set.end()) {
                    // std::cout << "Already evaluated" << std::endl;
                    // print_cp_id(current_id);
                    continue;
                }
                //std::cout << "Found new node!" << std::endl;
                ChokePoint current_cp = getChokePoint(current_id);
                ChokePoint neighbor_cp = getChokePoint(neighbor_id);

                float neighbor_dist = p_map->getBot()->Query()->PathingDistance(current_cp.getMidPoint(), neighbor_cp.getMidPoint());
                float tentative_g_score = g_score[current_id] + neighbor_dist;

                if(tentative_g_score > g_score[neighbor_id]) {
                    continue;
                }

                came_from[neighbor_id] = current_id;
                g_score[neighbor_id] = tentative_g_score;
                f_score[neighbor_id] = tentative_g_score + EuclideanDistance(neighbor_cp.getMidPoint(), cp_b.getMidPoint());

                astar_node neighbor_node(neighbor_id, f_score[neighbor_id]);
                open_set.push(neighbor_node);
            }
        }

        return total_path;
    }

	/*
	****************************
	*** Priavte members stop ***
	****************************
	*/
}























