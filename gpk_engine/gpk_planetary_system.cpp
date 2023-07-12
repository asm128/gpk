#include "gpk_planetary_system.h"
#include "gpk_stdstring.h"

::gpk::CELESTIAL_BODY	gpk::bodyTypeFromString		(::gpk::vcc bodyType) { 
	if(2 > bodyType.size())
		return (::gpk::CELESTIAL_BODY)-1;

	::gpk::achar				typeCased					= bodyType;
	const ::gpk::vcvcc			labels						= ::gpk::get_value_labels<::gpk::CELESTIAL_BODY>();
	::gpk::toupper(typeCased[0]);
	::gpk::tolower(::gpk::vc{&typeCased[1], typeCased.size() - 1});
	for(uint32_t iType = 0; iType < labels.size(); ++iType) {
		if(labels[iType] == typeCased)
			return ::gpk::get_enum<::gpk::CELESTIAL_BODY>().Values[iType];
	}
	return (::gpk::CELESTIAL_BODY)-1;
}
::gpk::error_t			gpk::planetarySystemSetup	(::gpk::SPlanetarySystem & planetarySystem, ::gpk::vcc jsonFilePath) { 
	::gpk::SJSONFile			jsonFile					= {};
	gpk_necs(::gpk::jsonFileRead(jsonFile, jsonFilePath), "%s", ::gpk::toString(jsonFilePath).begin());
	return ::gpk::planetarySystemSetup(planetarySystem, jsonFile.Reader);
}

::gpk::error_t			gpk::planetarySystemSetup	(::gpk::SPlanetarySystem & solarSystem, const ::gpk::SJSONReader & jsonData) {
	::gpk::avcc					stellarBodyNames			= {};
	::gpk::ai32					stellarBodyIndices;
	gpk_necs(::gpk::jsonObjectKeyList(jsonData, 0, stellarBodyIndices, stellarBodyNames));
	const ::gpk::vcvcc			jsonView					= jsonData.View;
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
		
		// Load planet properties.
		cws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody + 2, "type")); // ?? no type? shouldn't happen

		const ::gpk::vcc			strType						= jsonData.View[indexProperty]; 
		const ::gpk::CELESTIAL_BODY	bodyType					= ::gpk::bodyTypeFromString(strType);
		::gpk::SCelestialBody		bodyProperties				= {};
		cws_if_failed(::gpk::loadCelestialBody(bodyProperties, bodyType, jsonData, jsonIndexStellarBody + 2));
		if(fFurthest < bodyProperties.OrbitRadius) {
			fFurthest				= bodyProperties.OrbitRadius;
			iFurthest				= iPlanet;
		}
		gpk_necs(solarSystem.Type.push_back(bodyType));
		gpk_necs(solarSystem.Body.push_back(::gpk::label(stellarBodyNames[iPlanet]), bodyProperties));
	}

	for(uint32_t iPlanet = 0; iPlanet < solarSystem.Body.Keys.size(); ++iPlanet)
		gpk_necs(solarSystem.Parent.push_back(solarSystem.Body.Keys.find(bodyParentNames[iPlanet])));

	return iFurthest;
}
