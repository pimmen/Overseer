#include "Tile.h"

namespace Overseer{
	/*
	****************************
	*** Public members start ***
	****************************
	*/

	TileTerrain Tile::getTileTerrain(){

		return m_tileInfo.terrain;
	}

	int Tile::GroundHeight(){

		return m_tileInfo.groundHeight;
	}

	/*bool Tile::Doodad(){

		return m_tileInfo.doodad;
	}*/

	void Tile::setTileTerrain(TileTerrain path){
		m_tileInfo.terrain = terrain;
	}

	void Tile::setDistNearestUnpathable(float dist){
		m_distNearestUnpathable = dist;
	}

	float Tile::getDistNearestUnpathable() const {

		return m_distNearestUnpathable;
	}

	void Tile::setRegionId(size_t regionId) {
        m_regionId = regionId;
    }

    size_t Tile::getRegionId(){
        
        return m_regionId;
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

    Tile::m_tileInfo::m_tileInfo():terrain(TileTerrain::unpathable),groundHeight(0)/*,doodad(false)*/{}

	/*
	****************************
	*** Priavte members stop ***
	****************************
	*/
}