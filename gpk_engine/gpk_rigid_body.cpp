#include "gpk_rigid_body.h"
#include "gpk_apod_serialize.h"
#include "gpk_axis.h"

GPK_USING_TYPEINT	();
GPK_USING_VIEW		();
GPK_USING_APOD		();
GPK_USING_N2		();
GPK_USING_N3		();


gpk::err_t 		gpk::SRigidBodyIntegrator::Load	(vcu0_t & input) { 
	if_fail_fe(gpk::loadView(input, Frames			));
	if_fail_fe(gpk::loadView(input, Flags			));
	if_fail_fe(gpk::loadView(input, Forces			));
	if_fail_fe(gpk::loadView(input, Masses			));
	if_fail_fe(gpk::loadView(input, Centers			));
	if_fail_fe(gpk::loadView(input, TransformsLocal	));
	return 0;											
}

gpk::err_t 		gpk::SRigidBodyIntegrator::Save	(au0_t & output) const { 
	if_fail_fe(gpk::saveView(output, Frames			));
	if_fail_fe(gpk::saveView(output, Flags			));
	if_fail_fe(gpk::saveView(output, Forces			));
	if_fail_fe(gpk::saveView(output, Masses			));
	if_fail_fe(gpk::saveView(output, Centers		));
	if_fail_fe(gpk::saveView(output, TransformsLocal));

	info_printf("Saved %s, %i", "Frames"			, Frames			.size());
	info_printf("Saved %s, %i", "Flags"				, Flags				.size());
	info_printf("Saved %s, %i", "Forces"			, Forces			.size());
	info_printf("Saved %s, %i", "Masses"			, Masses			.size());
	info_printf("Saved %s, %i", "Transforms"		, Centers			.size());
	info_printf("Saved %s, %i", "TransformsLocal"	, TransformsLocal	.size());
	return 0; 
}

void			gpk::transformInertiaTensor	(m3f2_t & iitWorld, const m3f2_t & iitBody, const m4f2_t & rotmat)		{
	const f2_t					t4							= rotmat._11*iitBody._11 + rotmat._21*iitBody._12 + rotmat._31*iitBody._13;
	const f2_t					t9							= rotmat._11*iitBody._21 + rotmat._21*iitBody._22 + rotmat._31*iitBody._23;
	const f2_t					t14							= rotmat._11*iitBody._31 + rotmat._21*iitBody._32 + rotmat._31*iitBody._33;
	const f2_t					t28							= rotmat._12*iitBody._11 + rotmat._22*iitBody._12 + rotmat._32*iitBody._13;
	const f2_t					t33							= rotmat._12*iitBody._21 + rotmat._22*iitBody._22 + rotmat._32*iitBody._23;
	const f2_t					t38							= rotmat._12*iitBody._31 + rotmat._22*iitBody._32 + rotmat._32*iitBody._33;
	const f2_t					t52							= rotmat._13*iitBody._11 + rotmat._23*iitBody._12 + rotmat._33*iitBody._13;
	const f2_t					t57							= rotmat._13*iitBody._21 + rotmat._23*iitBody._22 + rotmat._33*iitBody._23;
	const f2_t					t62							= rotmat._13*iitBody._31 + rotmat._23*iitBody._32 + rotmat._33*iitBody._33;

	iitWorld._11			= t4 *rotmat._11 + t9 *rotmat._21 + t14*rotmat._31;
	iitWorld._21			= t4 *rotmat._12 + t9 *rotmat._22 + t14*rotmat._32;
	iitWorld._31			= t4 *rotmat._13 + t9 *rotmat._23 + t14*rotmat._33;
	iitWorld._12			= t28*rotmat._11 + t33*rotmat._21 + t38*rotmat._31;
	iitWorld._22			= t28*rotmat._12 + t33*rotmat._22 + t38*rotmat._32;
	iitWorld._32			= t28*rotmat._13 + t33*rotmat._23 + t38*rotmat._33;
	iitWorld._13			= t52*rotmat._11 + t57*rotmat._21 + t62*rotmat._31;
	iitWorld._23			= t52*rotmat._12 + t57*rotmat._22 + t62*rotmat._32;
	iitWorld._33			= t52*rotmat._13 + t57*rotmat._23 + t62*rotmat._33;
}

void					gpk::updateTransform	(SBodyCenter & bodyTransform, m4f2_t & transformLocal)	{
	//transformLocal.Scale( bodyTransform.Scale, true );
	bodyTransform.Orientation.Normalize();
	transformLocal.SetOrientation(bodyTransform.Orientation);
	transformLocal.SetTranslation(bodyTransform.Position, false);
}

gpk::err_t 		gpk::integrateForces	(f3_t duration, SBodyFrame & bodyFrame, SBodyForces & bodyForce, const SBodyMass & bodyMass) {
	// -- Calculate linear acceleration from force inputs.
	bodyFrame.LastFrameAcceleration	= bodyForce.Acceleration;
	bodyFrame.LastFrameAcceleration.AddScaled(bodyFrame.AccumulatedForce, bodyMass.InverseMass);
	// -- Adjust velocities
	bodyForce.Velocity.AddScaled(bodyFrame.LastFrameAcceleration, (f2_t)duration);	// Update linear velocity from both acceleration and impulse.

	// -- Calculate angular acceleration from torque inputs.
	n3f2_c			angularAcceleration		= bodyFrame.InverseInertiaTensorWorld.Transform(bodyFrame.AccumulatedTorque);
	bodyForce.Rotation.AddScaled(angularAcceleration, duration);	// Update angular velocity from both acceleration and impulse.

	// Impose drag.
	bodyForce.Velocity		*= pow((f3_t)bodyMass.LinearDamping , duration);
	bodyForce.Rotation		*= pow((f3_t)bodyMass.AngularDamping, duration);
	bodyFrame.ClearAccumulators();
	return 0;
}

gpk::err_t 		gpk::integratePosition	(f3_t duration, f3_t durationHalfSquared, SBodyFlags & bodyFlags, SBodyCenter & bodyTransform, const SBodyForces & bodyForces) {
	bodyTransform.Position		.AddScaled(bodyForces.Velocity, duration);
	bodyTransform.Position		.AddScaled(bodyForces.Velocity, durationHalfSquared);
	bodyTransform.Orientation	.AddScaled(bodyForces.Rotation, duration);	// Update angular position.
	bodyTransform.Orientation.Normalize();
	bodyFlags.UpdatedTransform		= false;
	bodyFlags.UpdatedTensorWorld	= false;
	return 0;
}
