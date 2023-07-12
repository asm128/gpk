#include "gpk_celestial_body.h"
#include "gpk_engine.h"
#include "gpk_json.h"
#include "gpk_view_color.h"

#ifndef GPK_ENGINE_CELESTIAL_BODY_H_23701
#define GPK_ENGINE_CELESTIAL_BODY_H_23701

namespace gpk
{
	stainli	::gpk::error_t	initOrbiterOrbit	(const ::gpk::SCelestialBody & bodyData, ::gpk::SBodyCenter	& orbitCenter, ::gpk::SBodyForces & orbitForces, double rotationUnit) { 
		return ::gpk::initOrbiterOrbit(orbitCenter, orbitForces, bodyData.OrbitalInclination, bodyData.OrbitalPeriod, rotationUnit); 
	}
	stainli	::gpk::error_t	initOrbiterCenter	(const ::gpk::SCelestialBody & bodyData, ::gpk::SBodyCenter	& planetCenter, double distanceScale) {
		return ::gpk::initOrbiterCenter(planetCenter, bodyData.OrbitRadius, distanceScale);
	}
	stainli	::gpk::error_t	initOrbiterBody		(const ::gpk::SCelestialBody & bodyData, ::gpk::SBodyCenter	& planetCenter, ::gpk::SBodyForces & planetForces, double rotationUnit) {
		return ::gpk::initOrbiterBody(planetCenter, planetForces, bodyData.ObliquityToOrbit, bodyData.RotationPeriod, rotationUnit);
	}
	stainli	::gpk::error_t	initOrbiterOrbit	(const ::gpk::SCelestialBody & bodyData, ::gpk::SRigidBodyIntegrator & integrator, int32_t rigidBodyIndex, double rotationUnit) {
		return ::gpk::initOrbiterOrbit(integrator.Centers[rigidBodyIndex], integrator.Forces[rigidBodyIndex], bodyData.OrbitalInclination, bodyData.OrbitalPeriod, rotationUnit); 
	}
	stainli	::gpk::error_t	initOrbiterCenter	(const ::gpk::SCelestialBody & bodyData, ::gpk::SRigidBodyIntegrator & integrator, int32_t rigidBodyIndex, double distanceScale) {
		return ::gpk::initOrbiterCenter(integrator.Centers[rigidBodyIndex], bodyData.OrbitRadius, distanceScale);
	}
	stainli	::gpk::error_t	initOrbiterBody		(const ::gpk::SCelestialBody & bodyData, ::gpk::SRigidBodyIntegrator & integrator, int32_t rigidBodyIndex, double rotationUnit) {
		return ::gpk::initOrbiterBody(integrator.Centers[rigidBodyIndex], integrator.Forces[rigidBodyIndex], bodyData.ObliquityToOrbit, bodyData.RotationPeriod, rotationUnit);
	}
} // namespace

#endif // GPK_ENGINE_CELESTIAL_BODY_H_23701
