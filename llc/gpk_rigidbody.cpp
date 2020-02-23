#include "gpk_rigidbody.h"

void												gpk::transformInertiaTensor			(::gpk::SMatrix3<float> & iitWorld, const ::gpk::SMatrix3<float> &iitBody, const ::gpk::SMatrix4<float> &rotmat)		{
	float													t4									= rotmat._11*iitBody._11 + rotmat._21*iitBody._12 + rotmat._31*iitBody._13;
	float													t9									= rotmat._11*iitBody._21 + rotmat._21*iitBody._22 + rotmat._31*iitBody._23;
	float													t14									= rotmat._11*iitBody._31 + rotmat._21*iitBody._32 + rotmat._31*iitBody._33;
	float													t28									= rotmat._12*iitBody._11 + rotmat._22*iitBody._12 + rotmat._32*iitBody._13;
	float													t33									= rotmat._12*iitBody._21 + rotmat._22*iitBody._22 + rotmat._32*iitBody._23;
	float													t38									= rotmat._12*iitBody._31 + rotmat._22*iitBody._32 + rotmat._32*iitBody._33;
	float													t52									= rotmat._13*iitBody._11 + rotmat._23*iitBody._12 + rotmat._33*iitBody._13;
	float													t57									= rotmat._13*iitBody._21 + rotmat._23*iitBody._22 + rotmat._33*iitBody._23;
	float													t62									= rotmat._13*iitBody._31 + rotmat._23*iitBody._32 + rotmat._33*iitBody._33;

	iitWorld._11										= t4 *rotmat._11 + t9 *rotmat._21 + t14*rotmat._31;
	iitWorld._21										= t4 *rotmat._12 + t9 *rotmat._22 + t14*rotmat._32;
	iitWorld._31										= t4 *rotmat._13 + t9 *rotmat._23 + t14*rotmat._33;
	iitWorld._12										= t28*rotmat._11 + t33*rotmat._21 + t38*rotmat._31;
	iitWorld._22										= t28*rotmat._12 + t33*rotmat._22 + t38*rotmat._32;
	iitWorld._32										= t28*rotmat._13 + t33*rotmat._23 + t38*rotmat._33;
	iitWorld._13										= t52*rotmat._11 + t57*rotmat._21 + t62*rotmat._31;
	iitWorld._23										= t52*rotmat._12 + t57*rotmat._22 + t62*rotmat._32;
	iitWorld._33										= t52*rotmat._13 + t57*rotmat._23 + t62*rotmat._33;
}

void												gpk::updateTransform					(::gpk::STransform3 & bodyTransform, ::gpk::SMatrix4<float> & transformLocal)	{
	//transformLocal.Scale( bodyTransform.Scale, true );
	bodyTransform.Orientation.Normalize();
	transformLocal.SetOrientation( bodyTransform.Orientation );
	transformLocal.SetTranslation( bodyTransform.Position, false );
}

int32_t												gpk::integrateForces					(double duration, ::gpk::SRigidBodyFrame& bodyFrame, ::gpk::SForce3 & bodyForce, const ::gpk::SMass3 & bodyMass) {
	// -- Calculate linear acceleration from force inputs.
	bodyFrame.LastFrameAcceleration						= bodyForce.Acceleration;
	bodyFrame.LastFrameAcceleration.AddScaled(bodyFrame.AccumulatedForce, bodyMass.InverseMass);
	// -- Adjust velocities
	bodyForce.Velocity.AddScaled(bodyFrame.LastFrameAcceleration, (float)duration);	// Update linear velocity from both acceleration and impulse.

	// -- Calculate angular acceleration from torque inputs.
	::gpk::SCoord3<float>									angularAcceleration						= bodyFrame.InverseInertiaTensorWorld.Transform(bodyFrame.AccumulatedTorque);
	bodyForce.Rotation.AddScaled(angularAcceleration, duration);	// Update angular velocity from both acceleration and impulse.

	// Impose drag.
	bodyForce.Velocity									*= pow((double)bodyMass.LinearDamping , duration);
	bodyForce.Rotation									*= pow((double)bodyMass.AngularDamping, duration);
	bodyFrame.ClearAccumulators();
	return 0;
}

// ------------- Adjust positions
int32_t												gpk::integratePosition				(double duration, double durationHalfSquared, ::gpk::SRigidBodyFlags& bodyFlags, ::gpk::STransform3 & bodyTransform, const ::gpk::SForce3 & bodyForces) {
	bodyTransform.Position		.AddScaled(bodyForces.Velocity, duration);
	bodyTransform.Position		.AddScaled(bodyForces.Velocity, durationHalfSquared);
	bodyTransform.Orientation	.AddScaled(bodyForces.Rotation, duration);	// Update angular position.
	bodyFlags.OutdatedTransform = bodyFlags.OutdatedTensorWorld	= true;	// Normalize the orientation, and update the matrices with the new position and orientation.
	return 0;
}

int													gpk::createOrbiter
	( ::gpk::SIntegrator3	& bodies
	, double				mass
	, double				distance
	, double				axialTilt
	, double				rotation_period
	, double				rotation_unit
	, double				orbital_period
	, double				orbital_inclination
	, double				distance_scale
	) {
	const int32_t											indexFirstBody					= bodies.Spawn(2);
	{
		::gpk::STransform3										& orbitTransform				= bodies.Transforms	[indexFirstBody]		= {};
		::gpk::SForce3											& orbitForces					= bodies.Forces		[indexFirstBody]		= {};
		orbitForces.Rotation								= {0.0f, (float)(::gpk::math_2pi / orbital_period), 0.0f};			// Set the orbital rotation velocity IN EARTH DAYS
		orbitTransform.Orientation.MakeFromEulerTaitBryan( (float)(::gpk::math_2pi / 360.0 * orbital_inclination), 0.0f, 0.0f );	// Calculate the orbital tilt IN RADIANS
	}
	{
		::gpk::SMass3											& planetMass					= bodies.Masses		[indexFirstBody + 1]	= {};
		::gpk::STransform3										& planetTransform				= bodies.Transforms	[indexFirstBody + 1]	= {};
		::gpk::SForce3											& planetForces					= bodies.Forces		[indexFirstBody + 1]	= {};
		planetMass.InverseMass								= float(1.0 / mass);
		planetTransform.Position.x							= float(distance_scale * distance);

		planetTransform.Orientation.MakeFromEulerTaitBryan((float)(::gpk::math_2pi / 360.0 * axialTilt), 0, 0);					// Calculate the axial inclination of the planet IN RADIANS

		planetForces.Rotation								= { 0.0f, -(float)(::gpk::math_2pi / rotation_unit * rotation_period), 0.0f };	// Calculate the rotation velocity of the planet IN EARTH DAYS
		planetForces.Rotation								= planetTransform.Orientation.RotateVector(planetForces.Rotation);		// Rotate our calculated torque in relation to the planetary axis
	}
	return indexFirstBody;
}
