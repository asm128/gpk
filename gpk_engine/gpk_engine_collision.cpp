#include "gpk_engine.h"

GPK_USING_APOD();
GPK_USING_ASTATIC();
GPK_USING_TYPEINT();
GPK_USING_VIEW();
GPK_USING_N2();
GPK_USING_N3();

using gpk::string;


static	::gpk::err_t	detectAABB				(const ::gpk::SRigidBodyIntegrator & integrator, u2_t bodyA, u2_t bodyB, ::gpk::SContact & contactCache, ::gpk::apod<::gpk::SContact> & contactsDetected) { (void)integrator, (void)bodyA, (void)bodyB, (void)contactCache, (void)contactsDetected; return 0; }
static	::gpk::err_t	detectCylinder			(const ::gpk::SRigidBodyIntegrator & integrator, u2_t bodyA, u2_t bodyB, ::gpk::SContact & contactCache, ::gpk::apod<::gpk::SContact> & contactsDetected) { (void)integrator, (void)bodyA, (void)bodyB, (void)contactCache, (void)contactsDetected; return 0; }
static	::gpk::err_t	detectCylinderAABB		(const ::gpk::SRigidBodyIntegrator & integrator, u2_t cylinder, u2_t aabb, ::gpk::SContact & contactCache, ::gpk::apod<::gpk::SContact> & contactsDetected) { (void)integrator, (void)cylinder, (void)aabb, (void)contactCache, (void)contactsDetected; return 0; }

static	::gpk::err_t	detectSphere			(const ::gpk::SRigidBodyIntegrator & integrator, u2_t rigidBodyA, u2_t rigidBodyB, ::gpk::SContact & contactCache, ::gpk::apod<::gpk::SContact> & contactsDetected) {
	const ::gpk::n3f2_t			& radiusA				= integrator.BoundingVolumes[rigidBodyA].HalfSizes;
	const ::gpk::n3f2_t			& positionA				= integrator.Centers[rigidBodyA].Position;
	const ::gpk::n3f2_t			& radiusB				= integrator.BoundingVolumes[rigidBodyB].HalfSizes;
	const ::gpk::n3f2_t			& positionB				= integrator.Centers[rigidBodyB].Position;

	const float					maxDistance				= radiusA.x + radiusB.x;
	const float					collisionThreshold		= maxDistance * maxDistance;
	contactCache.CenterDistance	= positionB - positionA;
	const float					distanceSquared			= (float)contactCache.CenterDistance.LengthSquared();
	if(distanceSquared >= collisionThreshold) 
		return 0;

	contactCache.DistanceLength	= distanceSquared ? sqrt(distanceSquared) : 0.0f;
	if_fail_fe(contactsDetected.push_back(contactCache));
	return ::gpk::err_t(1 + rigidBodyB);
}

static	::gpk::err_t	detectSphereAABB		(const ::gpk::SRigidBodyIntegrator & integrator, u2_t bodySphere, u2_t bodyAABB    , ::gpk::SContact & contactCache, ::gpk::apod<::gpk::SContact> & contactsDetected) { (void)integrator, (void)bodySphere, (void)bodyAABB, (void)contactCache, (void)contactsDetected; return 0; }
static	::gpk::err_t	detectSphereCylinder	(const ::gpk::SRigidBodyIntegrator & integrator, u2_t bodySphere, u2_t bodyCylinder, ::gpk::SContact & contactCache, ::gpk::apod<::gpk::SContact> & contactsDetected) { (void)integrator, (void)bodySphere, (void)bodyCylinder, (void)contactCache, (void)contactsDetected; return 0; }
static	::gpk::err_t	detectSame				(const ::gpk::SRigidBodyIntegrator & integrator, ::gpk::BOUNDING_TYPE bvA, u2_t rigidBodyA, u2_t rigidBodyB, ::gpk::SContact & contactCache, ::gpk::apod<::gpk::SContact> & contactsDetected) {
	::gpk::err_t				result					= -1;
	switch(bvA) {
	default								: warning_printf("Unsupported type: (%i) %s.", bvA, ::gpk::get_value_namep(bvA)); break; 
	case ::gpk::BOUNDING_TYPE_Sphere	: result = ::detectSphere  (integrator, rigidBodyA, rigidBodyB, contactCache, contactsDetected); break; 
	case ::gpk::BOUNDING_TYPE_Cylinder	: result = ::detectCylinder(integrator, rigidBodyA, rigidBodyB, contactCache, contactsDetected); break; 
	case ::gpk::BOUNDING_TYPE_AABB		: result = ::detectAABB    (integrator, rigidBodyA, rigidBodyB, contactCache, contactsDetected); break; 
	} // switch bvA
	return result;
}

