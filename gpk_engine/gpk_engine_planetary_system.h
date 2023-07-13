#include "gpk_planetary_system.h"
#include "gpk_engine.h"
#include "gpk_json.h"
#include "gpk_view_color.h"

#ifndef GPK_ENGINE_PLANETARY_SYSTEM_H_23701
#define GPK_ENGINE_PLANETARY_SYSTEM_H_23701

namespace gpk
{
	struct SPlanetarySystemEntityMap {
		::gpk::aeid				GravityCenters	;
		::gpk::aeid				Orbits			;
		::gpk::aeid				Bodies			;
		::gpk::aeid				Rings			;
		::gpk::eid_t			Background		= ::gpk::EID_INVALID;
		::gpk::eid_t			Camera			= ::gpk::EID_INVALID;
		::gpk::eid_t			Cameras			= ::gpk::EID_INVALID;
	};

	struct SEnginePlanetarySystem {
		SPlanetarySystem			PlanetarySystem;
		SPlanetarySystemEntityMap	EntityMap;
	};

	::gpk::error_t			planetarySystemReset			(const ::gpk::SPlanetarySystem & solarSystem, const ::gpk::SPlanetarySystemEntityMap & entityMap, ::gpk::SEngine & engine, const ::gpk::vc8bgra & colors);
	::gpk::error_t			planetarySystemCreateEntities	(const ::gpk::SPlanetarySystem & solarSystem, ::gpk::SPlanetarySystemEntityMap & entityMap, ::gpk::SEngine & engine);
	::gpk::error_t			planetarySystemSetup			(const ::gpk::SPlanetarySystem & solarSystem, ::gpk::SPlanetarySystemEntityMap & entityMap, ::gpk::SEngine & engine); 
	::gpk::error_t			planetarySystemSetup			(::gpk::SEnginePlanetarySystem & planetarySystem, ::gpk::SEngine & engine, const ::gpk::SJSONReader & jsonData);
	::gpk::error_t			planetarySystemSetup			(::gpk::SEnginePlanetarySystem & planetarySystem, ::gpk::SEngine & engine, ::gpk::vcc jsonFilePath);
} // namespace

#endif // GPK_ENGINE_PLANETARY_SYSTEM_H_23701
