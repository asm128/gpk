#include "gpk_celestial_body.h"
#include "gpk_linear_map_pod.h"

#include "gpk_json.h"

#ifndef GPK_PLANETARY_SYSTEM_H_23701
#define GPK_PLANETARY_SYSTEM_H_23701

namespace gpk
{
	typedef	::gpk::lmpod<::gpk::vcc, ::gpk::SCelestialBody>	SCelestialBodyMap;

	struct SPlanetarySystem {
		::gpk::SCelestialBodyMap			Body;
		::gpk::apod<::gpk::CELESTIAL_BODY>	Type;
		::gpk::ai32							Parent;
	};

	::gpk::error_t			planetarySystemSetup	(::gpk::SPlanetarySystem & planetarySystem, const ::gpk::SJSONReader & jsonData);
	::gpk::error_t			planetarySystemSetup	(::gpk::SPlanetarySystem & planetarySystem, ::gpk::vcc jsonFilePath);
} // namespace

#endif // GPK_PLANETARY_SYSTEM_H_23701
