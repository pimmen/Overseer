#include "Map.h"

namespace Overseer{

	int point2d_accessor::operator() (spatial::dimension_type dim, const sc2::Point2D p) const {
        
        switch(dim) {
            
            case 0: return p.x;
            
            case 1: return p.y;
            
            default: throw std::out_of_range("dim");
        }
    }


	/*
	****************************
	*** Public members start ***
	****************************
	*/

	Map::Map(){}

	Map::Map(sc2::Agent* bot){
        m_bot = bot;
        m_width  = m_bot->Observation()->GetGameInfo().width;
        m_height = m_bot->Observation()->GetGameInfo().height;
    }

    size_t Map::getHeight() const {

    	return m_height;
    }

    size_t Map::getWidth() const {

    	return m_width;
    }

    std::vector<std::shared_ptr<Region>> Map::getRegions() {
        std::vector<std::shared_ptr<Region>> regions;
        
        for(RegionMap::iterator it = m_regions.begin(); it != m_regions.end(); it++) {
            regions.push_back(it->second);
        }
        
        return regions;
    }

    Region* Map::getRegion(size_t id){
        
        return (m_regions.find(id) != m_regions.end()) ? m_regions[id].get() : nullptr;
    }

    const Region* Map::getNearestRegion(sc2::Point2D pos){
        Region *region = nullptr;
        
        for(spatial::neighbor_iterator<TilePositionContainer> iter = neighbor_begin(m_tilePositions, pos); iter != neighbor_end(m_tilePositions, pos); iter++) {
            
            if(iter->second->getRegionId()){
                
                return getRegion(iter->second->getRegionId());
            }
        }
        
        return region;
    }

    void Map::addTile(sc2::Point2D& pos, std::shared_ptr<Tile> tile){
        m_tilePositions.insert(std::make_pair(pos, tile));
    }

    bool Map::Valid(sc2::Point2D pos) const {
        
        return ((0 <= pos.x) && (pos.x <= m_width) && (0 <= pos.y) && (pos.y <= m_height));
    }

    TilePosition Map::getClosestTilePosition(sc2::Point2D pos) {
        spatial::neighbor_iterator<TilePositionContainer> iter = neighbor_begin(m_tilePositions, pos);
        iter++;
        
        return *iter;
    }

    void Map::addRegion(Region region) {
    	m_regions[region.getId()] = std::make_shared<Region>(region);
    }

    std::shared_ptr<Tile> Map::GetTile(sc2::Point2D pos) {

    	return m_tilePositions.find(pos)->second;
    }

    size_t Map::size() {

    	return m_tilePositions.size();
    }

    TilePositionContainer Map::getTilePositions() {

    	return m_tilePositions;
    }

    void Map::setBot(sc2::Agent* bot){
        m_bot = bot;
        m_width  = m_bot->Observation()->GetGameInfo().width;
        m_height = m_bot->Observation()->GetGameInfo().height;
    }

    std::vector<std::shared_ptr<TilePosition>> Map::getFrontierPositions(){

    	return m_frontierPositions;
    }

    RawFrontier Map::getRawFrontier() {

    	return m_rawFrontier;
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

    std::pair<size_t, size_t> Map::findNeighboringRegions(std::shared_ptr<TilePosition> tilePosition) {
        std::pair<size_t, size_t> result(0,0);
        
        for(sc2::Point2D delta: {sc2::Point2D(0,-1), sc2::Point2D(0,1), sc2::Point2D(-1,0), sc2::Point2D(1,0)}) {
            if(Valid(tilePosition->first + delta)) {
                std::shared_ptr<Tile> deltaTile = GetTile(tilePosition->first + delta);
                if(deltaTile->getTileTerrain()) {
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

	/*
	****************************
	*** Priavte members stop ***
	****************************
	*/
}