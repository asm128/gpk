#include "gpk_planetary_system.h"

::gpk::error_t			gpk::planetarySystemSetup	(::gpk::SPlanetarySystem & planetarySystem, ::gpk::vcc jsonFilePath) { 
	::gpk::SJSONFile			jsonFile					= {};
	gpk_necs(::gpk::jsonFileRead(jsonFile, jsonFilePath), "%s", ::gpk::toString(jsonFilePath).begin());
	return ::gpk::planetarySystemSetup(planetarySystem, jsonFile.Reader);
}

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
			gpk_necs(bodyParentNames.push_back(::gpk::label(jsonView[indexProperty])));
		
		{ // Load planet properties.
			::gpk::CELESTIAL_BODY		bodyType					= ::gpk::CELESTIAL_BODY_Planet;
			cws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody + 2, "type")) // ?? no type? shouldn't happen
			else {
				::gpk::vcc					strType						= jsonData.View[indexProperty]; 
				bodyType				= (::gpk::vcs("star") == strType) ? ::gpk::CELESTIAL_BODY_Star : (::gpk::vcs("moon") == strType) ? ::gpk::CELESTIAL_BODY_Moon : ::gpk::CELESTIAL_BODY_Planet;
			}

			::gpk::SCelestialBody		bodyProperties				= {};
			cws_if_failed(::gpk::loadCelestialBody(bodyProperties, bodyType, jsonData, jsonIndexStellarBody + 2));
			if(fFurthest < bodyProperties.DistanceFromParent) {
				fFurthest				= bodyProperties.DistanceFromParent;
				iFurthest				= iPlanet;
			}
			gpk_necs(solarSystem.Type.push_back(bodyType));
			gpk_necs(solarSystem.Body.push_back(::gpk::label(stellarBodyNames[iPlanet]), bodyProperties));
		}
	}

	for(uint32_t iPlanet = 0; iPlanet < solarSystem.Body.Keys.size(); ++iPlanet)
		gpk_necs(solarSystem.Parent.push_back(solarSystem.Body.Keys.find(bodyParentNames[iPlanet])));

	return iFurthest;
}
