#include "gpk_engine_orbit.h"

GPK_USING_TYPEINT	();
GPK_USING_VIEW		();
GPK_USING_APOD		();
GPK_USING_N2		();
GPK_USING_N3		();


gpk::err_t 		gpk::initOrbitRotation	(::gpk::SBodyForces & orbitForces, ::gpk::AXIS rotationAxis, const ::gpk::quatf2_t & orbitOrientation, f3_t rotation_period, f3_t rotation_unit) {
	orbitForces				= {};	
	::gpk::orbitRotation(rotationAxis, rotation_period, rotation_unit, orbitOrientation, orbitForces.Rotation);
	return 0;
}

gpk::err_t 		gpk::initOrbitCenter	(::gpk::SBodyCenter & orbitCenter, ::gpk::AXIS rotationAxis, f3_t orbitalInclination, f3_t orbitRadius) {
	orbitCenter				= {};
	::gpk::orientationFromEuler(rotationAxis, orbitalInclination, orbitCenter.Orientation);
	switch(rotationAxis) {
	case ::gpk::AXIS_X_NEGATIVE:
	case ::gpk::AXIS_X_POSITIVE: orbitCenter.Position.x	= f2_t(orbitRadius); break;
	case ::gpk::AXIS_Y_NEGATIVE:
	case ::gpk::AXIS_Y_POSITIVE: orbitCenter.Position.y	= f2_t(orbitRadius); break;
	case ::gpk::AXIS_Z_NEGATIVE:
	case ::gpk::AXIS_Z_POSITIVE: orbitCenter.Position.z	= f2_t(orbitRadius); break;
	}
	return 0;
}

gpk::err_t 		gpk::initOrbiterOrbit	(::gpk::SBodyCenter & orbitCenter, ::gpk::SBodyForces & orbitForces, f3_t orbitalInclination, f3_t rotationPeriod, f3_t rotationUnit) {
	orbitCenter				= {};
	orbitForces				= {};	
	::gpk::orientationFromEuler	(::gpk::AXIS_X_POSITIVE, orbitalInclination, orbitCenter.Orientation);
	::gpk::orbitRotation		(::gpk::AXIS_Y_NEGATIVE, rotationPeriod, rotationUnit, orbitCenter.Orientation, orbitForces.Rotation);
	return 0;
}

gpk::err_t 		gpk::initOrbiterCenter	(::gpk::SBodyCenter & planetCenter, f3_t orbitalInclination, f3_t orbitRadius, f3_t radiusScale) {
	::gpk::initOrbitCenter(planetCenter, ::gpk::AXIS_X_POSITIVE, orbitalInclination, orbitRadius * radiusScale);
	planetCenter			= {};
	planetCenter.Position.x	= f2_t(radiusScale * orbitRadius);
	return 0;
}

static	gpk::err_t initOrbiterBodyWithGravityCenter
	( ::gpk::SBodyCenter	& planetCenter
	, ::gpk::SBodyForces	& planetForces		
	, ::gpk::AXIS			orbitAxis
	, ::gpk::AXIS			rotationAxis
	, f3_t				axialTilt
	, f3_t				distance
	, f3_t				distance_scale
	, f3_t				rotation_period
	, f3_t				rotation_unit
	) {
	::gpk::initOrbitCenter(planetCenter, orbitAxis, axialTilt, distance * distance_scale);
	::gpk::orbitRotation(rotationAxis, rotation_period, rotation_unit, planetCenter.Orientation, planetForces.Rotation);
	return 0;
}

gpk::err_t 		gpk::createOrbiter
	( ::gpk::SRigidBodyIntegrator	& bodies
	, ::gpk::AXIS					orbitalInclinationAxis
	, ::gpk::AXIS					obliquityToOrbitAxis
	, f3_t						orbital_inclination
	, f3_t						orbital_period
	, f3_t						orbital_unit
	, f3_t						axialTilt
	, f3_t						distance
	, f3_t						distance_scale
	, f3_t						rotation_period
	, f3_t						rotation_unit
	) {
	const gpk::err_t 	indexFirstBody				= bodies.Create(2);
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
