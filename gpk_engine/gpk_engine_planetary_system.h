#include "gpk_planetary_system.h"
#include "gpk_engine.h"
#include "gpk_json.h"
#include "gpk_view_color.h"

#ifndef GPK_ENGINE_PLANETARY_SYSTEM_H_23701
#define GPK_ENGINE_PLANETARY_SYSTEM_H_23701

namespace gpk
{
	struct SPlanetarySystemEntityMap {
		aeid		GravityCenters	;
		aeid		Orbits			;
		aeid		Bodies			;
		aeid		Cameras			;
		eid_t		Camera			= EID_INVALID;
		eid_t		Background		= EID_INVALID;

		err_t		Save			(au0_t & output)	const	{ 
			if_fail_fe(saveView(output, GravityCenters	));
			if_fail_fe(saveView(output, Orbits			));
			if_fail_fe(saveView(output, Bodies			));
			if_fail_fe(saveView(output, Cameras		));
			if_fail_fe(savePOD (output, Camera			));
			if_fail_fe(savePOD (output, Background		));
			return 0;
		}

		err_t		Load			(vcu0_t & input)	{ 
			if_fail_fe(loadView(input, GravityCenters	));
			if_fail_fe(loadView(input, Orbits			));
			if_fail_fe(loadView(input, Bodies			));
			if_fail_fe(loadView(input, Cameras			));
			if_fail_fe(loadPOD (input, Camera			));
			if_fail_fe(loadPOD (input, Background		));
			return 0;
		}
	};

	struct SEnginePlanetarySystem {
		SPlanetarySystem			PlanetarySystem;
		SPlanetarySystemEntityMap	EntityMap;

		err_t		Save			(au0_t & output)	const	{ 
			if_fail_fe(PlanetarySystem.Save(output));
			if_fail_fe(EntityMap.Save(output));
			return 0;
		}

		err_t		Load			(vcu0_t & input)	{ 
			if_fail_fe(PlanetarySystem.Load(input));
			if_fail_fe(EntityMap.Load(input));
			return 0;
		}
	};

	err_t		planetarySystemReset			(const SPlanetarySystem & solarSystem, const SPlanetarySystemEntityMap & entityMap, SEngine & engine, vc8bgra colors);
	err_t		planetarySystemCreateEntities	(const SPlanetarySystem & solarSystem, SPlanetarySystemEntityMap & entityMap, SEngine & engine);
	err_t		planetarySystemSetup			(const SPlanetarySystem & solarSystem, SPlanetarySystemEntityMap & entityMap, SEngine & engine); 
	err_t		planetarySystemSetup			(SEnginePlanetarySystem & planetarySystem, SEngine & engine, const SJSONReader & jsonData);
	err_t		planetarySystemSetup			(SEnginePlanetarySystem & planetarySystem, SEngine & engine, vcst_t jsonFilePath);
} // namespace

#endif // GPK_ENGINE_PLANETARY_SYSTEM_H_23701
