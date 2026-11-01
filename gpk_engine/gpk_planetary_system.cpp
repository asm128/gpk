#include "gpk_planetary_system.h"

::gpk::err_t			gpk::planetarySystemSetup	(::gpk::SPlanetarySystem & planetarySystem, ::gpk::view<sc_t> jsonFilePath) { 
	::gpk::SJSONFile			jsonFile					= {};
	if_fail_fe(gpk::jsonFileRead(jsonFile, jsonFilePath));
	return ::gpk::planetarySystemSetup(planetarySystem, jsonFile.Reader);
}

::gpk::err_t			gpk::planetarySystemSetup	(::gpk::SPlanetarySystem & solarSystem, const ::gpk::SJSONReader & jsonData) {
	::gpk::avcsc_t				stellarBodyNames			= {};
	::gpk::as2_t					stellarBodyIndices;
	if_fail_fe(gpk::jsonObjectKeyList(jsonData, 0, stellarBodyIndices, stellarBodyNames));
	const ::gpk::view<view<sc_c>>	jsonView					= jsonData.View;
	::gpk::avcsc_t					bodyParentNames;

	s2_t						iFurthest					= 0;
	float						fFurthest					= 0;

	for(u2_t iPlanet = 0; iPlanet < stellarBodyIndices.size(); ++iPlanet) {
		const s2_t				jsonIndexStellarBody		= stellarBodyIndices[iPlanet];
		s2_t						indexProperty;

		if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody + 2, "parent")) 
			if_fail_fe(bodyParentNames.push_back({}));
		else
			if_fail_fe(bodyParentNames.push_back(::gpk::label(jsonView[indexProperty])));
		
		// Load planet properties.
		if_fail_cw(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody + 2, "type")); // ?? no type? shouldn't happen

		const ::gpk::view<sc_t>			strType						= jsonData.View[indexProperty]; 
		const ::gpk::CELESTIAL_BODY	bodyType					= ::gpk::bodyTypeFromString(strType);
		::gpk::SCelestialBody		bodyProperties				= {};
		if_fail_cw(::gpk::loadCelestialBody(bodyProperties, bodyType, jsonData, jsonIndexStellarBody + 2));
		if(fFurthest < bodyProperties.Orbit.Radius) {
			fFurthest				= bodyProperties.Orbit.Radius;
			iFurthest				= iPlanet;
		}
		if_fail_fe(solarSystem.Type.push_back(bodyType));
		if_fail_fe(solarSystem.Body.push_back(::gpk::label(stellarBodyNames[iPlanet]), bodyProperties));
	}

	for(u2_t iPlanet = 0; iPlanet < solarSystem.Body.Keys.size(); ++iPlanet)
		if_fail_fe(solarSystem.Parent.push_back(solarSystem.Body.Keys.find(bodyParentNames[iPlanet])));

	return iFurthest;
}
