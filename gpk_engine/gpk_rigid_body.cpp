#include "gpk_rigid_body.h"
#include "gpk_apod_serialize.h"
#include "gpk_axis.h"

::gpk::error_t			gpk::SRigidBodyIntegrator::Load	(::gpk::vcu8 & input) { 
	gpk_necs(::gpk::loadView(input, Frames			));
	gpk_necs(::gpk::loadView(input, Flags			));
	gpk_necs(::gpk::loadView(input, Forces			));
	gpk_necs(::gpk::loadView(input, Masses			));
	gpk_necs(::gpk::loadView(input, Centers			));
	gpk_necs(::gpk::loadView(input, TransformsLocal	));
	return 0;											
}

::gpk::error_t			gpk::SRigidBodyIntegrator::Save	(::gpk::au8 & output) const { 
	gpk_necs(::gpk::saveView(output, Frames			));
	gpk_necs(::gpk::saveView(output, Flags			));
	gpk_necs(::gpk::saveView(output, Forces			));
	gpk_necs(::gpk::saveView(output, Masses			));
	gpk_necs(::gpk::saveView(output, Centers		));
	gpk_necs(::gpk::saveView(output, TransformsLocal));

	info_printf("Saved %s, %i", "Frames"			, Frames			.size());
	info_printf("Saved %s, %i", "Flags"				, Flags				.size());
	info_printf("Saved %s, %i", "Forces"			, Forces			.size());
	info_printf("Saved %s, %i", "Masses"			, Masses			.size());
	info_printf("Saved %s, %i", "Transforms"		, Centers			.size());
	info_printf("Saved %s, %i", "TransformsLocal"	, TransformsLocal	.size());
	return 0; 
}

void					gpk::transformInertiaTensor	(::gpk::m3<float> & iitWorld, const ::gpk::m3<float> &iitBody, const ::gpk::m4f32 &rotmat)		{
	const float					t4							= rotmat._11*iitBody._11 + rotmat._21*iitBody._12 + rotmat._31*iitBody._13;
	const float					t9							= rotmat._11*iitBody._21 + rotmat._21*iitBody._22 + rotmat._31*iitBody._23;
	const float					t14							= rotmat._11*iitBody._31 + rotmat._21*iitBody._32 + rotmat._31*iitBody._33;
	const float					t28							= rotmat._12*iitBody._11 + rotmat._22*iitBody._12 + rotmat._32*iitBody._13;
	const float					t33							= rotmat._12*iitBody._21 + rotmat._22*iitBody._22 + rotmat._32*iitBody._23;
	const float					t38							= rotmat._12*iitBody._31 + rotmat._22*iitBody._32 + rotmat._32*iitBody._33;
	const float					t52							= rotmat._13*iitBody._11 + rotmat._23*iitBody._12 + rotmat._33*iitBody._13;
	const float					t57							= rotmat._13*iitBody._21 + rotmat._23*iitBody._22 + rotmat._33*iitBody._23;
	const float					t62							= rotmat._13*iitBody._31 + rotmat._23*iitBody._32 + rotmat._33*iitBody._33;

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

void					gpk::updateTransform	(::gpk::SBodyCenter & bodyTransform, ::gpk::m4f32 & transformLocal)	{
	//transformLocal.Scale( bodyTransform.Scale, true );
	bodyTransform.Orientation.Normalize();
	transformLocal.SetOrientation(bodyTransform.Orientation);
	transformLocal.SetTranslation(bodyTransform.Position, false);
}

::gpk::error_t			gpk::integrateForces	(double duration, ::gpk::SBodyFrame& bodyFrame, ::gpk::SBodyForces & bodyForce, const ::gpk::SBodyMass & bodyMass) {
	// -- Calculate linear acceleration from force inputs.
	bodyFrame.LastFrameAcceleration	= bodyForce.Acceleration;
	bodyFrame.LastFrameAcceleration.AddScaled(bodyFrame.AccumulatedForce, bodyMass.InverseMass);
	// -- Adjust velocities
	bodyForce.Velocity.AddScaled(bodyFrame.LastFrameAcceleration, (float)duration);	// Update linear velocity from both acceleration and impulse.

	// -- Calculate angular acceleration from torque inputs.
	const ::gpk::n3f32			angularAcceleration		= bodyFrame.InverseInertiaTensorWorld.Transform(bodyFrame.AccumulatedTorque);
	bodyForce.Rotation.AddScaled(angularAcceleration, duration);	// Update angular velocity from both acceleration and impulse.

	// Impose drag.
	bodyForce.Velocity		*= pow((double)bodyMass.LinearDamping , duration);
	bodyForce.Rotation		*= pow((double)bodyMass.AngularDamping, duration);
	bodyFrame.ClearAccumulators();
	return 0;
}

// ------------- Adjust positions
::gpk::error_t			gpk::integratePosition	(double duration, double durationHalfSquared, ::gpk::SBodyFlags& bodyFlags, ::gpk::SBodyCenter & bodyTransform, const ::gpk::SBodyForces & bodyForces) {
	bodyTransform.Position		.AddScaled(bodyForces.Velocity, duration);
	bodyTransform.Position		.AddScaled(bodyForces.Velocity, durationHalfSquared);
	bodyTransform.Orientation	.AddScaled(bodyForces.Rotation, duration);	// Update angular position.
	bodyTransform.Orientation.Normalize();
	bodyFlags.UpdatedTransform		= false;
	bodyFlags.UpdatedTensorWorld	= false;
	return 0;
}
