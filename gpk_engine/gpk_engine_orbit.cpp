#include "gpk_engine_orbit.h"

::gpk::error_t			gpk::initOrbitRotation	(::gpk::SBodyForces & orbitForces, ::gpk::AXIS rotationAxis, const ::gpk::quatf32 & orbitOrientation, double rotation_period, double rotation_unit) {
	orbitForces				= {};	
	::gpk::orbitRotation(rotationAxis, rotation_period, rotation_unit, orbitOrientation, orbitForces.Rotation);
	return 0;
}

::gpk::error_t			gpk::initOrbitCenter	(::gpk::SBodyCenter & orbitCenter, ::gpk::AXIS rotationAxis, double orbital_inclination, double distance) {
	orbitCenter				= {};
	::gpk::orientationFromEuler(rotationAxis, orbital_inclination, orbitCenter.Orientation);
	orbitCenter.Position.x	= float(distance);
	return 0;
}

::gpk::error_t			gpk::initOrbiterOrbit	(::gpk::SBodyCenter & orbitCenter, ::gpk::SBodyForces & orbitForces, double orbital_inclination, double rotation_period, double rotation_unit) {
	orbitCenter				= {};
	orbitForces				= {};	
	::gpk::orientationFromEuler	(::gpk::AXIS_X_POSITIVE, orbital_inclination, orbitCenter.Orientation);
	::gpk::orbitRotation		(::gpk::AXIS_Y_NEGATIVE, rotation_period, rotation_unit, orbitCenter.Orientation, orbitForces.Rotation);
	return 0;
}

::gpk::error_t			gpk::initOrbiterCenter	(::gpk::SBodyCenter & planetCenter, double distance, double distance_scale) {
	planetCenter			= {};
	planetCenter.Position.x	= float(distance_scale * distance);
	return 0;
}

static	::gpk::error_t	initOrbiterBodyWithGravityCenter
	( ::gpk::SBodyCenter	& planetCenter
	, ::gpk::SBodyForces	& planetForces		
	, ::gpk::SBodyMass		& planetMass		
	, double				mass
	, double				axialTilt
	, double				distance
	, double				distance_scale
	, double				rotation_period
	, double				rotation_unit
	) {
	planetMass.InverseMass	= mass ? float(1.0 / mass) : 0;
	::gpk::initOrbiterCenter(planetCenter, distance, distance_scale);
	::gpk::orientationFromEuler(::gpk::AXIS_X_POSITIVE, axialTilt, planetCenter.Orientation);
	::gpk::orbitRotation(::gpk::AXIS_Y_NEGATIVE, rotation_period, rotation_unit, planetCenter.Orientation, planetForces.Rotation);
	return 0;
}

::gpk::error_t			gpk::createOrbiter
	( ::gpk::SRigidBodyIntegrator	& bodies
	, double						orbital_inclination
	, double						orbital_period
	, double						orbital_unit
	, double						mass
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
		, bodies.Masses	[indexFirstBody + 1] = {}
		, mass
		, axialTilt
		, distance
		, distance_scale
		, rotation_period
		, rotation_unit
		);
	return indexFirstBody;
}
