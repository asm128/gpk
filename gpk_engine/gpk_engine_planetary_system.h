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

	::gpk::error_t			planetarySystemSetup		(::gpk::SPlanetarySystem & planetarySystem, ::gpk::SEngine & engine, const ::gpk::SJSONReader & jsonData);
	::gpk::error_t			planetarySystemSetup		(::gpk::SPlanetarySystem & planetarySystem, ::gpk::SEngine & engine, ::gpk::vcc jsonFilePath);
} // namespace

#endif // GPK_PLANETARY_SYSTEM_H_23701
