#ifndef MapImpl_h
#define MapImpl_h
#include "Graph.h"

class MapImpl : public Map {
private:
    Graph m_graph;
public:
    ~MapImpl(){}
    
    void Initialize(){
        
        m_graph.setMap(this);
        
        CreateTiles();
        
        ComputeAltitudes();
        
        std::vector<Region> tmp_regions = ComputeTempRegions();
        
        CreateRegions(tmp_regions);
        
        CreateFrontiers();
        
        m_graph.CreateChokePoints();
        
        m_graph.ComputePaths();
        
    }
    
    //Iterate over all integer coordinate positions and create a tile
    //Set default region id as 0
    void CreateTiles() {
        for (size_t x(0); x < m_width; ++x) {
            for (size_t y(0); y < m_height; ++y) {
                sc2::Point2D pos(x,y);
                
                bool buildable = (m_bot->Observation()->IsPlacable(pos) || m_bot->Observation()->IsPathable(pos));
                
                std::shared_ptr<Tile> tile = std::make_shared<Tile>();
                tile->setBuildable(buildable);
                tile->setRegionId(0);
                
                if(buildable) {
                    m_buildableTiles.push_back(std::shared_ptr<TilePosition>(new TilePosition(std::make_pair(pos, tile))));
                } else {
                    //Add ubuildable tiles to k-d tree, for rapid retrieval when it's time to compute altitudes
                    tile->setDistNearestUnpathable(0);
                    addTile(pos, tile);
                }
            }
        }
    }
    
    //Iterate over the tiles and compute the distance to nearest unpathable tile
    void ComputeAltitudes() {
        //For each buildable tile, find the distance to the nearest unbuildable tile
        for(auto& buildableTile: m_buildableTiles) {
            sc2::Point2D pos = buildableTile->first;
            
            for(neighbor_iterator<TilePositionContainer> iter = neighbor_begin(m_tilePositions, pos); iter != neighbor_end(m_tilePositions, pos); iter++) {
                if(!(iter->second->Buildable())){
                    buildableTile->second->setDistNearestUnpathable(distance(iter));
                    break;
                }
            }
        }
        
        std::sort(m_buildableTiles.begin(), m_buildableTiles.end(), GreaterTile());
        
        //Push buildable tiles into k-d tree
        for(const auto& buildableTile: m_buildableTiles) {
            addTile(buildableTile->first, buildableTile->second);
        }
    }
    
    //Iterate over all tiles, starting with those furthest away from unpathables (probable candidates for region centers), and add to neighboring region
    //Create new region if no neighboring region is found, if two are found merge the smaller into the larger or create frontier
    std::vector<Region> ComputeTempRegions() {
        std::vector<Region> tmp_regions(1);
        
        for(auto& buildableTile: m_buildableTiles) {
            std::pair<size_t, size_t> neighboringRegions = findNeighboringRegions(buildableTile);
            
            if(!neighboringRegions.first) {
                //std::cout << "New region " << tmp_regions.size() << std::endl;
                tmp_regions.emplace_back((size_t) tmp_regions.size(), buildableTile);
            } else if(!neighboringRegions.second) {
                //std::cout << "Add to region " << neighboringRegions.first << std::endl;
                tmp_regions[neighboringRegions.first].AddTilePosition(buildableTile);
            } else {
                size_t smaller = neighboringRegions.first;
                size_t larger = neighboringRegions.second;
                if(tmp_regions[larger].getArea() < tmp_regions[smaller].getArea()) {
                    std::swap(smaller, larger);
                }
                
                if(tmp_regions[smaller].getArea() < 80 ||
                   (buildableTile->second->getDistNearestUnpathable() / tmp_regions[smaller].getLargestDistanceToUnpathable() >= 0.90) ||
                   (buildableTile->second->getDistNearestUnpathable() / tmp_regions[larger].getLargestDistanceToUnpathable() >= 0.90)) {
                    //std::cout << "Merge " << smaller << " into " << larger << std::endl;
                    
                    tmp_regions[larger].AddTilePosition(buildableTile);
                    tmp_regions[larger].Merge(tmp_regions[smaller]);
                    tmp_regions[smaller].Clear();
                } else {
                    //std::cout << "No merge" << std::endl;
                    m_frontierPositions.push_back(buildableTile);
                }
            }
        }
        
        return tmp_regions;
    }
    
    //Find the regions with a real area and add them to map, resolve the frontiers
    void CreateRegions(std::vector<Region> tmp_regions) {
        size_t index = 1;
        for(auto& tmp_region: tmp_regions) {
            if(tmp_region.getArea() > 0) {
                tmp_region.setId(index);
                addRegion(tmp_region);
                index++;
            }
        }
    }
    
    void CreateFrontiers(){
        //Create frontier positions between regions
        for(auto& frontierPosition : getFrontierPositions()) {
            std::pair<size_t, size_t> neighboringRegions = findNeighboringRegions(frontierPosition);
            
            if(!neighboringRegions.second) {
                getRegion(neighboringRegions.first)->AddTilePosition(frontierPosition);
            } else {
                size_t smaller = neighboringRegions.first;
                size_t larger = neighboringRegions.second;
                
                if(getRegion(larger)->getArea() < getRegion(smaller)->getArea()) {
                    std::swap(smaller, larger);
                    neighboringRegions = std::make_pair(smaller, larger);
                }
                
                if(m_rawFrontier.count(neighboringRegions)) {
                    m_rawFrontier[neighboringRegions].push_back(*frontierPosition);
                } else {
                    std::vector<TilePosition> regionFrontier = {*frontierPosition};
                    m_rawFrontier[neighboringRegions] = regionFrontier;
                }
            }
        }
    }
    
    Graph getGraph(){ return m_graph; }
};

#endif /* MapImpl_h */
