#include "MapImpl.h"

namespace Overseer{
    /*
    ****************************
    *** Public members start ***
    ****************************
    */

    MapImpl::~MapImpl(){}

    MapImpl::MapImpl(sc2::Agent* bot):Map(bot){}

    MapImpl::MapImpl():Map(){}

    void MapImpl::Initialize(){
        m_graph.setMap(this);
        CreateTiles();
        ComputeAltitudes();
        std::vector<Region> tmp_regions = ComputeTempRegions();
        CreateRegions(tmp_regions);
        CreateFrontiers();
        m_graph.CreateChokePoints();
    }

    Graph MapImpl::getGraph(){
    	
    	return m_graph; 
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

    void MapImpl::CreateTiles() {
        std::unique_ptr<NeutralImpl> neutralCheck(new NeutralImpl(m_bot->Observation()));

        std::cout << "Unit list size: " << unitList.size() << std::endl;

        for (size_t x(0); x < m_width; ++x) {

            for (size_t y(0); y < m_height; ++y) {
                sc2::Point2D pos(x,y);
                TileTerrain* set = checkTerrainType(neutralCheck, pos, m_bot->Observation());

                std::shared_ptr<Tile> tile = std::make_shared<Tile>();

                tile->setTileTerrain(&set);
                tile->setRegionId(0);
                
                if(*set != TileTerrain::flyOnly) {
                    m_buildableTiles.push_back(std::shared_ptr<TilePosition>(new TilePosition(std::make_pair(pos, tile))));
                
                } else {
                    //Add ubuildable tiles to k-d tree, for rapid retrieval when it's time to compute altitudes
                    tile->setDistNearestUnpathable(0);
                    addTile(pos, tile);
                }
            }
        }
    }

    void MapImpl::ComputeAltitudes() {
        
        //For each buildable tile, find the distance to the nearest unbuildable tile
        for(auto& buildableTile: m_buildableTiles) {
            sc2::Point2D pos = buildableTile->first;
            
            for(spatial::neighbor_iterator<TilePositionContainer> iter = neighbor_begin(m_tilePositions, pos); iter != neighbor_end(m_tilePositions, pos); iter++) {
                
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

    TileTerrain MapImpl::checkTerrainType(std::unique_ptr<NeutralImpl>& checkWith, sc2::Point2D& pos, ObservationInterface* obs){
        
        if(checkWith->isMineral(pos)){
            return new TileTerrain::mineral;
        } else if(checkWith->isGas(pos)){
            return new TileTerrain::gas;
        } else if(checkWith->isDestructible(pos)){
            return new TileTerrain::destructable;
        } else if(checkWith->isNagaTower(pos)){
            return new TileTerrain::nagaTower;
        } else if(obs->IsPlacable(pos) && obs->IsPathable(pos)){
            return new TileTerrain::buildAndPath;
        } else if(obs->IsPathable(pos)){
            return new TileTerrain::path;
        } else if(obs->IsPlacable(pos)){
            return new TileTerrain::build;
        } else {
            return new TileTerrain::flyOnly;
        }
    }

    std::vector<Region> MapImpl::ComputeTempRegions() {
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
                
                if(tmp_regions[smaller].getArea() < min_region_area ||
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

    void MapImpl::CreateRegions(std::vector<Region> tmp_regions) {
        size_t index = 1;
        
        for(auto& tmp_region: tmp_regions) {
            
            if(tmp_region.getArea() > 0) {
                tmp_region.setId(index);
                addRegion(tmp_region);
                index++;
            }
        }
    }

    void MapImpl::CreateFrontiers(){
        
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

    /*
    ****************************
    *** Priavte members stop ***
    ****************************
    */
}