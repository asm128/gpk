#include "gpk_engine_planetary_system.h"
#include "gpk_stdstring.h"
#include "gpk_json_expression.h"

//static	::gpk::error_t	setupPlanet		(::gpk::SSolarSystem & /*solarSystem*/, ::gpk::SEngine & /*engine*/, const ::gpk::SJSONReader & /*jsonData*/) { return 0; }
//static	::gpk::error_t	setupStar		(::gpk::SSolarSystem & /*solarSystem*/, ::gpk::SEngine & /*engine*/, const ::gpk::SJSONReader & /*jsonData*/) { return 0; }

::gpk::error_t			gpk::planetarySystemSetup	(::gpk::SPlanetarySystem & solarSystem, ::gpk::SEngine & engine, const ::gpk::SJSONReader & jsonData) {
	::gpk::avcc					stellarBodyNames			= {};
	::gpk::ai32					stellarBodyIndices;
	gpk_necs(::gpk::jsonObjectKeyList(jsonData, 0, stellarBodyIndices, stellarBodyNames));
	::gpk::vcvcc				jsonView					= jsonData.View;
	::gpk::avcc					bodyParentNames;

	float						furthest					= 0;

	for(uint32_t iPlanet = 0; iPlanet < stellarBodyIndices.size(); ++iPlanet) {
		const int32_t				jsonIndexStellarBody		= stellarBodyIndices[iPlanet];
		int32_t						indexProperty;

		if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody + 2, "parent")) 
			gpk_necs(bodyParentNames.push_back({}));
		else
			gpk_necs(bodyParentNames.push_back(jsonView[indexProperty]));
		
		{ // Load planet properties.
			::gpk::CELESTIAL_BODY		bodyType					= ::gpk::CELESTIAL_BODY_PLANET;
			ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody + 2, "type")) // ?? no type? shouldn't happen
			else {
				::gpk::vcc					strType						= jsonData.View[indexProperty]; 
				bodyType				= (::gpk::vcs("star") == strType) ? ::gpk::CELESTIAL_BODY_STAR : ::gpk::CELESTIAL_BODY_PLANET;
			}

			::gpk::SCelestialBody		bodyProperties				= {};
			switch(bodyType) {
			case ::gpk::CELESTIAL_BODY_STAR		: ws_if_failed(loadStar  (bodyProperties.Detail.Star  , jsonData, jsonIndexStellarBody + 2)); printStar  (bodyProperties.Detail.Star  ); break;
			case ::gpk::CELESTIAL_BODY_PLANET	: ws_if_failed(loadPlanet(bodyProperties.Detail.Planet, jsonData, jsonIndexStellarBody + 2)); printPlanet(bodyProperties.Detail.Planet); break;
			}
			furthest				= ::gpk::max(furthest, bodyProperties.Detail.Planet.DistanceFromSun);
			gpk_necs(solarSystem.Type.push_back(bodyType));
			gpk_necs(solarSystem.Body.push_back(stellarBodyNames[iPlanet], bodyProperties));
		}
	}

	for(uint32_t iPlanet = 0; iPlanet < solarSystem.Body.Keys.size(); ++iPlanet)
		gpk_necs(solarSystem.Parent.push_back(solarSystem.Body.Keys.find(bodyParentNames[iPlanet])));

	for(uint32_t iPlanet = 0; iPlanet < solarSystem.Body.Keys.size(); ++iPlanet) {
		const ::gpk::SCelestialBody	& bodyData					= solarSystem.Body.Values[iPlanet];
		const ::gpk::error_t		iEntity						= engine.CreateOrbiter
			( bodyData.Detail.Planet.Diameter
			, bodyData.Detail.Planet.Mass
			, bodyData.Detail.Planet.DistanceFromSun
			, bodyData.Detail.Planet.ObliquityToOrbit
			, bodyData.Detail.Planet.LengthOfDay
			, 24
			, bodyData.Detail.Planet.OrbitalPeriod
			, bodyData.Detail.Planet.OrbitalInclination
			, 1.0 / furthest * 2500
			);
		gpk_necs(solarSystem.Entity.push_back(iEntity));
	}
	return 0;
}

::gpk::error_t			gpk::planetarySystemSetup	(::gpk::SPlanetarySystem & solarSystem, ::gpk::SEngine & engine, ::gpk::vcc jsonFilePath) {
	::gpk::SJSONFile			jsonFile					= {};
	gpk_necs(::gpk::jsonFileRead(jsonFile, jsonFilePath));
	return ::gpk::planetarySystemSetup(solarSystem, engine, jsonFile.Reader);
}
