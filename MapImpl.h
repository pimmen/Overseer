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
                size_t regionId = GetTile(tilePosition->first + delta)->getRegionId();
                
                if(regionId > 0) {
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
        for(auto& buildableTile: m_buildableTiles) {
            std::pair<size_t, size_t> neighboringAreas = findNeighboringAreas(buildableTile);
            
            if(!neighboringAreas.first) {
                
            } else if(!neighboringAreas.second) {
                buildableTile->second->setRegionId(neighboringAreas.second);
            } else {
                
            }
        }
    }
};

#endif /* MapImpl_h */
