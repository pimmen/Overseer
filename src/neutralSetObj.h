#ifndef _NEUTRAL_SET_OBJ_H_
#define _NEUTRAL_SET_OBJ_H_

namespace Overseer{

	struct NeutralImpl{
		NeutralImpl(ObservationInterface* obs);

		bool isNeutral(sc2::UNIT_TYPEID& check);
		bool isMineral(sc2::Point2D& pos);
		bool isGas(sc2::Point2D& pos);
		bool isDestructible(sc2::Point2D& pos);
		bool isNagaTower(sc2::Point2D& pos);

		std::map<sc2::Point2D, sc2::UNIT_TYPEID> neutralUnits;
	};
}

#endif /*_NEUTRAL_SET_OBJ_H_*/