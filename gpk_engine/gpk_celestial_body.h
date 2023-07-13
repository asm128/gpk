#include "gpk_json.h"
#include "gpk_orbit.h"

#ifndef GPK_CELESTIAL_BODY_2379
#define GPK_CELESTIAL_BODY_2379

namespace gpk
{
#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE(CELESTIAL_BODY, uint8_t);
	GDEFINE_ENUM_VALUE(CELESTIAL_BODY, System	, 0);
	GDEFINE_ENUM_VALUE(CELESTIAL_BODY, Orbit	, 1);
	GDEFINE_ENUM_VALUE(CELESTIAL_BODY, Star		, 2);
	GDEFINE_ENUM_VALUE(CELESTIAL_BODY, Planet	, 3);
	GDEFINE_ENUM_VALUE(CELESTIAL_BODY, Moon		, 4);
	//GDEFINE_ENUM_VALUE(CELESTIAL_BODY, Asteroid	, 5);
	//GDEFINE_ENUM_VALUE(CELESTIAL_BODY, Comet		, 6);

	GDEFINE_ENUM_TYPE (PLANET_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(PLANET_TYPE, Dwarf		, 0);
	GDEFINE_ENUM_VALUE(PLANET_TYPE, Terrestrial	, 1);
	GDEFINE_ENUM_VALUE(PLANET_TYPE, GasGiant	, 2);
	GDEFINE_ENUM_VALUE(PLANET_TYPE, IceGiant	, 3);
	GDEFINE_ENUM_VALUE(PLANET_TYPE, MassiveSolid, 4);

	struct SDetailStar {
		float			SurfaceGravity			= {};	// 274.0		// (eq.) (m/s2)		// 5
		float			CentralPressure			= {};	// 2.477		// x 1011	bar		// 15
		float			CentralDensity			= {};	// 1.622		// x 105	kg/m3	// 17
		float			Luminosity				= {};	// 382.8		// (1024 J/s)		// 11
		float			SurfaceEmission			= {};	// 62.94		// (106 J/m2s)		//
		uint32_t		MeanDensity				= {};	// 1408		// (kg/m3)			//
		uint32_t		Gm						= {};	// 132712		// (x 106 km3/s2)	// 1
		uint32_t		Volume					= {};	// 1412000	// (1012 km3)		//
		uint32_t		VolumetricMeanRadius	= {};	// 695700		// (km)				// 3
		float			Ellipticity				= {};	// 0.00005						// 7
		float			MomentOfInertia			= {};	// 0.070		// (I/MR2)			//
		float			VisualMagnitude			= {};	// -26.74		// V(1,0)			// 9
		float			AbsoluteMagnitude		= {};	// 4.83							//
		uint32_t		MassConversionRate		= {};	// 4260		// (106 kg/s)		//
		float			MeanEnergyProduction	= {};	// 0.1925		// (10-3 J/kg s)	// 13
		float			CentralTemperature		= {};	// 1.571		// x 107	K		//
	};

	struct SDetailPlanet {
		float			Gravity					= {};	// 3.7		// (m/s^2)		// 3
		uint32_t		SurfacePressure			= {};	// 0			// (Bars)		//
		uint32_t		Density					= {};	// 5427		// (kg/m3)		//
		uint32_t		MeanTemperature			= {};	// 167		// (Celsius)	// 15
		uint32_t		NumberOfMoons			= {};	// 0			// 				// 17
		float			RadiusPolar				= {};	// (km)						//
		float			RadiusEquatorial		= {};	// (km)						// 21
		float			Perihelion				= {};	// 46.0		// (10^6 km)	//
		float			Aphelion				= {};	// 69.8		// (10^6 km)	// 9
		float			LengthOfDay				= {};	//
		bool			RingSystem				= {};	// false						//
		bool			GlobalMagneticField		= {};	// true						// 19
		PLANET_TYPE		Subtype					= {};
	};

	struct SCelestialBody : public SOrbiter {
		float			Mass					= {}; // 0.330		// (10^24 kg)	//
		uint32_t		Diameter				= {}; // 4879		// (km)			// 1
		float			OrbitalVelocity			= {}; // 47.4		// (km/s)		// 11
		float			OrbitalEccentricity		= {}; // 0.205						// 13
		float			EscapeVelocity			= {}; // 4.3		// (km/s)		//
		union UBody {
			SDetailPlanet	Planet	;
			SDetailStar		Star	;
		}				Detail					= {};
	};
#pragma pack(pop)

	stacxpr	size_t	s0						= sizeof(SDetailPlanet);
	stacxpr	size_t	s1						= sizeof(SDetailStar);

	::gpk::error_t	loadStar				(::gpk::SDetailStar    & body, const ::gpk::SJSONReader & jsonData, int32_t jsonIndexStellarBody);
	::gpk::error_t	loadPlanet				(::gpk::SDetailPlanet  & body, const ::gpk::SJSONReader & jsonData, int32_t jsonIndexStellarBody);
	::gpk::error_t	loadCelestialBody		(::gpk::SCelestialBody & body, ::gpk::CELESTIAL_BODY type, const ::gpk::SJSONReader & jsonData, int32_t jsonIndexStellarBody);
	::gpk::error_t	printStar				(const ::gpk::SDetailStar    & body);
	::gpk::error_t	printPlanet				(const ::gpk::SDetailPlanet  & body);
	::gpk::error_t	printCelestialBody		(const ::gpk::SCelestialBody & body, ::gpk::CELESTIAL_BODY type);

	CELESTIAL_BODY	bodyTypeFromString		(::gpk::vcc bodyType);
	PLANET_TYPE		bodySubtypeFromString	(::gpk::vcc bodyType);
} // namespace

#endif // GPK_CELESTIAL_BODY