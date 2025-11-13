#include "gpk_planetary_system.h"

::gpk::error_t			gpk::planetarySystemSetup	(::gpk::SPlanetarySystem & planetarySystem, ::gpk::vcsc_t jsonFilePath) { 
	::gpk::SJSONFile			jsonFile					= {};
	gpk_necs(gpk::jsonFileRead(jsonFile, jsonFilePath));
	return ::gpk::planetarySystemSetup(planetarySystem, jsonFile.Reader);
}

::gpk::error_t			gpk::planetarySystemSetup	(::gpk::SPlanetarySystem & solarSystem, const ::gpk::SJSONReader & jsonData) {
	::gpk::avcsc_t				stellarBodyNames			= {};
	::gpk::as2_t					stellarBodyIndices;
	gpk_necs(gpk::jsonObjectKeyList(jsonData, 0, stellarBodyIndices, stellarBodyNames));
	const ::gpk::view<vcsc_c>	jsonView					= jsonData.View;
	::gpk::avcsc_t					bodyParentNames;

	int32_t						iFurthest					= 0;
	float						fFurthest					= 0;

	for(uint32_t iPlanet = 0; iPlanet < stellarBodyIndices.size(); ++iPlanet) {
		const int32_t				jsonIndexStellarBody		= stellarBodyIndices[iPlanet];
		int32_t						indexProperty;

		if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody + 2, "parent")) 
			gpk_necs(bodyParentNames.push_back({}));
		else
			gpk_necs(bodyParentNames.push_back(::gpk::label(jsonView[indexProperty])));
		
		// Load planet properties.
		cws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody + 2, "type")); // ?? no type? shouldn't happen

		const ::gpk::vcsc_t			strType						= jsonData.View[indexProperty]; 
		const ::gpk::CELESTIAL_BODY	bodyType					= ::gpk::bodyTypeFromString(strType);
		::gpk::SCelestialBody		bodyProperties				= {};
		cws_if_failed(::gpk::loadCelestialBody(bodyProperties, bodyType, jsonData, jsonIndexStellarBody + 2));
		if(fFurthest < bodyProperties.Orbit.Radius) {
			fFurthest				= bodyProperties.Orbit.Radius;
			iFurthest				= iPlanet;
		}
		gpk_necs(solarSystem.Type.push_back(bodyType));
		gpk_necs(solarSystem.Body.push_back(::gpk::label(stellarBodyNames[iPlanet]), bodyProperties));
	}

	for(uint32_t iPlanet = 0; iPlanet < solarSystem.Body.Keys.size(); ++iPlanet)
		gpk_necs(solarSystem.Parent.push_back(solarSystem.Body.Keys.find(bodyParentNames[iPlanet])));

	return iFurthest;
}
