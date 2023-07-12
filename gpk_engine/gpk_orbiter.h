#include "gpk_json.h"

#ifndef GPK_ORBTIER
#define GPK_ORBTIER

namespace gpk
{
#pragma pack(push, 1)
	struct SOrbit {
		float		OrbitRadius			= {}; // 57.9	// (10^6 km)	// 7
		float		OrbitalPeriod		= {}; // 88.0	// (days)		//
		float		OrbitalInclination	= {}; // 7.0	// (degrees)	//
	};

	struct SOrbiter : public SOrbit {
		float		ObliquityToOrbit	= {}; // 0.034	// (degrees)	//
		float		RotationPeriod		= {}; // 1407.6	// (hours)		// 5
		float		LengthOfDay			= {}; // 4222.6		// (hours)		//
	};
#pragma pack(pop)
} // namespace

#endif // GPK_ORBTIER