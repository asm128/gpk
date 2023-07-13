#include "gpk_celestial_body.h"

::gpk::error_t			gpk::printStar		(const ::gpk::SDetailStar & body) {
	info_printf(
		" \nGm					: %u"
		" \nVolume				: %u"
		" \nVolumetricMeanRadius: %u"
		" \nMeanDensity			: %u"
		" \nSurfaceGravity		: %f"
		" \nEllipticity			: %f"
		" \nMomentOfInertia		: %f"
		" \nVisualMagnitude		: %f"
		" \nAbsoluteMagnitude	: %f"
		" \nLuminosity			: %f"
		" \nMassConversionRate	: %u"
		" \nMeanEnergyProduction: %f"
		" \nSurfaceEmission		: %f"
		" \nCentralPressure		: %f"
		" \nCentralTemperature	: %f"
		" \nCentralDensity		: %f"
		, body.Gm							
		, body.Volume						
		, body.VolumetricMeanRadius		
		, body.MeanDensity				
		, body.SurfaceGravity				
		, body.Ellipticity				
		, body.MomentOfInertia			
		, body.VisualMagnitude			
		, body.AbsoluteMagnitude			
		, body.Luminosity					
		, body.MassConversionRate			
		, body.MeanEnergyProduction		
		, body.SurfaceEmission			
		, body.CentralPressure			
		, body.CentralTemperature			
		, body.CentralDensity				
	);
	return 0;
}

::gpk::error_t			gpk::printPlanet		(const ::gpk::SDetailPlanet & body) {
	info_printf(
		" \nGravity					: %f"
		" \nSurfacePressure			: %u"
		" \nDensity					: %u"
		" \nMeanTemperature			: %u"
		" \nNumberOfMoons			: %u"
		" \nGlobalMagneticField		: %s"
		" \nRadiusPolar				: %f"
		" \nRadiusEquatorial		: %f"
		" \nPerihelion				: %f"
		" \nAphelion				: %f"
		" \nRingSystem				: %s"
		" \nLengthOfDay				: %f"
		, body.Gravity					
		, body.SurfacePressure
		, body.Density
		, body.MeanTemperature
		, body.NumberOfMoons
		, ::gpk::bool2char(body.GlobalMagneticField)
		, body.RadiusPolar
		, body.RadiusEquatorial
		, body.Perihelion
		, body.Aphelion
		, ::gpk::bool2char(body.RingSystem)
		, body.LengthOfDay
		);
	return 0;
}

::gpk::error_t			gpk::printCelestialBody	(const ::gpk::SCelestialBody & body, ::gpk::CELESTIAL_BODY type) {
	info_printf(
		" \nMass				: %f"
		" \nDiameter			: %u"
		" \nOrbitRadius			: %f"
		" \nOrbitalPeriod		: %f"
		" \nOrbitalVelocity		: %f"
		" \nOrbitalInclination	: %f"
		" \nOrbitalEccentricity	: %f"
		" \nObliquityToOrbit	: %f"
		" \nRotationPeriod		: %f"
		" \nEscapeVelocity		: %f"
		, body.Mass					
		, body.Diameter				
		, body.Orbit.Radius		
		, body.Orbit.RotationPeriod			
		, body.OrbitalVelocity			
		, body.Orbit.AxialTilt
		, body.OrbitalEccentricity		
		, body.Body.AxialTilt		
		, body.Body.RotationPeriod			
		, body.EscapeVelocity			
		);
	switch(type) {
	default:
	case ::gpk::CELESTIAL_BODY_Planet: ::gpk::printPlanet(body.Detail.Planet); break;
	case ::gpk::CELESTIAL_BODY_Star  : ::gpk::printStar  (body.Detail.Star  ); break;
	}
	return 0;
}

