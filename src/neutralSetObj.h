#ifndef _NEUTRAL_SET_OBJ_H_
#define _NEUTRAL_SET_OBJ_H_

namespace Overseer{

	struct NeutralImpl{
		NeutralImpl();

		bool isNeutral(ObservationInterface* obs);
		bool isMineral();
		bool isGas();
		bool isDestructible();
		bool isNagaTower();

		std::map<sc2::Point2D, sc2::UNIT_TYPEID> neutralUnits;
	};
}

#endif /*_NEUTRAL_SET_OBJ_H_*/