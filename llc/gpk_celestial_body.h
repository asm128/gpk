#include "gpk_json.h"
#include "gpk_orbit.h"
#include "gpk_ctti_enum.h"

#ifndef GPK_CELESTIAL_BODY_2379
#define GPK_CELESTIAL_BODY_2379

nmsp gpk
{
#pragma pack(push, 1)
	CTTI_ENUM(CELESTIAL_BODY,
	enum CELESTIAL_BODY : u0_t
		{ CELESTIAL_BODY_System 
		, CELESTIAL_BODY_Orbit  
		, CELESTIAL_BODY_Star   
		, CELESTIAL_BODY_Planet 
		, CELESTIAL_BODY_Moon   
	//CELESTIAL_BODY_Asteroid	
	//CELESTIAL_BODY_Comet		
		};
	);

	CTTI_ENUM(PLANET_TYPE,
	enum PLANET_TYPE : u0_t
		{ PLANET_TYPE_Dwarf		
		, PLANET_TYPE_Terrestrial	
		, PLANET_TYPE_GasGiant	
		, PLANET_TYPE_IceGiant	
		, PLANET_TYPE_MassiveSolid
		};
	);
	struct SDetailStar {
		f2_t			SurfaceGravity			= {};	// 274.0		// (eq.) (m/s2)		// 5
		f2_t			CentralPressure			= {};	// 2.477		// x 1011	bar		// 15
		f2_t			CentralDensity			= {};	// 1.622		// x 105	kg/m3	// 17
		f2_t			Luminosity				= {};	// 382.8		// (1024 J/s)		// 11
		f2_t			SurfaceEmission			= {};	// 62.94		// (106 J/m2s)		//
		u2_t			MeanDensity				= {};	// 1408		// (kg/m3)			//
		u2_t			Gm						= {};	// 132712		// (x 106 km3/s2)	// 1
		u2_t			Volume					= {};	// 1412000	// (1012 km3)		//
		u2_t			VolumetricMeanRadius	= {};	// 695700		// (km)				// 3
		f2_t			Ellipticity				= {};	// 0.00005						// 7
		f2_t			MomentOfInertia			= {};	// 0.070		// (I/MR2)			//
		f2_t			VisualMagnitude			= {};	// -26.74		// V(1,0)			// 9
		f2_t			AbsoluteMagnitude		= {};	// 4.83							//
		u2_t			MassConversionRate		= {};	// 4260		// (106 kg/s)		//
		f2_t			MeanEnergyProduction	= {};	// 0.1925		// (10-3 J/kg s)	// 13
		f2_t			CentralTemperature		= {};	// 1.571		// x 107	K		//
	};

	struct SDetailPlanet {
		f2_t			Gravity					= {};	// 3.7		// (m/s^2)		// 3
		u2_t			SurfacePressure			= {};	// 0			// (Bars)		//
		u2_t			Density					= {};	// 5427		// (kg/m3)		//
		u2_t			MeanTemperature			= {};	// 167		// (Celsius)	// 15
		u2_t			NumberOfMoons			= {};	// 0			// 				// 17
		f2_t			RadiusPolar				= {};	// (km)						//
		f2_t			RadiusEquatorial		= {};	// (km)						// 21
		f2_t			Perihelion				= {};	// 46.0		// (10^6 km)	//
		f2_t			Aphelion				= {};	// 69.8		// (10^6 km)	// 9
		f2_t			LengthOfDay				= {};	//
		b8_t			RingSystem				= {};	// false						//
		b8_t			GlobalMagneticField		= {};	// true						// 19
		PLANET_TYPE		Subtype					= {};
	};

	struct SCelestialBody : public SOrbiter {
		f2_t			Mass					= {}; // 0.330		// (10^24 kg)	//
		u2_t			Diameter				= {}; // 4879		// (km)			// 1
		f2_t			OrbitalVelocity			= {}; // 47.4		// (km/s)		// 11
		f2_t			OrbitalEccentricity		= {}; // 0.205						// 13
		f2_t			EscapeVelocity			= {}; // 4.3		// (km/s)		//
		union UBody {
			SDetailPlanet	Planet	;
			SDetailStar		Star	;
		}				Detail					= {};
	};
#pragma pack(pop)

	stxp	u2_t	s0planet				= szof(SDetailPlanet);
	stxp	u2_t	s1star					= szof(SDetailStar);

	err_t	loadStar				(SDetailStar    & body, cnst SJSONReader & jsonData, s2_t jsonIndexStellarBody);
	err_t	loadPlanet				(SDetailPlanet  & body, cnst SJSONReader & jsonData, s2_t jsonIndexStellarBody);
	err_t	loadCelestialBody		(SCelestialBody & body, CELESTIAL_BODY type, cnst SJSONReader & jsonData, s2_t jsonIndexStellarBody);
	err_t	printStar				(cnst SDetailStar    & body);
	err_t	printPlanet				(cnst SDetailPlanet  & body);
	err_t	printCelestialBody		(cnst SCelestialBody & body, CELESTIAL_BODY type);

	CELESTIAL_BODY	bodyTypeFromString		(vcst_t bodyType);
	PLANET_TYPE		bodySubtypeFromString	(vcst_t bodyType);
} // nmsp

#endif // GPK_CELESTIAL_BODY