static	::gpk::err_t	collisionDetectAny		(const ::gpk::SRigidBodyIntegrator & integrator, ::gpk::BOUNDING_TYPE bvA, ::gpk::BOUNDING_TYPE bvB, u2_t rigidBodyA, u2_t rigidBodyB, ::gpk::SContact & contactCache, ::gpk::apod<::gpk::SContact> & contactsDetected) {
	::gpk::err_t				result					= 0;
	if(bvA == bvB) 
		return detectSame(integrator, bvA, rigidBodyA, rigidBodyB, contactCache, contactsDetected);

	switch(bvA) {
	case ::gpk::BOUNDING_TYPE_Sphere	: 
		switch(bvB) { // rigid body A is Sphere
		case ::gpk::BOUNDING_TYPE_AABB		: i_if(result = ::detectSphereAABB		(integrator, rigidBodyA, rigidBodyB, contactCache, contactsDetected)); break; 
		case ::gpk::BOUNDING_TYPE_Cylinder	: i_if(result = ::detectSphereCylinder	(integrator, rigidBodyA, rigidBodyB, contactCache, contactsDetected)); break; 
		}  // switch bvB
		break;
	case ::gpk::BOUNDING_TYPE_Cylinder	: 
		switch(bvB) { // rigid body A is Cylinder
		case ::gpk::BOUNDING_TYPE_AABB		: i_if(result = ::detectCylinderAABB  (integrator, rigidBodyA, rigidBodyB, contactCache, contactsDetected)); break; 
		case ::gpk::BOUNDING_TYPE_Sphere	: i_if(result = ::detectSphereCylinder(integrator, rigidBodyB, rigidBodyA, contactCache, contactsDetected)); break; 
		}  // switch bvB
		break;
	case ::gpk::BOUNDING_TYPE_AABB		: 
		switch(bvB) { // rigid body A is AABB
		case ::gpk::BOUNDING_TYPE_Cylinder	: i_if(result = ::detectCylinderAABB(integrator, rigidBodyB, rigidBodyA, contactCache, contactsDetected)); break; 
		case ::gpk::BOUNDING_TYPE_Sphere	: i_if(result = ::detectSphereAABB  (integrator, rigidBodyB, rigidBodyA, contactCache, contactsDetected)); break; 
		}  // switch bvB
		break;
	} // switch bvA
	return result;
}

::gpk::err_t			gpk::collisionDetect	(const ::gpk::SEngine & engine, double totalSeconds, ::gpk::apod<::gpk::SContact> & contactsDetected) {
	u2_t					count					= contactsDetected.size();
	for(::gpk::SContact	contact	= {totalSeconds}; contact.EntityA < engine.Entities.size(); ++contact.EntityA) {
		const ::gpk::SEntity	& entityA				= engine.Entities[contact.EntityA];
		if(entityA.RigidBody >= engine.Integrator.Flags.size())
			continue;

		const ::gpk::SBodyFlags		& flagsA				= engine.Integrator.Flags[entityA.RigidBody];
		if(false == flagsA.Collides)
			continue;

		for(contact.EntityB = contact.EntityA + 1; contact.EntityB < engine.Entities.size(); ++contact.EntityB) {
			const ::gpk::SEntity	& entityB				= engine.Entities[contact.EntityB];
			if(entityB.RigidBody >= engine.Integrator.Flags.size())
				continue;

			const ::gpk::SBodyFlags		& flagsB				= engine.Integrator.Flags[entityB.RigidBody];
			if(false == flagsB.Collides)
				continue;

			if_fail_fe(::collisionDetectAny(engine.Integrator, (::gpk::BOUNDING_TYPE)flagsA.BVType, (::gpk::BOUNDING_TYPE)flagsB.BVType, entityA.RigidBody, entityB.RigidBody, contact, contactsDetected));
		}
	}
	//ginfo_if(contactsDetected.size() - count, "%i collisions detected.", contactsDetected.size() - count);
	return contactsDetected.size() - count;
}

//				"\nEntityA       : %i"
//				"\nEntityB       : %i"
//				"\nCenterDistance: " N3_F32
//				"\nDistanceLength: %f"
//				, contact.EntityA
//				, contact.EntityB
//				, gpk_xyz(contact.CenterDistance)
//				, contact.DistanceLength
//				);
