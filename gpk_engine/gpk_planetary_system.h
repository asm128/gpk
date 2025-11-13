#include "gpk_celestial_body.h"
#include "gpk_linear_map_pod.h"

#include "gpk_json.h"

#ifndef GPK_PLANETARY_SYSTEM_H_23701
#define GPK_PLANETARY_SYSTEM_H_23701

namespace gpk
{
	typedef	::gpk::lmpod<::gpk::vcsc_t, ::gpk::SCelestialBody>	SCelestialBodyMap;

	struct SPlanetarySystem {
		::gpk::SCelestialBodyMap			Body;
		::gpk::apod<::gpk::CELESTIAL_BODY>	Type;
		::gpk::as2_t						Parent;

		::gpk::error_t				Save			(::gpk::au0_t & output)	const	{ 
			gpk_necs(Body.Save(output));
			gpk_necs(gpk::saveView(output, Type));
			gpk_necs(gpk::saveView(output, Parent));
			return 0;
		}

		::gpk::error_t				Load			(::gpk::vcu0_t & input)	{ 
			gpk_necs(Body.Load(input));
			gpk_necs(gpk::loadView(input, Type));
			gpk_necs(gpk::loadView(input, Parent));
			return 0;
		}
	};

	::gpk::error_t			planetarySystemSetup	(::gpk::SPlanetarySystem & planetarySystem, const ::gpk::SJSONReader & jsonData);
	::gpk::error_t			planetarySystemSetup	(::gpk::SPlanetarySystem & planetarySystem, ::gpk::vcsc_t jsonFilePath);
} // namespace

#endif // GPK_PLANETARY_SYSTEM_H_23701

// Mercury - 20° E of crater Hun Kal
// Venus - central peak of crater Ariadne
// Earth - no longer defined by a single point, but by hundreds of points around the world, taking into account tectonic motion
// Mars - 47.95137° E of Viking 1 lander
// The gas giants - multiple systems are used, based either on planetary magnetic fields or observed atmospheric features.
// Moon - the mean Earth point: the point on average facing the Earth
// Io - the mean Jupiter point
// Europa - 178° W of crater Cilix
// Ganymede - 128° E of crater Anat
// Callisto - 34° W of crater Saga
// Mimas - 162° E of crater Palomides
// Enceladus - 5° E of crater Salih
// Tethys - 61° W of crater Arete
// Dione - 63° E of crater Palinurus
// Rhea - 20° W of crater Tore
// Iapetus - 84° W of crater Almeric
// The IAU also defines meridians for some dwarf planets, asteroids, and comets.