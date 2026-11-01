#include "gpk_celestial_body.h"
#include "gpk_stdstring.h"

gpk::err_t 		gpk::printStar		(const ::gpk::SDetailStar & body) {
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

gpk::err_t 		gpk::printPlanet		(const ::gpk::SDetailPlanet & body) {
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
		, ::gpk::b82st(body.GlobalMagneticField)
		, body.RadiusPolar
		, body.RadiusEquatorial
		, body.Perihelion
		, body.Aphelion
		, ::gpk::b82st(body.RingSystem)
		, body.LengthOfDay
		);
	return 0;
}

gpk::err_t			gpk::printCelestialBody	(const SCelestialBody & body, CELESTIAL_BODY type) {
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
	case CELESTIAL_BODY_Planet: printPlanet(body.Detail.Planet); break;
	case CELESTIAL_BODY_Star  : printStar  (body.Detail.Star  ); break;
	}
	return 0;
}

gpk::CELESTIAL_BODY	gpk::bodyTypeFromString		(vcst_t bodyType) { 
	if(2 > bodyType.size())
		return (CELESTIAL_BODY)-1;

	return get_value_camelcased<CELESTIAL_BODY>(bodyType);
}

gpk::PLANET_TYPE		gpk::bodySubtypeFromString	(vcst_t bodyType) {
	if(2 > bodyType.size())
		return (PLANET_TYPE)-1;

	return get_value_camelcased<PLANET_TYPE>(bodyType);
}

gpk::err_t		gpk::loadPlanet		(SDetailPlanet & body, cnst SJSONReader & jsonData, s2_t jsonIndexStellarBody) { 
	vcst_t				subtype;
	if_fail_w(::gpk::jsonObjectGetAsString (jsonData, jsonIndexStellarBody, "subtype"				, subtype					));
	body.Subtype	= ::gpk::bodySubtypeFromString(subtype);
	if_fail_w(::gpk::jsonObjectGetAsInteger(jsonData, jsonIndexStellarBody, "density"				, body.Density				));
	if_fail_w(::gpk::jsonObjectGetAsDecimal(jsonData, jsonIndexStellarBody, "gravity"				, body.Gravity				));
	if_fail_w(::gpk::jsonObjectGetAsDecimal(jsonData, jsonIndexStellarBody, "perihelion"				, body.Perihelion			));
	if_fail_w(::gpk::jsonObjectGetAsDecimal(jsonData, jsonIndexStellarBody, "aphelion"				, body.Aphelion				));
	if_fail_w(::gpk::jsonObjectGetAsInteger(jsonData, jsonIndexStellarBody, "mean_temperature"		, body.MeanTemperature		));
	if_fail_w(::gpk::jsonObjectGetAsInteger(jsonData, jsonIndexStellarBody, "surface_pressure"		, body.SurfacePressure		));
	if_fail_w(::gpk::jsonObjectGetAsInteger(jsonData, jsonIndexStellarBody, "number_of_moons"		, body.NumberOfMoons		));
	if_fail_w(::gpk::jsonObjectGetAsBoolean(jsonData, jsonIndexStellarBody, "ring_system"			, body.RingSystem			));
	if_fail_w(::gpk::jsonObjectGetAsBoolean(jsonData, jsonIndexStellarBody, "global_magnetic_field"	, body.GlobalMagneticField	));
	if_fail_w(::gpk::jsonObjectGetAsDecimal(jsonData, jsonIndexStellarBody, "length_of_day"			, body.LengthOfDay			));
	return 0; 
}

