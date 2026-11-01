#include "gpk_rigid_body.h"
#include "gpk_orbit.h"

#ifndef GPK_ENGINE_ORBIT_H_23701
#define GPK_ENGINE_ORBIT_H_23701

namespace gpk
{
	gpk::err_t 		initOrbitRotation	(::gpk::SBodyForces & orbitForces, ::gpk::AXIS rotationAxis, const ::gpk::quatf2_t & orbitOrientation, f3_t rotation_period, f3_t rotation_unit);
	gpk::err_t 		initOrbitCenter		(::gpk::SBodyCenter & orbitCenter, ::gpk::AXIS rotationAxis, f3_t orbitalInclination, f3_t orbitRadius);

	stin gpk::err_t initOrbit			(::gpk::SBodyCenter & orbitCenter, ::gpk::SBodyForces & orbitForces, const ::gpk::SOrbit & orbit) {
		initOrbitCenter(orbitCenter, orbit.RotationAxis, orbit.AxialTilt, orbit.Radius);
		return initOrbitRotation(orbitForces, orbit.RotationAxis, orbitCenter.Orientation, orbit.RotationPeriod, orbit.RotationUnit);
	}

	gpk::err_t 		initOrbiterOrbit	(::gpk::SBodyCenter & orbitCenter , ::gpk::SBodyForces & orbitForces, f3_t orbitalInclination, f3_t orbital_period, f3_t orbital_unit);
	gpk::err_t 		initOrbiterCenter	(::gpk::SBodyCenter & planetCenter, f3_t orbitalInclination, f3_t orbitRadius, f3_t radiusScale);

	stin gpk::err_t initOrbiterBody		(::gpk::SBodyCenter & planetCenter, ::gpk::SBodyForces & planetForces, f3_t axialTilt, f3_t	rotation_period, f3_t	rotation_unit) {
		return initOrbiterOrbit(planetCenter, planetForces, axialTilt, rotation_period, rotation_unit);
	}
	stin	gpk::err_t initOrbiterOrbit	(const ::gpk::SOrbiter & bodyData, ::gpk::SBodyCenter	& orbitCenter, ::gpk::SBodyForces & orbitForces, f3_t rotationUnit) { 
		return ::gpk::initOrbiterOrbit(orbitCenter, orbitForces, bodyData.Orbit.AxialTilt, bodyData.Orbit.RotationPeriod, rotationUnit); 
	}
	stin	gpk::err_t initOrbiterCenter	(const ::gpk::SOrbiter & bodyData, ::gpk::SBodyCenter	& planetCenter, f3_t distanceScale) {
		return ::gpk::initOrbiterCenter(planetCenter, bodyData.Orbit.AxialTilt, bodyData.Orbit.Radius, distanceScale);
	}
	stin	gpk::err_t initOrbiterBody		(const ::gpk::SOrbiter & bodyData, ::gpk::SBodyCenter	& planetCenter, ::gpk::SBodyForces & planetForces, f3_t rotationUnit) {
		return ::gpk::initOrbiterBody(planetCenter, planetForces, bodyData.Body.AxialTilt, bodyData.Body.RotationPeriod, rotationUnit);
	}
	stin	gpk::err_t initOrbiterOrbit	(const ::gpk::SOrbiter & bodyData, ::gpk::SRigidBodyIntegrator & integrator, s2_t rigidBodyIndex, f3_t rotationUnit) {
		return ::gpk::initOrbiterOrbit(integrator.Centers[rigidBodyIndex], integrator.Forces[rigidBodyIndex], bodyData.Orbit.AxialTilt, bodyData.Orbit.RotationPeriod, rotationUnit); 
	}
	stin	gpk::err_t initOrbiterCenter	(const ::gpk::SOrbiter & bodyData, ::gpk::SRigidBodyIntegrator & integrator, s2_t rigidBodyIndex, f3_t distanceScale) {
		return ::gpk::initOrbiterCenter(integrator.Centers[rigidBodyIndex], bodyData.Orbit.AxialTilt, bodyData.Orbit.Radius, distanceScale);
	}
	stin	gpk::err_t initOrbiterBody		(const ::gpk::SOrbiter & bodyData, ::gpk::SRigidBodyIntegrator & integrator, s2_t rigidBodyIndex, f3_t rotationUnit) {
		return ::gpk::initOrbiterOrbit(integrator.Centers[rigidBodyIndex], integrator.Forces[rigidBodyIndex], bodyData.Body.AxialTilt, bodyData.Body.RotationPeriod, rotationUnit);
	}

	// deprecated
	gpk::err_t 		createOrbiter
		( ::gpk::SRigidBodyIntegrator	& bodies
		, ::gpk::AXIS	orbitalInclinationAxis
		, ::gpk::AXIS	obliquityToOrbitAxis
		, f3_t		orbitalInclination
		, f3_t		orbitalPeriod
		, f3_t		orbitalUnit
		, f3_t		obliquityToOrbit
		, f3_t		distance
		, f3_t		distanceScale
		, f3_t		rotationPeriod
		, f3_t		rotationUnit
		);
} // namespace

#endif // GPK_ENGINE_ORBIT_H_23701
