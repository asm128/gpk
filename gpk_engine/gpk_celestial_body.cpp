#include "gpk_celestial_body.h"

::gpk::error_t			gpk::printStar		(const ::gpk::SDetailStar & body) {
	info_printf(
		" \nMass				: %u"
		" \nGm					: %u"
		" \nVolume				: %u"
		" \nVolumetricMeanRadius: %u"
		" \nMeanDensity			: %u"
		" \nSurfaceGravity		: %f"
		" \nEscapeVelocity		: %f"
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
		, body.Mass						
		, body.Gm							
		, body.Volume						
		, body.VolumetricMeanRadius		
		, body.MeanDensity				
		, body.SurfaceGravity				
		, body.EscapeVelocity				
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
		" \nMass				: %f"
		" \nDiameter			: %u"
		" \nDensity				: %u"
		" \nGravity				: %f"
		" \nEscapeVelocity		: %f"
		" \nRotationPeriod		: %f"
		" \nLengthOfDay			: %f"
		" \nDistanceFromSun		: %f"
		" \nPerihelion			: %f"
		" \nAphelion			: %f"
		" \nOrbitalPeriod		: %f"
		" \nOrbitalVelocity		: %f"
		" \nOrbitalInclination	: %f"
		" \nOrbitalEccentricity	: %f"
		" \nObliquityToOrbit	: %f"
		" \nMeanTemperature		: %u"
		" \nSurfacePressure		: %u"
		" \nNumberOfMoons		: %u"
		" \nRingSystem			: %s"
		" \nGlobalMagneticField	: %s"
		" \nRadiusPolar			: %f"
		" \nRadiusEquatorial	: %f"
		, body.Mass				
		, body.Diameter			
		, body.Density				
		, body.Gravity				
		, body.EscapeVelocity		
		, body.RotationPeriod		
		, body.LengthOfDay			
		, body.DistanceFromSun		
		, body.Perihelion			
		, body.Aphelion			
		, body.OrbitalPeriod		
		, body.OrbitalVelocity		
		, body.OrbitalInclination	
		, body.OrbitalEccentricity	
		, body.ObliquityToOrbit	
		, body.MeanTemperature		
		, body.SurfacePressure		
		, body.NumberOfMoons		
		, ::gpk::bool2char(body.RingSystem			)
		, ::gpk::bool2char(body.GlobalMagneticField	)
		, body.RadiusPolar			
		, body.RadiusEquatorial	
	);
	return 0;
}

::gpk::error_t			gpk::loadPlanet		(::gpk::SDetailPlanet & bodyData, const ::gpk::SJSONReader & jsonData, int32_t jsonIndexStellarBody) { 
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "mass"					, bodyData.Mass					));
	ws_if_failed(::gpk::jsonObjectGetInteger(jsonData, jsonIndexStellarBody, "diameter"				, bodyData.Diameter				));
	ws_if_failed(::gpk::jsonObjectGetInteger(jsonData, jsonIndexStellarBody, "density"				, bodyData.Density				));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "gravity"				, bodyData.Gravity				));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "escape_velocity"		, bodyData.EscapeVelocity		));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "rotation_period"		, bodyData.RotationPeriod		));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "length_of_day"		, bodyData.LengthOfDay			));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "distance_from_sun"	, bodyData.DistanceFromSun		));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "perihelion"			, bodyData.Perihelion			));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "aphelion"				, bodyData.Aphelion				));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "orbital_period"		, bodyData.OrbitalPeriod		));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "orbital_velocity"		, bodyData.OrbitalVelocity		));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "orbital_inclination"	, bodyData.OrbitalInclination	));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "orbital_eccentricity"	, bodyData.OrbitalEccentricity	));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "obliquity_to_orbit"	, bodyData.ObliquityToOrbit		));
	ws_if_failed(::gpk::jsonObjectGetInteger(jsonData, jsonIndexStellarBody, "mean_temperature"		, bodyData.MeanTemperature		));
	ws_if_failed(::gpk::jsonObjectGetInteger(jsonData, jsonIndexStellarBody, "surface_pressure"		, bodyData.SurfacePressure		));
	ws_if_failed(::gpk::jsonObjectGetInteger(jsonData, jsonIndexStellarBody, "number_of_moons"		, bodyData.NumberOfMoons		));
	ws_if_failed(::gpk::jsonObjectGetBoolean(jsonData, jsonIndexStellarBody, "ring_system"			, bodyData.RingSystem			));
	ws_if_failed(::gpk::jsonObjectGetBoolean(jsonData, jsonIndexStellarBody, "global_magnetic_field", bodyData.GlobalMagneticField	));
	return 0; 
}

::gpk::error_t			gpk::loadStar		(::gpk::SDetailStar & bodyData, const ::gpk::SJSONReader & jsonData, int32_t jsonIndexStellarBody) { 
	ws_if_failed(::gpk::jsonObjectGetInteger(jsonData, jsonIndexStellarBody, "mass"						, bodyData.Mass					));
	ws_if_failed(::gpk::jsonObjectGetInteger(jsonData, jsonIndexStellarBody, "gm"						, bodyData.Gm					));
	ws_if_failed(::gpk::jsonObjectGetInteger(jsonData, jsonIndexStellarBody, "volume"					, bodyData.Volume				));
	ws_if_failed(::gpk::jsonObjectGetInteger(jsonData, jsonIndexStellarBody, "volumetric_mean_radius"	, bodyData.VolumetricMeanRadius	));
	ws_if_failed(::gpk::jsonObjectGetInteger(jsonData, jsonIndexStellarBody, "mean_density"				, bodyData.MeanDensity			));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "surface_gravity"			, bodyData.SurfaceGravity		));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "escape_velocity"			, bodyData.EscapeVelocity		));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "ellipticity"				, bodyData.Ellipticity			));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "moment_of_inertia"		, bodyData.MomentOfInertia		));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "visual_magnitude"			, bodyData.VisualMagnitude		));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "absolute_magnitude"		, bodyData.AbsoluteMagnitude	));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "luminosity"				, bodyData.Luminosity			));
	ws_if_failed(::gpk::jsonObjectGetInteger(jsonData, jsonIndexStellarBody, "mass_conversion_rate"		, bodyData.MassConversionRate	));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "mean_energy_production"	, bodyData.MeanEnergyProduction	));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "surface_emission"			, bodyData.SurfaceEmission		));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "central_pressure"			, bodyData.CentralPressure		));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "central_temperature"		, bodyData.CentralTemperature	));
	ws_if_failed(::gpk::jsonObjectGetDecimal(jsonData, jsonIndexStellarBody, "central_density"			, bodyData.CentralDensity		));
	return 0; 
}
