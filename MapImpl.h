//
//  MapImpl.h
//  martin
//
//  Created by Pontus Ekholm on 2017-10-17.
//

#ifndef MapImpl_h
#define MapImpl_h
#include <algorithm>
#include "Map.h"

class MapImpl : public Map {
private:
    std::pair<size_t, size_t> findNeighboringAreas(std::shared_ptr<TilePosition> tilePosition) {
        std::pair<size_t, size_t> result(0,0);
        
        for(sc2::Point2D delta: {sc2::Point2D(0,-1), sc2::Point2D(0,1), sc2::Point2D(-1,0), sc2::Point2D(1,0)}) {
            if(Valid(tilePosition->first + delta)) {
                std::shared_ptr<Tile> deltaTile = GetTile(tilePosition->first + delta);
                if(deltaTile->Buildable()) {
                    size_t regionId = deltaTile->getRegionId();
                    
                    if(regionId) {
                        if(!result.first) {
                            result.first = regionId;
                        } else if(result.first != regionId) {
                            if(!result.second || regionId < result.second) {
                                result.second = regionId;
                            }
                        }
                    }
                }
            }
        }
        
        return result;
    }
    
public:
    ~MapImpl(){}
    
    void Initialize(){
        
        CreateTiles();
        
        ComputeAltitudes();
        
        ComputeRegions();
        
    }
    
    void CreateTiles() {
        //Create all tiles, add them to multimap if not buildable add them to a vector if they are
        for (size_t x(0); x < m_width; ++x) {
            for (size_t y(0); y < m_height; ++y) {
                sc2::Point2D pos(x,y);
                
                bool pathable = m_bot->Observation()->IsPlacable(pos);
                
                std::shared_ptr<Tile> tile = std::make_shared<Tile>();
                tile->setBuildable(pathable);
                tile->setRegionId(0);
                
                if(pathable) {
                    m_buildableTiles.push_back(std::shared_ptr<TilePosition>(new TilePosition(std::make_pair(pos, tile))));
                } else {
                    tile->setDistNearestUnpathable(0);
                    addTile(pos, tile);
                }
            }
        }
    }
    
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
    
    void ComputeRegions() {
        std::vector<Region> tmp_regions(1);
        int noMerge = 0;
        for(auto& buildableTile: m_buildableTiles) {
            std::pair<size_t, size_t> neighboringAreas = findNeighboringAreas(buildableTile);
            
            if(!neighboringAreas.first) {
                std::cout << "New region " << tmp_regions.size() << std::endl;
                tmp_regions.emplace_back((size_t) tmp_regions.size(), buildableTile);
            } else if(!neighboringAreas.second) {
                std::cout << "Add to region " << neighboringAreas.first << std::endl;
                tmp_regions[neighboringAreas.first].AddTilePosition(buildableTile);
            } else {
                size_t smaller = neighboringAreas.first;
                size_t larger = neighboringAreas.second;
                if(tmp_regions[larger].getArea() < tmp_regions[smaller].getArea()) {
                    std::swap(smaller, larger);
                }
                
                if(tmp_regions[smaller].getArea() < 80 ||
                   (buildableTile->second->getDistNearestUnpathable() / tmp_regions[smaller].getLargestDistanceToUnpathable() >= 0.90) ||
                   (buildableTile->second->getDistNearestUnpathable() / tmp_regions[larger].getLargestDistanceToUnpathable() >= 0.90)) {
                    std::cout << "Merge " << smaller << " into " << larger << std::endl;
                    
                    tmp_regions[larger].AddTilePosition(buildableTile);
                    tmp_regions[larger].Merge(tmp_regions[smaller]);
                    tmp_regions[smaller].Clear();
                } else {
                    noMerge++;
                    std::cout << "No merge: " << noMerge << std::endl;
                }
            }
        }
    }
};

#endif /* MapImpl_h */
