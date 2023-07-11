#include "gpk_celestial_body.h"
#include "gpk_engine.h"
#include "gpk_json.h"
#include "gpk_view_color.h"

#ifndef GPK_ENGINE_CELESTIAL_BODY_H_23701
#define GPK_ENGINE_CELESTIAL_BODY_H_23701

namespace gpk
{
	stainli	::gpk::error_t	initOrbiterOrbit			(const ::gpk::SCelestialBody & bodyData, ::gpk::SBodyCenter	& orbitCenter, ::gpk::SBodyForces & orbitForces)	{ return ::gpk::initOrbiterOrbit(orbitCenter, orbitForces, bodyData.OrbitalInclination, bodyData.OrbitalPeriod); }
	stainli	::gpk::error_t	initOrbiterOrbit			(const ::gpk::SCelestialBody & bodyData, ::gpk::SRigidBodyIntegrator & integrator, int32_t rigidBodyIndex)		{ return ::gpk::initOrbiterOrbit(integrator.Centers[rigidBodyIndex], integrator.Forces[rigidBodyIndex], bodyData.OrbitalInclination, bodyData.OrbitalPeriod); }
	stainli	::gpk::error_t	initOrbiterBody				(const ::gpk::SCelestialBody & bodyData, ::gpk::SBodyCenter	& planetCenter, ::gpk::SBodyForces & planetForces, ::gpk::SBodyMass	& planetMass, double rotationUnit) {
		return ::gpk::initOrbiterMassBody
			( planetCenter
			, planetForces
			, planetMass
			, bodyData.Mass
			, bodyData.ObliquityToOrbit
			, bodyData.RotationPeriod
			, rotationUnit
			);
	}
	stainli	::gpk::error_t	initOrbiterBody
		( const SCelestialBody			& bodyData
		, ::gpk::SRigidBodyIntegrator	& integrator
		, int32_t						rigidBodyIndex
		, double						rotationUnit
		) {
		return ::gpk::initOrbiterMassBody
			( integrator.Centers[rigidBodyIndex]
			, integrator.Forces [rigidBodyIndex]
			, integrator.Masses [rigidBodyIndex]
			, bodyData.Mass
			, bodyData.ObliquityToOrbit
			, bodyData.RotationPeriod
			, rotationUnit
			);
	}
	stainli	::gpk::error_t	initOrbiterGravityCenter	(const ::gpk::SCelestialBody & bodyData, ::gpk::SBodyCenter	& planetCenter, ::gpk::SBodyForces & planetForces, ::gpk::SBodyMass	& planetMass, double distanceScale) {
		return ::gpk::initOrbiterGravityCenter
			( planetCenter
			, planetForces
			, planetMass
			, bodyData.DistanceFromParent
			, distanceScale
			);
	}
	stainli	::gpk::error_t	initOrbiterGravityCenter
		( const SCelestialBody			& bodyData
		, ::gpk::SRigidBodyIntegrator	& integrator
		, int32_t						rigidBodyIndex
		, double						distanceScale
		) {
		return ::gpk::initOrbiterGravityCenter
			( integrator.Centers[rigidBodyIndex]
			, integrator.Forces [rigidBodyIndex]
			, integrator.Masses [rigidBodyIndex]
			, bodyData.DistanceFromParent
			, distanceScale
			);
	}
	stainli	::gpk::error_t	initOrbiterBodyWithGravityCenter(const ::gpk::SCelestialBody & bodyData, ::gpk::SBodyCenter	& planetCenter, ::gpk::SBodyForces & planetForces, ::gpk::SBodyMass	& planetMass, float distanceScale, float rotationUnit) {
		return ::gpk::initOrbiterBodyWithGravityCenter
			( planetCenter
			, planetForces
			, planetMass
			, bodyData.Mass
			, bodyData.ObliquityToOrbit
			, bodyData.DistanceFromParent
			, distanceScale
			, bodyData.RotationPeriod
			, rotationUnit
			);
	}
	stainli	::gpk::error_t	initOrbiterBodyWithGravityCenter
		( const SCelestialBody			& bodyData
		, ::gpk::SRigidBodyIntegrator	& integrator
		, int32_t						rigidBodyIndex
		, float							distanceScale
		, float							rotationUnit
		) {
		return ::gpk::initOrbiterBodyWithGravityCenter
			( integrator.Centers[rigidBodyIndex]
			, integrator.Forces [rigidBodyIndex]
			, integrator.Masses [rigidBodyIndex]
			, bodyData.Mass
			, bodyData.ObliquityToOrbit
			, bodyData.DistanceFromParent
			, distanceScale
			, bodyData.RotationPeriod
			, rotationUnit
			);
	}
} // namespace

#endif // GPK_ENGINE_PLANETARY_SYSTEM_H_23701
