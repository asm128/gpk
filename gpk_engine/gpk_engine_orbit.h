#include "gpk_rigid_body.h"
#include "gpk_orbit.h"

#ifndef GPK_ENGINE_ORBIT_H_23701
#define GPK_ENGINE_ORBIT_H_23701

namespace gpk
{
	::gpk::error_t			initOrbitRotation	(::gpk::SBodyForces & orbitForces, ::gpk::AXIS rotationAxis, const ::gpk::quatf32 & orbitOrientation, double rotation_period, double rotation_unit);
	::gpk::error_t			initOrbitCenter		(::gpk::SBodyCenter & orbitCenter, ::gpk::AXIS rotationAxis, double orbital_inclination, double distance);

	stainli ::gpk::error_t	initOrbit			(::gpk::SBodyCenter & orbitCenter, ::gpk::SBodyForces & orbitForces, const ::gpk::SOrbit & orbit) {
		initOrbitCenter(orbitCenter, orbit.RotationAxis, orbit.AxialTilt, orbit.Radius);
		return initOrbitRotation(orbitForces, orbit.RotationAxis, orbitCenter.Orientation, orbit.RotationPeriod, orbit.RotationUnit);
	}

	::gpk::error_t			initOrbiterOrbit	(::gpk::SBodyCenter & orbitCenter , ::gpk::SBodyForces & orbitForces, double orbital_inclination, double orbital_period, double orbital_unit);
	::gpk::error_t			initOrbiterCenter	(::gpk::SBodyCenter & planetCenter, double distance, double distance_scale);

	stainli ::gpk::error_t	initOrbiterBody		(::gpk::SBodyCenter & planetCenter, ::gpk::SBodyForces & planetForces, double axialTilt, double	rotation_period, double	rotation_unit) {
		return initOrbiterOrbit(planetCenter, planetForces, axialTilt, rotation_period, rotation_unit);
	}
	stainli	::gpk::error_t	initOrbiterOrbit	(const ::gpk::SOrbiter & bodyData, ::gpk::SBodyCenter	& orbitCenter, ::gpk::SBodyForces & orbitForces, double rotationUnit) { 
		return ::gpk::initOrbiterOrbit(orbitCenter, orbitForces, bodyData.Orbit.AxialTilt, bodyData.Orbit.RotationPeriod, rotationUnit); 
	}
	stainli	::gpk::error_t	initOrbiterCenter	(const ::gpk::SOrbiter & bodyData, ::gpk::SBodyCenter	& planetCenter, double distanceScale) {
		return ::gpk::initOrbiterCenter(planetCenter, bodyData.Orbit.Radius, distanceScale);
	}
	stainli	::gpk::error_t	initOrbiterBody		(const ::gpk::SOrbiter & bodyData, ::gpk::SBodyCenter	& planetCenter, ::gpk::SBodyForces & planetForces, double rotationUnit) {
		return ::gpk::initOrbiterBody(planetCenter, planetForces, bodyData.Body.AxialTilt, bodyData.Body.RotationPeriod, rotationUnit);
	}
	stainli	::gpk::error_t	initOrbiterOrbit	(const ::gpk::SOrbiter & bodyData, ::gpk::SRigidBodyIntegrator & integrator, int32_t rigidBodyIndex, double rotationUnit) {
		return ::gpk::initOrbiterOrbit(integrator.Centers[rigidBodyIndex], integrator.Forces[rigidBodyIndex], bodyData.Orbit.AxialTilt, bodyData.Orbit.RotationPeriod, rotationUnit); 
	}
	stainli	::gpk::error_t	initOrbiterCenter	(const ::gpk::SOrbiter & bodyData, ::gpk::SRigidBodyIntegrator & integrator, int32_t rigidBodyIndex, double distanceScale) {
		return ::gpk::initOrbiterCenter(integrator.Centers[rigidBodyIndex], bodyData.Orbit.Radius, distanceScale);
	}
	stainli	::gpk::error_t	initOrbiterBody		(const ::gpk::SOrbiter & bodyData, ::gpk::SRigidBodyIntegrator & integrator, int32_t rigidBodyIndex, double rotationUnit) {
		return ::gpk::initOrbiterOrbit(integrator.Centers[rigidBodyIndex], integrator.Forces[rigidBodyIndex], bodyData.Body.AxialTilt, bodyData.Body.RotationPeriod, rotationUnit);
	}

	// deprecated
	::gpk::error_t			createOrbiter
		( ::gpk::SRigidBodyIntegrator	& bodies
		, double	orbital_inclination
		, double	orbital_period
		, double	orbital_unit
		, double	mass
		, double	axialTilt
		, double	distance
		, double	distance_scale
		, double	rotation_period
		, double	rotation_unit
		);
} // namespace

#endif // GPK_ENGINE_ORBIT_H_23701
