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

void			gpk::transformInertiaTensor	(m3a3f2_t & iitWorld, const m3a3f2_t & iitBody, const m4f2_t & rotmat)		{
	m3a3f2_t					rotation						= {};
	for(u0_t row = 0; row < 3; ++row)
		for(u0_t column = 0; column < 3; ++column)
			rotation.MathElement(row, column) = rotmat.MathElement(row, column);
	iitWorld					= rotation.GetTranspose() * iitBody * rotation;
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
