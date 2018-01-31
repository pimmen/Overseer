#include "ChokePoint.h"

namespace Overseer{
	/*
	****************************
	*** Public members start ***
	****************************
	*/

	ChokePoint::ChokePoint(Graph *graph, const Region* region1, const Region* region2, std::vector<TilePosition> tilePositions){
	    p_graph = graph;
	    m_regions.first = region1;
	    m_regions.second = region2;
	    m_tilePositions = tilePositions;
	    std::vector<TilePosition>::iterator midTilePosition = std::max_element(m_tilePositions.begin(), m_tilePositions.end(),
	        [](TilePosition a, TilePosition b){ return a.second->getDistNearestUnpathable() < b.second->getDistNearestUnpathable(); });

	    m_center = *midTilePosition;
	}

	std::vector<UnitPosition> ChokePoint::getNeutralUnitPositions() const {

		return m_neutralUnitPositions;
	}

	std::pair<const Region *, const Region *> & ChokePoint::getRegions() {

		return m_regions;
	}

	size_t ChokePoint::Size() const {

		return m_tilePositions.size();
	}

	sc2::Point2D ChokePoint::getMidPoint() const {
	                
	    return m_center.first;
	}

	std::vector<sc2::Point2D> ChokePoint::getPoints() {
	    std::vector<sc2::Point2D> points;

	    for(auto const & tilePosition : m_tilePositions) {
	        points.push_back(tilePosition.first);
	    }

	    return points;
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

	Graph* ChokePoint::GetGraph(){

		return p_graph;
	}

	/*
	****************************
	*** Priavte members stop ***
	****************************
	*/
}