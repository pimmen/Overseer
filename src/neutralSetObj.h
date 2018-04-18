#ifndef _NEUTRAL_SET_OBJ_H_
#define _NEUTRAL_SET_OBJ_H_

#include "sc2api/sc2_api.h"

namespace Overseer{

	struct NeutralImpl{
		NeutralImpl(sc2::ObservationInterface* obs);

		bool isNeutral(sc2::UNIT_TYPEID& check);
		bool isMineral(sc2::Point2D& pos);
		bool isGas(sc2::Point2D& pos);
		bool isDestructible(sc2::Point2D& pos);
		bool isNagaTower(sc2::Point2D& pos);

		std::map<sc2::Point2D, sc2::UNIT_TYPEID> neutralUnits;
	};
}

#endif /*_NEUTRAL_SET_OBJ_H_*/