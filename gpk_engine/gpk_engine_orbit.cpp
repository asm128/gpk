#include "gpk_engine_orbit.h"

::gpk::error_t			gpk::initOrbitRotation	(::gpk::SBodyForces & orbitForces, ::gpk::AXIS rotationAxis, const ::gpk::quatf32 & orbitOrientation, double rotation_period, double rotation_unit) {
	orbitForces				= {};	
	::gpk::orbitRotation(rotationAxis, rotation_period, rotation_unit, orbitOrientation, orbitForces.Rotation);
	return 0;
}

::gpk::error_t			gpk::initOrbitCenter	(::gpk::SBodyCenter & orbitCenter, ::gpk::AXIS rotationAxis, double orbitalInclination, double orbitRadius) {
	orbitCenter				= {};
	::gpk::orientationFromEuler(rotationAxis, orbitalInclination, orbitCenter.Orientation);
	switch(rotationAxis) {
	case ::gpk::AXIS_X_NEGATIVE:
	case ::gpk::AXIS_X_POSITIVE: orbitCenter.Position.x	= float(orbitRadius); break;
	case ::gpk::AXIS_Y_NEGATIVE:
	case ::gpk::AXIS_Y_POSITIVE: orbitCenter.Position.y	= float(orbitRadius); break;
	case ::gpk::AXIS_Z_NEGATIVE:
	case ::gpk::AXIS_Z_POSITIVE: orbitCenter.Position.z	= float(orbitRadius); break;
	}
	return 0;
}

::gpk::error_t			gpk::initOrbiterOrbit	(::gpk::SBodyCenter & orbitCenter, ::gpk::SBodyForces & orbitForces, double orbitalInclination, double rotationPeriod, double rotationUnit) {
	orbitCenter				= {};
	orbitForces				= {};	
	::gpk::orientationFromEuler	(::gpk::AXIS_X_POSITIVE, orbitalInclination, orbitCenter.Orientation);
	::gpk::orbitRotation		(::gpk::AXIS_Y_NEGATIVE, rotationPeriod, rotationUnit, orbitCenter.Orientation, orbitForces.Rotation);
	return 0;
}

::gpk::error_t			gpk::initOrbiterCenter	(::gpk::SBodyCenter & planetCenter, double orbitalInclination, double orbitRadius, double radiusScale) {
	::gpk::initOrbitCenter(planetCenter, ::gpk::AXIS_X_POSITIVE, orbitalInclination, orbitRadius * radiusScale);
	planetCenter			= {};
	planetCenter.Position.x	= float(radiusScale * orbitRadius);
	return 0;
}

static	::gpk::error_t	initOrbiterBodyWithGravityCenter
	( ::gpk::SBodyCenter	& planetCenter
	, ::gpk::SBodyForces	& planetForces		
	, ::gpk::AXIS			orbitAxis
	, ::gpk::AXIS			rotationAxis
	, double				axialTilt
	, double				distance
	, double				distance_scale
	, double				rotation_period
	, double				rotation_unit
	) {
	::gpk::initOrbitCenter(planetCenter, orbitAxis, axialTilt, distance * distance_scale);
	::gpk::orbitRotation(rotationAxis, rotation_period, rotation_unit, planetCenter.Orientation, planetForces.Rotation);
	return 0;
}

::gpk::error_t			gpk::createOrbiter
	( ::gpk::SRigidBodyIntegrator	& bodies
	, ::gpk::AXIS					orbitalInclinationAxis
	, ::gpk::AXIS					obliquityToOrbitAxis
	, double						orbital_inclination
	, double						orbital_period
	, double						orbital_unit
	, double						axialTilt
	, double						distance
	, double						distance_scale
	, double						rotation_period
	, double						rotation_unit
	) {
	const ::gpk::error_t		indexFirstBody				= bodies.Create(2);
	::gpk::initOrbiterOrbit(bodies.Centers[indexFirstBody] = {}, bodies.Forces[indexFirstBody] = {}, orbital_inclination, orbital_period, orbital_unit);
	::initOrbiterBodyWithGravityCenter
		( bodies.Centers[indexFirstBody + 1] = {}
		, bodies.Forces	[indexFirstBody + 1] = {}
		, orbitalInclinationAxis
		, obliquityToOrbitAxis
		, axialTilt
		, distance
		, distance_scale
		, rotation_period
		, rotation_unit
		);
	return indexFirstBody;
}
