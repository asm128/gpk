#include "gpk_axis.h"
#include "gpk_quat.h"

#ifndef GPK_ORBTIER_H
#define GPK_ORBTIER_H

namespace gpk
{

#pragma pack(push, 1)
	struct SOrbit {
		float		Radius				= {}; // 57.9	// (10^6 km)	// 7
		float		AxialTilt			= {}; // degrees
		float		RotationPeriod		= {}; // in units
		float		RotationUnit		= {}; // 
		::gpk::AXIS	RotationAxis		= ::gpk::AXIS_Y_POSITIVE; // 
		::gpk::AXIS	TiltAxis			= ::gpk::AXIS_X_POSITIVE; // 
	};

	struct SOrbiter {
		SOrbit			Body;
		SOrbit			Orbit;
	};
#pragma pack(pop)

	stincxp	double			degreesToRadians		(double degrees) { return ::gpk::math_2pi / 360.0 * degrees; }

	// Calculate the axial inclination of the planet IN RADIANS
	template<typename __tAxis>
	static ::gpk::error_t	orientationFromEuler	(::gpk::AXIS iAxis, double axialTilt, ::gpk::quat<__tAxis> & orientation) {
		const __tAxis				rotationValue			= __tAxis(degreesToRadians(axialTilt));
		switch(iAxis) {
		case ::gpk::AXIS_X_NEGATIVE: orientation.MakeFromEuler(-rotationValue, 0, 0); break;
		case ::gpk::AXIS_X_POSITIVE: orientation.MakeFromEuler( rotationValue, 0, 0); break;
		case ::gpk::AXIS_Y_NEGATIVE: orientation.MakeFromEuler(0, -rotationValue, 0); break;
		case ::gpk::AXIS_Y_POSITIVE: orientation.MakeFromEuler(0,  rotationValue, 0); break;
		case ::gpk::AXIS_Z_NEGATIVE: orientation.MakeFromEuler(0, 0, -rotationValue); break;
		case ::gpk::AXIS_Z_POSITIVE: orientation.MakeFromEuler(0, 0,  rotationValue); break;
		}
		orientation.Normalize();
		return 0;
	}

	// Calculate the rotation velocity of the planet IN EARTH DAYS
	template<typename _tAxis, typename _tAxisOrientation>
	static ::gpk::error_t	orbitRotation			(::gpk::AXIS iAxis, double rotation_period, double rotation_unit, const ::gpk::quat<_tAxisOrientation> & orientation, ::gpk::n3<_tAxis> & rotation) {
		const _tAxis				rotationValue			= _tAxis(::gpk::math_2pi / rotation_period * rotation_unit);
		switch(iAxis) {
		case ::gpk::AXIS_X_NEGATIVE: rotation = {-rotationValue, 0, 0}; break;
		case ::gpk::AXIS_X_POSITIVE: rotation = { rotationValue, 0, 0}; break;
		case ::gpk::AXIS_Y_NEGATIVE: rotation = {0, -rotationValue, 0}; break;
		case ::gpk::AXIS_Y_POSITIVE: rotation = {0,  rotationValue, 0}; break;
		case ::gpk::AXIS_Z_NEGATIVE: rotation = {0, 0, -rotationValue}; break;
		case ::gpk::AXIS_Z_POSITIVE: rotation = {0, 0,  rotationValue}; break;
		}
		rotation				= orientation.RotateVector(rotation);		// Rotate our calculated torque in relation to the planetary axis
		return 0;
	}

	// Calculate the rotation velocity of the planet IN EARTH DAYS
	template<typename _tAxis, typename _tAxisOrientation>
	static ::gpk::error_t	orbitCenter			(::gpk::AXIS iAxis, double position, ::gpk::n3<_tAxis> & center) {
		switch(iAxis) {
		case ::gpk::AXIS_X_NEGATIVE: center = {-position, 0, 0}; break;
		case ::gpk::AXIS_X_POSITIVE: center = { position, 0, 0}; break;
		case ::gpk::AXIS_Y_NEGATIVE: center = {0, -position, 0}; break;
		case ::gpk::AXIS_Y_POSITIVE: center = {0,  position, 0}; break;
		case ::gpk::AXIS_Z_NEGATIVE: center = {0, 0, -position}; break;
		case ::gpk::AXIS_Z_POSITIVE: center = {0, 0,  position}; break;
		}
		return 0;
	}

} // namespace

#endif // GPK_ORBTIER_H