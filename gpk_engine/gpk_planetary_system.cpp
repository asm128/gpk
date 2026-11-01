#include "gpk_planetary_system.h"

gpk::err_t 		gpk::planetarySystemSetup	(SPlanetarySystem & planetarySystem, vcst_t jsonFilePath) { 
	SJSONFile			jsonFile					= {};
	if_fail_fe(gpk::jsonFileRead(jsonFile, jsonFilePath));
	return planetarySystemSetup(planetarySystem, jsonFile.Reader);
}

gpk::err_t 		gpk::planetarySystemSetup	(SPlanetarySystem & solarSystem, const SJSONReader & jsonData) {
	aobj<vcst_t>		stellarBodyNames			= {};
	as2_t				stellarBodyIndices;
	if_fail_fe(gpk::jsonObjectKeyList(jsonData, 0, stellarBodyIndices, stellarBodyNames));
	view<vcst_c>		jsonView					= jsonData.View;
	aobj<vcst_t>		bodyParentNames;

	s2_t						iFurthest					= 0;
	f2_t						fFurthest					= 0;

	for(u2_t iPlanet = 0; iPlanet < stellarBodyIndices.size(); ++iPlanet) {
		const s2_t				jsonIndexStellarBody		= stellarBodyIndices[iPlanet];
		s2_t						indexProperty;

		if_failed(indexProperty = jsonObjectValueGet(jsonData, jsonIndexStellarBody + 2, "parent")) {
			if_fail_fe(bodyParentNames.push_back({}));
		}
		else
			if_fail_fe(bodyParentNames.push_back(label(jsonView[indexProperty])));
		
		// Load planet properties.
		if_fail_cw(indexProperty = jsonObjectValueGet(jsonData, jsonIndexStellarBody + 2, "type")); // ?? no type? shouldn't happen

		const vcst_t			strType						= jsonData.View[indexProperty]; 
		const CELESTIAL_BODY	bodyType					= bodyTypeFromString(strType);
		SCelestialBody		bodyProperties				= {};
		if_fail_cw(loadCelestialBody(bodyProperties, bodyType, jsonData, jsonIndexStellarBody + 2));
		if(fFurthest < bodyProperties.Orbit.Radius) {
			fFurthest				= bodyProperties.Orbit.Radius;
			iFurthest				= iPlanet;
		}
		if_fail_fe(solarSystem.Type.push_back(bodyType));
		if_fail_fe(solarSystem.Body.push_back(label(stellarBodyNames[iPlanet]), bodyProperties));
	}

	for(u2_t iPlanet = 0; iPlanet < solarSystem.Body.Keys.size(); ++iPlanet) {
		if_fail_fe(solarSystem.Parent.push_back(solarSystem.Body.Keys.find(bodyParentNames[iPlanet])));
	}
	return iFurthest;
}
