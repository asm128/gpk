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

		::gpk::err_t			Save			(::gpk::au0_t & output)	const	{ 
			if_fail_fe(gpk::saveView(output, GravityCenters	));
			if_fail_fe(gpk::saveView(output, Orbits			));
			if_fail_fe(gpk::saveView(output, Bodies			));
			if_fail_fe(gpk::saveView(output, Cameras		));
			if_fail_fe(gpk::savePOD (output, Camera			));
			if_fail_fe(gpk::savePOD (output, Background		));
			return 0;
		}

		::gpk::err_t			Load				(::gpk::vcu0_t & input)	{ 
			if_fail_fe(gpk::loadView(input, GravityCenters	));
			if_fail_fe(gpk::loadView(input, Orbits			));
			if_fail_fe(gpk::loadView(input, Bodies			));
			if_fail_fe(gpk::loadView(input, Cameras			));
			if_fail_fe(gpk::loadPOD (input, Camera			));
			if_fail_fe(gpk::loadPOD (input, Background		));
			return 0;
		}
	};

	struct SEnginePlanetarySystem {
		SPlanetarySystem			PlanetarySystem;
		SPlanetarySystemEntityMap	EntityMap;

		::gpk::err_t				Save			(::gpk::au0_t & output)	const	{ 
			if_fail_fe(PlanetarySystem.Save(output));
			if_fail_fe(EntityMap.Save(output));
			return 0;
		}

		::gpk::err_t				Load			(::gpk::vcu0_t & input)	{ 
			if_fail_fe(PlanetarySystem.Load(input));
			if_fail_fe(EntityMap.Load(input));
			return 0;
		}
	};

	::gpk::err_t			planetarySystemReset			(const ::gpk::SPlanetarySystem & solarSystem, const ::gpk::SPlanetarySystemEntityMap & entityMap, ::gpk::SEngine & engine, const ::gpk::vc8bgra & colors);
	::gpk::err_t			planetarySystemCreateEntities	(const ::gpk::SPlanetarySystem & solarSystem, ::gpk::SPlanetarySystemEntityMap & entityMap, ::gpk::SEngine & engine);
	::gpk::err_t			planetarySystemSetup			(const ::gpk::SPlanetarySystem & solarSystem, ::gpk::SPlanetarySystemEntityMap & entityMap, ::gpk::SEngine & engine); 
	::gpk::err_t			planetarySystemSetup			(::gpk::SEnginePlanetarySystem & planetarySystem, ::gpk::SEngine & engine, const ::gpk::SJSONReader & jsonData);
	::gpk::err_t			planetarySystemSetup			(::gpk::SEnginePlanetarySystem & planetarySystem, ::gpk::SEngine & engine, ::gpk::view<sc_t> jsonFilePath);
} // namespace

#endif // GPK_ENGINE_PLANETARY_SYSTEM_H_23701
