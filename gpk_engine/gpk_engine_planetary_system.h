#include "gpk_engine_celestial_body.h"
#include "gpk_engine.h"
#include "gpk_json.h"

#ifndef GPK_PLANETARY_SYSTEM_H_23701
#define GPK_PLANETARY_SYSTEM_H_23701

namespace gpk
{
	struct SPlanetarySystem {
		::gpk::SCelestialBodyMap			Body;
		::gpk::apod<::gpk::CELESTIAL_BODY>	Type;
		::gpk::ai32							Parent;
		::gpk::aeid							EntityOrbit;
		::gpk::aeid							EntityBody;
	};

	::gpk::error_t			planetarySystemSetup	(::gpk::SPlanetarySystem & planetarySystem, ::gpk::SEngine & engine);
	::gpk::error_t			planetarySystemSetup	(::gpk::SPlanetarySystem & planetarySystem, const ::gpk::SJSONReader & jsonData);
	stainli	::gpk::error_t	planetarySystemSetup	(::gpk::SPlanetarySystem & planetarySystem, ::gpk::vcc jsonFilePath) { 
		::gpk::SJSONFile			jsonFile				= {};
		gpk_necs(::gpk::jsonFileRead(jsonFile, jsonFilePath), "%s", ::gpk::toString(jsonFilePath).begin());
		return ::gpk::planetarySystemSetup(planetarySystem, jsonFile.Reader);
	}

	stainli	::gpk::error_t	planetarySystemSetup	(::gpk::SPlanetarySystem & planetarySystem, ::gpk::SEngine & engine, const ::gpk::SJSONReader & jsonData)	{ gpk_necall(::gpk::planetarySystemSetup(planetarySystem, jsonData    ), "%s", ::gpk::toString(jsonData.View[0]).begin());	return ::gpk::planetarySystemSetup(planetarySystem, engine); }
	stainli	::gpk::error_t	planetarySystemSetup	(::gpk::SPlanetarySystem & planetarySystem, ::gpk::SEngine & engine, ::gpk::vcc jsonFilePath)				{ gpk_necall(::gpk::planetarySystemSetup(planetarySystem, jsonFilePath), "%s", jsonFilePath.begin());						return ::gpk::planetarySystemSetup(planetarySystem, engine); }

	stainli	::gpk::error_t	initOrbiterOrbit		(const ::gpk::SCelestialBody & bodyData, ::gpk::SBodyCenter	& orbitCenter, ::gpk::SBodyForces & orbitForces) {
		return ::gpk::initOrbiterOrbit(orbitCenter, orbitForces, bodyData.Detail.Planet.OrbitalInclination, bodyData.Detail.Planet.OrbitalPeriod);
	}

	stainli	::gpk::error_t	initOrbiterBody
		( const SCelestialBody	& bodyData
		, float					distanceScale
		, float					rotationUnit
		, ::gpk::SBodyCenter	& planetCenter
		, ::gpk::SBodyForces	& planetForces
		, ::gpk::SBodyMass		& planetMass
		) {
		return ::gpk::initOrbiterBody
			( planetCenter
			, planetForces
			, planetMass
			, bodyData.Detail.Planet.Mass
			, bodyData.Detail.Planet.ObliquityToOrbit
			, bodyData.Detail.Planet.DistanceFromSun
			, distanceScale
			, bodyData.Detail.Planet.RotationPeriod
			, rotationUnit
			);
	}


	stainli	::gpk::error_t	initOrbiterOrbit		(const SCelestialBody & bodyData, ::gpk::SRigidBodyIntegrator & integrator, int32_t rigidBodyIndex) {
		return ::gpk::initOrbiterOrbit(integrator.Centers[rigidBodyIndex], integrator.Forces [rigidBodyIndex], bodyData.Detail.Planet.OrbitalInclination, bodyData.Detail.Planet.OrbitalPeriod);
	}

	stainli	::gpk::error_t	initOrbiterBody
		( const SCelestialBody			& bodyData
		, ::gpk::SRigidBodyIntegrator	& integrator
		, int32_t						rigidBodyIndex
		, float							distanceScale
		, float							rotationUnit
		) {
		return ::gpk::initOrbiterBody
			( integrator.Centers[rigidBodyIndex]
			, integrator.Forces [rigidBodyIndex]
			, integrator.Masses [rigidBodyIndex]
			, bodyData.Detail.Planet.Mass
			, bodyData.Detail.Planet.ObliquityToOrbit
			, bodyData.Detail.Planet.DistanceFromSun
			, distanceScale
			, bodyData.Detail.Planet.RotationPeriod
			, rotationUnit
			);
	}
} // namespace

#endif // GPK_PLANETARY_SYSTEM_H_23701