::gpk::error_t			gpk::loadPlanet		(::gpk::SDetailPlanet & body, const ::gpk::SJSONReader & jsonData, int32_t jsonIndexStellarBody) { 
	ws_if_failed(::gpk::jsonObjectGetInteger(jsonData, jsonIndexStellarBody, "density"				, body.Density				));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "gravity"				, body.Gravity				));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "perihelion"			, body.Perihelion			));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "aphelion"				, body.Aphelion				));
	ws_if_failed(::gpk::jsonObjectGetInteger(jsonData, jsonIndexStellarBody, "mean_temperature"		, body.MeanTemperature		));
	ws_if_failed(::gpk::jsonObjectGetInteger(jsonData, jsonIndexStellarBody, "surface_pressure"		, body.SurfacePressure		));
	ws_if_failed(::gpk::jsonObjectGetInteger(jsonData, jsonIndexStellarBody, "number_of_moons"		, body.NumberOfMoons		));
	ws_if_failed(::gpk::jsonObjectGetBoolean(jsonData, jsonIndexStellarBody, "ring_system"			, body.RingSystem			));
	ws_if_failed(::gpk::jsonObjectGetBoolean(jsonData, jsonIndexStellarBody, "global_magnetic_field", body.GlobalMagneticField	));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "length_of_day"		, body.LengthOfDay			));
	return 0; 
}

::gpk::error_t			gpk::loadStar		(::gpk::SDetailStar & body, const ::gpk::SJSONReader & jsonData, int32_t jsonIndexStellarBody) { 
	ws_if_failed(::gpk::jsonObjectGetInteger(jsonData, jsonIndexStellarBody, "gm"						, body.Gm					));
	ws_if_failed(::gpk::jsonObjectGetInteger(jsonData, jsonIndexStellarBody, "volume"					, body.Volume				));
	ws_if_failed(::gpk::jsonObjectGetInteger(jsonData, jsonIndexStellarBody, "volumetric_mean_radius"	, body.VolumetricMeanRadius	));
	ws_if_failed(::gpk::jsonObjectGetInteger(jsonData, jsonIndexStellarBody, "mean_density"				, body.MeanDensity			));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "surface_gravity"			, body.SurfaceGravity		));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "ellipticity"				, body.Ellipticity			));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "moment_of_inertia"		, body.MomentOfInertia		));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "visual_magnitude"			, body.VisualMagnitude		));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "absolute_magnitude"		, body.AbsoluteMagnitude	));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "luminosity"				, body.Luminosity			));
	ws_if_failed(::gpk::jsonObjectGetInteger(jsonData, jsonIndexStellarBody, "mass_conversion_rate"		, body.MassConversionRate	));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "mean_energy_production"	, body.MeanEnergyProduction	));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "surface_emission"			, body.SurfaceEmission		));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "central_pressure"			, body.CentralPressure		));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "central_temperature"		, body.CentralTemperature	));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "central_density"			, body.CentralDensity		));
	return 0; 
}

::gpk::error_t			gpk::loadCelestialBody(::gpk::SCelestialBody & body, ::gpk::CELESTIAL_BODY type, const ::gpk::SJSONReader & jsonData, int32_t jsonIndexStellarBody) { 
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "escape_velocity"			, body.EscapeVelocity		));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "mass"						, body.Mass					));
	ws_if_failed(::gpk::jsonObjectGetInteger(jsonData, jsonIndexStellarBody, "diameter"					, body.Diameter				));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "distance_from_sun"		, body.Orbit.Radius			));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "escape_velocity"			, body.EscapeVelocity		));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "rotation_period"			, body.Body.RotationPeriod	));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "orbital_period"			, body.Orbit.RotationPeriod	));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "orbital_velocity"			, body.OrbitalVelocity		));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "orbital_inclination"		, body.Orbit.AxialTilt		));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "orbital_eccentricity"		, body.OrbitalEccentricity	));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "obliquity_to_orbit"		, body.Body.AxialTilt		));
	body.Body.Radius		= body.Diameter * .5f;
	switch(type) {
	default:
	case ::gpk::CELESTIAL_BODY_Planet: ::gpk::loadPlanet(body.Detail.Planet, jsonData, jsonIndexStellarBody); break;
	case ::gpk::CELESTIAL_BODY_Star  : ::gpk::loadStar  (body.Detail.Star  , jsonData, jsonIndexStellarBody); break;
	}

	return 0; 
}
