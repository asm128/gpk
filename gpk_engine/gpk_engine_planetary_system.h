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
		::gpk::aeid				Cameras			;
		::gpk::eid_t			Camera			= ::gpk::EID_INVALID;
		::gpk::eid_t			Background		= ::gpk::EID_INVALID;

		::gpk::error_t			Save			(::gpk::au8 & output)	const	{ 
			gpk_necs(gpk::saveView(output, GravityCenters	));
			gpk_necs(gpk::saveView(output, Orbits			));
			gpk_necs(gpk::saveView(output, Bodies			));
			gpk_necs(gpk::saveView(output, Cameras		));
			gpk_necs(gpk::savePOD (output, Camera			));
			gpk_necs(gpk::savePOD (output, Background		));
			return 0;
		}

		::gpk::error_t			Load				(::gpk::vcu8 & input)	{ 
			gpk_necs(gpk::loadView(input, GravityCenters	));
			gpk_necs(gpk::loadView(input, Orbits			));
			gpk_necs(gpk::loadView(input, Bodies			));
			gpk_necs(gpk::loadView(input, Cameras			));
			gpk_necs(gpk::loadPOD (input, Camera			));
			gpk_necs(gpk::loadPOD (input, Background		));
			return 0;
		}
	};

	struct SEnginePlanetarySystem {
		SPlanetarySystem			PlanetarySystem;
		SPlanetarySystemEntityMap	EntityMap;

		::gpk::error_t				Save			(::gpk::au8 & output)	const	{ 
			gpk_necs(PlanetarySystem.Save(output));
			gpk_necs(EntityMap.Save(output));
			return 0;
		}

		::gpk::error_t				Load			(::gpk::vcu8 & input)	{ 
			gpk_necs(PlanetarySystem.Load(input));
			gpk_necs(EntityMap.Load(input));
			return 0;
		}
	};

	::gpk::error_t			planetarySystemReset			(const ::gpk::SPlanetarySystem & solarSystem, const ::gpk::SPlanetarySystemEntityMap & entityMap, ::gpk::SEngine & engine, const ::gpk::vc8bgra & colors);
	::gpk::error_t			planetarySystemCreateEntities	(const ::gpk::SPlanetarySystem & solarSystem, ::gpk::SPlanetarySystemEntityMap & entityMap, ::gpk::SEngine & engine);
	::gpk::error_t			planetarySystemSetup			(const ::gpk::SPlanetarySystem & solarSystem, ::gpk::SPlanetarySystemEntityMap & entityMap, ::gpk::SEngine & engine); 
	::gpk::error_t			planetarySystemSetup			(::gpk::SEnginePlanetarySystem & planetarySystem, ::gpk::SEngine & engine, const ::gpk::SJSONReader & jsonData);
	::gpk::error_t			planetarySystemSetup			(::gpk::SEnginePlanetarySystem & planetarySystem, ::gpk::SEngine & engine, ::gpk::vcc jsonFilePath);
} // namespace

#endif // GPK_ENGINE_PLANETARY_SYSTEM_H_23701
