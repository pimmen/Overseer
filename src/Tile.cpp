#include "Tile.h"

namespace Overseer{
	/*
	****************************
	*** Public members start ***
	****************************
	*/

	bool Tile::Buildable(){

		return m_tileInfo.buildable;
	}

	int Tile::GroundHeight(){

		return m_tileInfo.groundHeight;
	}

	bool Tile::Doodad(){

		return m_tileInfo.doodad;
	}

	void Tile::setBuildable(bool buildable){
		m_tileInfo.buildable = buildable;
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

    Tile::TileInfo::TileInfo():buildable(false),groundHeight(0),doodad(false) {}

	/*
	****************************
	*** Priavte members stop ***
	****************************
	*/
}