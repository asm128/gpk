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
		::gpk::ai32							Entity;
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


} // namespace

#endif // GPK_PLANETARY_SYSTEM_H_23701
