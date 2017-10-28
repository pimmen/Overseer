//
//  MapImpl.h
//  martin
//
//  Created by Pontus Ekholm on 2017-10-17.
//

#ifndef MapImpl_h
#define MapImpl_h
#include "Map.h"
#include "spatial/neighbor_iterator.hpp"

class MapImpl : public Map {
private:
    sc2::Agent* m_bot;
public:
    MapImpl(sc2::Agent* bot){m_bot = bot;}
    MapImpl(){};
    
    void setBot(sc2::Agent* bot){m_bot = bot;}
    
    void Initialize(){
        m_width  = m_bot->Observation()->GetGameInfo().width;
        m_height = m_bot->Observation()->GetGameInfo().height;
        
        std::vector<sc2::Point2D> buildablePoints;
        
        for (size_t x(0); x < m_width; ++x)
        {
            for (size_t y(0); y < m_height; ++y)
            {
                sc2::Point2D pos(x,y);

                bool buildable = m_bot->Observation()->IsPlacable(pos);
                
                
                if(buildable) {
                    buildablePoints.push_back(pos);
                } else {
                    Tile tile;
                    tile.setBuildable(buildable);
                    addTile(pos, tile);
                }
            }
        }
        
        for(int i = 0; i < buildablePoints.size(); i++) {
            sc2::Point2D pos = buildablePoints.at(i);
            std::cout << "Neighbor search from " << pos.x << ", " << pos.y << std::endl;
            spatial::neighbor_iterator<TileContainer> iter = spatial::neighbor_begin(m_tilePositions, pos);
            
            while(iter != spatial::neighbor_end(m_tilePositions, pos)) {
                std::cout << iter->second.Buildable() << std::endl;
                if(!(iter->second.Buildable())){
                    std::cout << "Distance to nearest unplaceable: " << spatial::distance(iter) << std::endl;
                    break;
                }
            }
        }
    }
};

#endif /* MapImpl_h */