gpk::err_t 		gpk::loadStar		(::gpk::SDetailStar & body, const ::gpk::SJSONReader & jsonData, s2_t jsonIndexStellarBody) { 
	if_fail_w(::gpk::jsonObjectGetAsInteger(jsonData, jsonIndexStellarBody, "gm"						, body.Gm					));
	if_fail_w(::gpk::jsonObjectGetAsInteger(jsonData, jsonIndexStellarBody, "volume"					, body.Volume				));
	if_fail_w(::gpk::jsonObjectGetAsInteger(jsonData, jsonIndexStellarBody, "volumetric_mean_radius"	, body.VolumetricMeanRadius	));
	if_fail_w(::gpk::jsonObjectGetAsInteger(jsonData, jsonIndexStellarBody, "mean_density"			, body.MeanDensity			));
	if_fail_w(::gpk::jsonObjectGetAsDecimal(jsonData, jsonIndexStellarBody, "surface_gravity"		, body.SurfaceGravity		));
	if_fail_w(::gpk::jsonObjectGetAsDecimal(jsonData, jsonIndexStellarBody, "ellipticity"			, body.Ellipticity			));
	if_fail_w(::gpk::jsonObjectGetAsDecimal(jsonData, jsonIndexStellarBody, "moment_of_inertia"		, body.MomentOfInertia		));
	if_fail_w(::gpk::jsonObjectGetAsDecimal(jsonData, jsonIndexStellarBody, "visual_magnitude"		, body.VisualMagnitude		));
	if_fail_w(::gpk::jsonObjectGetAsDecimal(jsonData, jsonIndexStellarBody, "absolute_magnitude"		, body.AbsoluteMagnitude	));
	if_fail_w(::gpk::jsonObjectGetAsDecimal(jsonData, jsonIndexStellarBody, "luminosity"				, body.Luminosity			));
	if_fail_w(::gpk::jsonObjectGetAsInteger(jsonData, jsonIndexStellarBody, "mass_conversion_rate"	, body.MassConversionRate	));
	if_fail_w(::gpk::jsonObjectGetAsDecimal(jsonData, jsonIndexStellarBody, "mean_energy_production"	, body.MeanEnergyProduction	));
	if_fail_w(::gpk::jsonObjectGetAsDecimal(jsonData, jsonIndexStellarBody, "surface_emission"		, body.SurfaceEmission		));
	if_fail_w(::gpk::jsonObjectGetAsDecimal(jsonData, jsonIndexStellarBody, "central_pressure"		, body.CentralPressure		));
	if_fail_w(::gpk::jsonObjectGetAsDecimal(jsonData, jsonIndexStellarBody, "central_temperature"	, body.CentralTemperature	));
	if_fail_w(::gpk::jsonObjectGetAsDecimal(jsonData, jsonIndexStellarBody, "central_density"		, body.CentralDensity		));
	return 0; 
}

gpk::err_t 		gpk::loadCelestialBody(::gpk::SCelestialBody & body, ::gpk::CELESTIAL_BODY type, const ::gpk::SJSONReader & jsonData, s2_t jsonIndexStellarBody) { 
	if_fail_w(::gpk::jsonObjectGetAsDecimal(jsonData, jsonIndexStellarBody, "escape_velocity"		, body.EscapeVelocity		));
	if_fail_w(::gpk::jsonObjectGetAsDecimal(jsonData, jsonIndexStellarBody, "mass"					, body.Mass					));
	if_fail_w(::gpk::jsonObjectGetAsInteger(jsonData, jsonIndexStellarBody, "diameter"				, body.Diameter				));
	if_fail_w(::gpk::jsonObjectGetAsDecimal(jsonData, jsonIndexStellarBody, "distance_from_sun"		, body.Orbit.Radius			));
	if_fail_w(::gpk::jsonObjectGetAsDecimal(jsonData, jsonIndexStellarBody, "escape_velocity"		, body.EscapeVelocity		));
	if_fail_w(::gpk::jsonObjectGetAsDecimal(jsonData, jsonIndexStellarBody, "rotation_period"		, body.Body.RotationPeriod	));
	if_fail_w(::gpk::jsonObjectGetAsDecimal(jsonData, jsonIndexStellarBody, "orbital_period"			, body.Orbit.RotationPeriod	));
	if_fail_w(::gpk::jsonObjectGetAsDecimal(jsonData, jsonIndexStellarBody, "orbital_velocity"		, body.OrbitalVelocity		));
	if_fail_w(::gpk::jsonObjectGetAsDecimal(jsonData, jsonIndexStellarBody, "orbital_inclination"	, body.Orbit.AxialTilt		));
	if_fail_w(::gpk::jsonObjectGetAsDecimal(jsonData, jsonIndexStellarBody, "orbital_eccentricity"	, body.OrbitalEccentricity	));
	if_fail_w(::gpk::jsonObjectGetAsDecimal(jsonData, jsonIndexStellarBody, "obliquity_to_orbit"		, body.Body.AxialTilt		));
	body.Body.Radius		= body.Diameter * .5f;
	body.Body.TiltAxis		= ::gpk::AXIS_X_POSITIVE;
	body.Orbit.TiltAxis		= ::gpk::AXIS_Y_NEGATIVE;
	switch(type) {
	default:
	case ::gpk::CELESTIAL_BODY_Planet: ::gpk::loadPlanet(body.Detail.Planet, jsonData, jsonIndexStellarBody); break;
	case ::gpk::CELESTIAL_BODY_Star  : ::gpk::loadStar  (body.Detail.Star  , jsonData, jsonIndexStellarBody); break;
	}
	return 0; 
}
