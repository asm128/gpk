#include "gpk_planetary_system.h"
#include "gpk_engine.h"
#include "gpk_json.h"
#include "gpk_view_color.h"

#ifndef GPK_ENGINE_PLANETARY_SYSTEM_H_23701
#define GPK_ENGINE_PLANETARY_SYSTEM_H_23701

namespace gpk
{
	struct SPlanetarySystemEntityMap {
		::gpk::aeid					GravityCenters;
		::gpk::aeid					Orbits;
		::gpk::aeid					Bodies;
	};

	struct SEnginePlanetarySystem {
		SPlanetarySystem			PlanetarySystem;
		SPlanetarySystemEntityMap	EntityMap;
	};

	::gpk::error_t			planetarySystemReset			(const ::gpk::SPlanetarySystem & solarSystem, const ::gpk::SPlanetarySystemEntityMap & entityMap, ::gpk::SEngine & engine, const ::gpk::vc8bgra & colors);
	::gpk::error_t			planetarySystemCreateEntities	(const ::gpk::SPlanetarySystem & solarSystem, ::gpk::SPlanetarySystemEntityMap & entityMap, ::gpk::SEngine & engine);
	::gpk::error_t			planetarySystemSetup			(const ::gpk::SPlanetarySystem & solarSystem, ::gpk::SPlanetarySystemEntityMap & entityMap, ::gpk::SEngine & engine); 

	stainli	::gpk::error_t	planetarySystemSetup			(::gpk::SEnginePlanetarySystem & planetarySystem, ::gpk::SEngine & engine, const ::gpk::SJSONReader & jsonData)	{ gpk_necall(::gpk::planetarySystemSetup(planetarySystem.PlanetarySystem, jsonData    ), "%s", ::gpk::toString(jsonData.View[0]).begin());	return ::gpk::planetarySystemSetup(planetarySystem.PlanetarySystem, planetarySystem.EntityMap, engine); }
	stainli	::gpk::error_t	planetarySystemSetup			(::gpk::SEnginePlanetarySystem & planetarySystem, ::gpk::SEngine & engine, ::gpk::vcc jsonFilePath)				{ gpk_necall(::gpk::planetarySystemSetup(planetarySystem.PlanetarySystem, jsonFilePath), "%s", jsonFilePath.begin());						return ::gpk::planetarySystemSetup(planetarySystem.PlanetarySystem, planetarySystem.EntityMap, engine); }
} // namespace

#endif // GPK_ENGINE_PLANETARY_SYSTEM_H_23701
