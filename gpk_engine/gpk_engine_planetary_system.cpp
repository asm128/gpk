#include "gpk_engine_planetary_system.h"
#include "gpk_stdstring.h"
#include "gpk_json_expression.h"
#include "gpk_engine_shader.h"

//static	::gpk::error_t	setupPlanet		(::gpk::SSolarSystem & /*solarSystem*/, ::gpk::SEngine & /*engine*/, const ::gpk::SJSONReader & /*jsonData*/) { return 0; }
//static	::gpk::error_t	setupStar		(::gpk::SSolarSystem & /*solarSystem*/, ::gpk::SEngine & /*engine*/, const ::gpk::SJSONReader & /*jsonData*/) { return 0; }

::gpk::error_t			gpk::planetarySystemSetup	(::gpk::SPlanetarySystem & solarSystem, const ::gpk::SJSONReader & jsonData) {
	::gpk::avcc					stellarBodyNames			= {};
	::gpk::ai32					stellarBodyIndices;
	gpk_necs(::gpk::jsonObjectKeyList(jsonData, 0, stellarBodyIndices, stellarBodyNames));
	::gpk::vcvcc				jsonView					= jsonData.View;
	::gpk::avcc					bodyParentNames;

	int32_t						iFurthest					= 0;
	float						fFurthest					= 0;

	for(uint32_t iPlanet = 0; iPlanet < stellarBodyIndices.size(); ++iPlanet) {
		const int32_t				jsonIndexStellarBody		= stellarBodyIndices[iPlanet];
		int32_t						indexProperty;

		if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody + 2, "parent")) 
			gpk_necs(bodyParentNames.push_back({}));
		else
			gpk_necs(bodyParentNames.push_back(jsonView[indexProperty]));
		
		{ // Load planet properties.
			::gpk::CELESTIAL_BODY		bodyType					= ::gpk::CELESTIAL_BODY_Planet;
			ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody + 2, "type")) // ?? no type? shouldn't happen
			else {
				::gpk::vcc					strType						= jsonData.View[indexProperty]; 
				bodyType				= (::gpk::vcs("star") == strType) ? ::gpk::CELESTIAL_BODY_Star : ::gpk::CELESTIAL_BODY_Planet;
			}

#define CASE_LOAD(Token)					\
	case ::gpk::CELESTIAL_BODY_##Token:		\
		ws_if_failed(load##Token(bodyProperties.Detail.Token, jsonData, jsonIndexStellarBody + 2)); print##Token(bodyProperties.Detail.Token);	\
		break;

			::gpk::SCelestialBody		bodyProperties				= {};
			switch(bodyType) {
			CASE_LOAD(Star  ); 
			CASE_LOAD(Planet); 
			}
			if(fFurthest < bodyProperties.Detail.Planet.DistanceFromSun) {
				fFurthest				= bodyProperties.Detail.Planet.DistanceFromSun;
				iFurthest				= iPlanet;
			}
			gpk_necs(solarSystem.Type.push_back(bodyType));
			gpk_necs(solarSystem.Body.push_back(stellarBodyNames[iPlanet], bodyProperties));
		}
	}

	for(uint32_t iPlanet = 0; iPlanet < solarSystem.Body.Keys.size(); ++iPlanet)
		gpk_necs(solarSystem.Parent.push_back(solarSystem.Body.Keys.find(bodyParentNames[iPlanet])));

	return iFurthest;
}

//static	::gpk::error_t	setupSolarSystemEntities	(::gpk::SPlanetarySystem & solarSystem, ::gpk::SEngine & engine) {
//	{	// planets
//		::gpk::SParamsSphere		params			= {};
//		params.CellCount		= {24, 24};
//		//params.Origin			= {};
//		params.Origin			= {0, .5};
//
//		params.Radius			= .5f;
//		params.Reverse			= false;
//		params.DiameterRatio	= 1.f;
//
//		gpk_necs(solarSystem.EntityBody.push_back(engine.CreateSphere(params)));
//	}
//
//	gpk_necs(engine.SetShader(solarSystem.EntityBody[0], ::gpk::psSphereAxis, "psSphereAxis"));
//	for(uint32_t iOrbiter = 1; iOrbiter < solarSystem.Body.size(); ++iOrbiter)
//		gpk_necs(solarSystem.EntityBody.push_back(engine.Clone(solarSystem.EntityBody[0], true, true, true)));
//
//	{	// orbits circles
//		::gpk::SParamsCircle		params	= {};
//		params.Slices			= 24;
//		params.Origin			= {};
//		params.Radius			= .5f;
//		params.Reverse			= false;
//		params.DiameterRatio	= 1.f;
//		gpk_necs(solarSystem.EntityOrbit.push_back(engine.CreateCircle(params)));
//	}
//
//	gpk_necs(engine.SetShader(solarSystem.EntityOrbit[0], ::gpk::psGridRuler, "psGridRuler"));
//	for(uint32_t iOrbiter = 1; iOrbiter < solarSystem.Body.size(); ++iOrbiter)
//		gpk_necs(solarSystem.EntityOrbit.push_back(engine.Clone(solarSystem.EntityOrbit[0], true, true, true)));
//
//	return 0;
//}

::gpk::error_t			gpk::planetarySystemSetup	(::gpk::SPlanetarySystem & solarSystem, ::gpk::SEngine & engine) {
	int32_t						iFurthest					= solarSystem.Body.Values.max<float>([](const ::gpk::SCelestialBody & body){ return body.Detail.Planet.DistanceFromSun; });
	for(uint32_t iPlanet = 0; iPlanet < solarSystem.Body.Keys.size(); ++iPlanet) {
		const ::gpk::SCelestialBody	& bodyData					= solarSystem.Body.Values[iPlanet];
		const ::gpk::error_t		iEntity						= engine.CreateOrbiter
			( bodyData.Detail.Planet.Diameter
			, bodyData.Detail.Planet.OrbitalInclination
			, bodyData.Detail.Planet.OrbitalPeriod
			, bodyData.Detail.Planet.Mass
			, bodyData.Detail.Planet.DistanceFromSun
			, 1.0 / solarSystem.Body.Values[iFurthest].Detail.Planet.DistanceFromSun * 2500
			, bodyData.Detail.Planet.ObliquityToOrbit
			, bodyData.Detail.Planet.LengthOfDay
			, 24
			);
		gpk_necs(solarSystem.EntityOrbit.push_back(iEntity));
	}
	return 0;
}
