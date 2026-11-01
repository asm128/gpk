#include "gpk_matrix.h"
#include "gpk_quat.h"
#include "gpk_tri3.h"

#ifndef GPK_MATRIX3_H_23627
#define GPK_MATRIX3_H_23627

namespace gpk
{
#pragma pack(push, 1)
	tplt<tpnm T, MATRIX_MATH _math = MATRIX_MATH_ROW_VECTOR, MATRIX_LAYOUT _layout = MATRIX_LAYOUT_ROW_MAJOR>
	stct m3a3 : m3<T> {
		tydf T							TValue;
		tydf m3<T>					TStorage;
		tydf m3a3<T, _math, _layout>	TMatrix;
		tydf n3<T>					TCoord;
		usng TStorage::Value;

		cxpr T      &	oper()		(u0_t row, u0_t column)			nxpt	{ rtrn Value[matrixIndex<3, _layout>(row, column)]; }
		cxpr cnst T &	oper()		(u0_t row, u0_t column)	csnx	{ rtrn Value[matrixIndex<3, _layout>(row, column)]; }
		cxpr T      &	MathElement	(u0_t row, u0_t column)			nxpt	{ rtrn MATRIX_MATH_ROW_VECTOR == _math ? (*this)(row, column) : (*this)(column, row); }
		cxpr cnst T &	MathElement	(u0_t row, u0_t column)	csnx	{ rtrn MATRIX_MATH_ROW_VECTOR == _math ? (*this)(row, column) : (*this)(column, row); }

		cxpr bool	oper==	(cnst TMatrix & other)	csnx	{ rtrn matrixEqual<3>(*this, other); }
		cxpr bool	oper!=	(cnst TMatrix & other)	csnx	{ rtrn false == operator==(other); }
		cxpr TMatrix	oper+	(cnst TMatrix & other)	csnx	{ rtrn matrixAdd<3>(*this, other); }
		cxpr TMatrix	oper-	(cnst TMatrix & other)	csnx	{ rtrn matrixSubtract<3>(*this, other); }
		cxpr TMatrix	oper*	(f3_t scalar)			csnx	{ rtrn matrixScale<3>(*this, scalar); }
		     TMatrix	oper/	(f3_t scalar)			cnst		{ rtrn matrixScale<3>(*this, 1.0 / scalar); }
		cxpr TMatrix	oper*	(cnst TMatrix & other)	csnx	{ rtrn matrixMultiply<3>(*this, other); }

		TMatrix &	oper+=	(cnst TMatrix & other)	nxpt	{ rtrn *this = *this + other; }
		TMatrix &	oper-=	(cnst TMatrix & other)	nxpt	{ rtrn *this = *this - other; }
		TMatrix &	oper*=	(f3_t scalar)				nxpt	{ rtrn *this = *this * scalar; }
		TMatrix &	oper/=	(f3_t scalar)						{ rtrn *this = *this / scalar; }
		TMatrix &	oper*=	(cnst TMatrix & other)	nxpt	{ rtrn *this = *this * other; }

		stxp TMatrix	GetIdentity	() nxpt	{ rtrn matrixIdentity<3, TMatrix>(); }
		cxpr TMatrix	GetTranspose	() csnx	{ rtrn matrixTranspose<3>(*this); }
		     TMatrix	GetInverse	() cnst		{ rtrn matrixInverse<3>(*this); }
		cxpr f3_t	GetDeterminant	() csnx	{ rtrn matrixDeterminant<3>(*this); }

		void	Identity		() nxpt	{ *this = GetIdentity(); }
		void	Transpose		(cnst TMatrix & other) nxpt	{ *this = other.GetTranspose(); }
		void	Invert			()					{ *this = GetInverse(); }
		TMatrix & LinearInterpolate(cnst TMatrix & first, cnst TMatrix & second, f3_t factor) nxpt	{ rtrn *this = matrixInterpolate<3>(first, second, factor); }

		TCoord	Transform	(cnst TCoord & value) csnx {
			if(MATRIX_MATH_ROW_VECTOR == _math)
				rtrn
					{ (T)(value.x * (*this)(0, 0) + value.y * (*this)(1, 0) + value.z * (*this)(2, 0))
					, (T)(value.x * (*this)(0, 1) + value.y * (*this)(1, 1) + value.z * (*this)(2, 1))
					, (T)(value.x * (*this)(0, 2) + value.y * (*this)(1, 2) + value.z * (*this)(2, 2))
					};
			rtrn
				{ (T)((*this)(0, 0) * value.x + (*this)(0, 1) * value.y + (*this)(0, 2) * value.z)
				, (T)((*this)(1, 0) * value.x + (*this)(1, 1) * value.y + (*this)(1, 2) * value.z)
				, (T)((*this)(2, 0) * value.x + (*this)(2, 1) * value.y + (*this)(2, 2) * value.z)
				};
		}
		TCoord	TransformInverse(cnst TCoord & value) csnx	{ rtrn GetTranspose().Transform(value); }

		void	RotationX	(f3_t angle) nxpt {
			Identity();
			cnst SSinCos angleSinCos = getSinCos(angle);
			MathElement(1, 1) = (T)angleSinCos.Cos;	MathElement(1, 2) = (T)angleSinCos.Sin;
			MathElement(2, 1) = (T)-angleSinCos.Sin;	MathElement(2, 2) = (T)angleSinCos.Cos;
		}
		void	RotationY	(f3_t angle) nxpt {
			Identity();
			cnst SSinCos angleSinCos = getSinCos(angle);
			MathElement(0, 0) = (T)angleSinCos.Cos;	MathElement(0, 2) = (T)-angleSinCos.Sin;
			MathElement(2, 0) = (T)angleSinCos.Sin;	MathElement(2, 2) = (T)angleSinCos.Cos;
		}
		void	RotationZ	(f3_t angle) nxpt {
			Identity();
			cnst SSinCos angleSinCos = getSinCos(angle);
			MathElement(0, 0) = (T)angleSinCos.Cos;	MathElement(0, 1) = (T)angleSinCos.Sin;
			MathElement(1, 0) = (T)-angleSinCos.Sin;	MathElement(1, 1) = (T)angleSinCos.Cos;
		}
		void	Scale		(cnst TCoord & scale, bool eraseContent) nxpt {
			if(eraseContent)
				Identity();
			MathElement(0, 0) = (T)(MathElement(0, 0) * scale.x);
			MathElement(1, 1) = (T)(MathElement(1, 1) * scale.y);
			MathElement(2, 2) = (T)(MathElement(2, 2) * scale.z);
		}
		void	Rotation	(cnst TCoord & angles) nxpt {
			cnst SSinCos yaw = getSinCos(angles.z), pitch = getSinCos(angles.y), roll = getSinCos(angles.x);
			MathElement(0, 0) = (T)(pitch.Cos * yaw.Cos);
			MathElement(0, 1) = (T)(pitch.Cos * yaw.Sin);
			MathElement(0, 2) = (T)-pitch.Sin;
			MathElement(1, 0) = (T)(roll.Sin * pitch.Sin * yaw.Cos - roll.Cos * yaw.Sin);
			MathElement(1, 1) = (T)(roll.Sin * pitch.Sin * yaw.Sin + roll.Cos * yaw.Cos);
			MathElement(1, 2) = (T)(roll.Sin * pitch.Cos);
			MathElement(2, 0) = (T)(roll.Cos * pitch.Sin * yaw.Cos + roll.Sin * yaw.Sin);
			MathElement(2, 1) = (T)(roll.Cos * pitch.Sin * yaw.Sin - roll.Sin * yaw.Cos);
			MathElement(2, 2) = (T)(roll.Cos * pitch.Cos);
		}
		void	RotationArbitraryAxis(cnst TCoord & sourceAxis, T angle) {
			cnst SSinCos angleSinCos = getSinCos(angle);
			TCoord axis = sourceAxis;
			if(1 != axis.LengthSquared())
				axis.Normalize();
			cnst f3_t complement = 1 - angleSinCos.Cos;
			MathElement(0, 0) = (T)(axis.x * axis.x * complement + angleSinCos.Cos);
			MathElement(0, 1) = (T)(axis.x * axis.y * complement - axis.z * angleSinCos.Sin);
			MathElement(0, 2) = (T)(axis.x * axis.z * complement + axis.y * angleSinCos.Sin);
			MathElement(1, 0) = (T)(axis.y * axis.x * complement + axis.z * angleSinCos.Sin);
			MathElement(1, 1) = (T)(axis.y * axis.y * complement + angleSinCos.Cos);
			MathElement(1, 2) = (T)(axis.y * axis.z * complement - axis.x * angleSinCos.Sin);
			MathElement(2, 0) = (T)(axis.z * axis.x * complement - axis.y * angleSinCos.Sin);
			MathElement(2, 1) = (T)(axis.z * axis.y * complement + axis.x * angleSinCos.Sin);
			MathElement(2, 2) = (T)(axis.z * axis.z * complement + angleSinCos.Cos);
		}
		void	SetOrientation(cnst quat<T> & orientation) nxpt {
			cnst f3_t x2 = orientation.x + orientation.x, y2 = orientation.y + orientation.y, z2 = orientation.z + orientation.z;
			cnst f3_t xx = orientation.x * x2, xy = orientation.x * y2, xz = orientation.x * z2;
			cnst f3_t yy = orientation.y * y2, yz = orientation.y * z2, zz = orientation.z * z2;
			cnst f3_t wx = orientation.w * x2, wy = orientation.w * y2, wz = orientation.w * z2;
			MathElement(0, 0) = (T)(1 - yy - zz);	MathElement(0, 1) = (T)(xy + wz);		MathElement(0, 2) = (T)(xz - wy);
			MathElement(1, 0) = (T)(xy - wz);		MathElement(1, 1) = (T)(1 - xx - zz);	MathElement(1, 2) = (T)(yz + wx);
			MathElement(2, 0) = (T)(xz + wy);		MathElement(2, 1) = (T)(yz - wx);		MathElement(2, 2) = (T)(1 - xx - yy);
		}

		void	SetCoeffsAngularMass(f3_t ix, f3_t iy, f3_t iz, f3_t ixy, f3_t ixz, f3_t iyz) nxpt {
			(*this)(0, 0) = (T) ix;	(*this)(0, 1) = (*this)(1, 0) = (T)-ixy;	(*this)(0, 2) = (*this)(2, 0) = (T)-ixz;
			(*this)(1, 1) = (T) iy;	(*this)(1, 2) = (*this)(2, 1) = (T)-iyz;	(*this)(2, 2) = (T) iz;
		}
		void	SetBlockAngularMass(cnst TCoord & halfSizes, f3_t mass) nxpt {
			cnst f3_t massFactor = mass / 3;
			SetCoeffsAngularMass
				( massFactor * (halfSizes.y * halfSizes.y + halfSizes.z * halfSizes.z)
				, massFactor * (halfSizes.x * halfSizes.x + halfSizes.z * halfSizes.z)
				, massFactor * (halfSizes.x * halfSizes.x + halfSizes.y * halfSizes.y)
				, 0, 0, 0
				);
		}
	};

	tplt<tpnm T, MATRIX_MATH _math = MATRIX_MATH_ROW_VECTOR, MATRIX_LAYOUT _layout = MATRIX_LAYOUT_ROW_MAJOR>
	stct m4 {
		tydf T						TValue;
		tydf m4<T, _math, _layout>	TMatrix;
		tydf n3<T>				TCoord;

		T	Value[16]	= {};

		cxpr T      &	oper()		(u0_t row, u0_t column)			nxpt	{ rtrn Value[matrixIndex<4, _layout>(row, column)]; }
		cxpr cnst T &	oper()		(u0_t row, u0_t column)	csnx	{ rtrn Value[matrixIndex<4, _layout>(row, column)]; }
		cxpr T      &	MathElement	(u0_t row, u0_t column)			nxpt	{ rtrn MATRIX_MATH_ROW_VECTOR == _math ? (*this)(row, column) : (*this)(column, row); }
		cxpr cnst T &	MathElement	(u0_t row, u0_t column)	csnx	{ rtrn MATRIX_MATH_ROW_VECTOR == _math ? (*this)(row, column) : (*this)(column, row); }

		cxpr bool	oper==	(cnst TMatrix & other)	csnx	{ rtrn matrixEqual<4>(*this, other); }
		cxpr bool	oper!=	(cnst TMatrix & other)	csnx	{ rtrn false == operator==(other); }
		cxpr TMatrix	oper+	(cnst TMatrix & other)	csnx	{ rtrn matrixAdd<4>(*this, other); }
		cxpr TMatrix	oper-	(cnst TMatrix & other)	csnx	{ rtrn matrixSubtract<4>(*this, other); }
		cxpr TMatrix	oper*	(f3_t scalar)			csnx	{ rtrn matrixScale<4>(*this, scalar); }
		     TMatrix	oper/	(f3_t scalar)			cnst		{ rtrn matrixScale<4>(*this, 1.0 / scalar); }
		cxpr TMatrix	oper*	(cnst TMatrix & other)	csnx	{ rtrn matrixMultiply<4>(*this, other); }

		TMatrix &	oper+=	(cnst TMatrix & other)	nxpt	{ rtrn *this = *this + other; }
		TMatrix &	oper-=	(cnst TMatrix & other)	nxpt	{ rtrn *this = *this - other; }
		TMatrix &	oper*=	(f3_t scalar)				nxpt	{ rtrn *this = *this * scalar; }
		TMatrix &	oper/=	(f3_t scalar)						{ rtrn *this = *this / scalar; }
		TMatrix &	oper*=	(cnst TMatrix & other)	nxpt	{ rtrn *this = *this * other; }

		stxp TMatrix	GetIdentity	() nxpt	{ rtrn matrixIdentity<4, TMatrix>(); }
		cxpr TMatrix	GetTranspose	() csnx	{ rtrn matrixTranspose<4>(*this); }
		     TMatrix	GetInverse	() cnst		{ rtrn matrixInverse<4>(*this); }
		cxpr f3_t	GetDeterminant	() csnx	{ rtrn matrixDeterminant<4>(*this); }
		cxpr TCoord	GetTranslation	() csnx	{ rtrn {MathElement(3, 0), MathElement(3, 1), MathElement(3, 2)}; }

		void	Identity		() nxpt	{ *this = GetIdentity(); }
		void	SetTranspose	(cnst TMatrix & other) nxpt	{ *this = other.GetTranspose(); }
		void	Transpose		(cnst TMatrix & other) nxpt	{ *this = other.GetTranspose(); }
		void	SetInverse		(cnst TMatrix & other)		{ *this = other.GetInverse(); }
		void	Invert			()					{ *this = GetInverse(); }
		TMatrix & LinearInterpolate(cnst TMatrix & first, cnst TMatrix & second, f3_t factor) nxpt	{ rtrn *this = matrixInterpolate<4>(first, second, factor); }

		TCoord	InverseTranslate(cnst TCoord & value) csnx	{ rtrn value - GetTranslation(); }
		void	InverseTranslateInPlace(TCoord & value) csnx	{ value -= GetTranslation(); }
		TCoord	Transform(cnst TCoord & value) csnx {
			f3_t x = {}, y = {}, z = {}, w = {};
			if(MATRIX_MATH_ROW_VECTOR == _math) {
				x = value.x * (*this)(0, 0) + value.y * (*this)(1, 0) + value.z * (*this)(2, 0) + (*this)(3, 0);
				y = value.x * (*this)(0, 1) + value.y * (*this)(1, 1) + value.z * (*this)(2, 1) + (*this)(3, 1);
				z = value.x * (*this)(0, 2) + value.y * (*this)(1, 2) + value.z * (*this)(2, 2) + (*this)(3, 2);
				w = value.x * (*this)(0, 3) + value.y * (*this)(1, 3) + value.z * (*this)(2, 3) + (*this)(3, 3);
			}
			else {
				x = (*this)(0, 0) * value.x + (*this)(0, 1) * value.y + (*this)(0, 2) * value.z + (*this)(0, 3);
				y = (*this)(1, 0) * value.x + (*this)(1, 1) * value.y + (*this)(1, 2) * value.z + (*this)(1, 3);
				z = (*this)(2, 0) * value.x + (*this)(2, 1) * value.y + (*this)(2, 2) * value.z + (*this)(2, 3);
				w = (*this)(3, 0) * value.x + (*this)(3, 1) * value.y + (*this)(3, 2) * value.z + (*this)(3, 3);
			}
			rtrn {(T)(x / w), (T)(y / w), (T)(z / w)};
		}
		TCoord	TransformDirection(cnst TCoord & value) csnx {
			if(MATRIX_MATH_ROW_VECTOR == _math)
				rtrn
					{ (T)(value.x * (*this)(0, 0) + value.y * (*this)(1, 0) + value.z * (*this)(2, 0))
					, (T)(value.x * (*this)(0, 1) + value.y * (*this)(1, 1) + value.z * (*this)(2, 1))
					, (T)(value.x * (*this)(0, 2) + value.y * (*this)(1, 2) + value.z * (*this)(2, 2))
					};
			rtrn
				{ (T)((*this)(0, 0) * value.x + (*this)(0, 1) * value.y + (*this)(0, 2) * value.z)
				, (T)((*this)(1, 0) * value.x + (*this)(1, 1) * value.y + (*this)(1, 2) * value.z)
				, (T)((*this)(2, 0) * value.x + (*this)(2, 1) * value.y + (*this)(2, 2) * value.z)
				};
		}
		TCoord	TransformInverseDirection(cnst TCoord & value) csnx	{ rtrn GetTranspose().TransformDirection(value); }

		void	ViewportRH(cnst n2<u1_t> & metrics) nxpt {
			Identity();
			MathElement(0, 0) = (T)(metrics.x * .5);
			MathElement(1, 1) = (T)(metrics.y * .5);
			MathElement(3, 0) = (T)(metrics.x * .5);
			MathElement(3, 1) = (T)(metrics.y * .5);
		}
		void	ViewportLH(cnst n2<u1_t> & metrics) nxpt {
			Identity();
			MathElement(0, 0) = (T)(metrics.x * .5);
			MathElement(1, 1) = (T)(metrics.y * -.5);
			MathElement(3, 0) = (T)(metrics.x * .5);
			MathElement(3, 1) = (T)(metrics.y * .5);
		}
		void	RotationX(f3_t angle) nxpt {
			Identity();
			cnst SSinCos angleSinCos = getSinCos(angle);
			MathElement(1, 1) = (T)angleSinCos.Cos;	MathElement(1, 2) = (T)angleSinCos.Sin;
			MathElement(2, 1) = (T)-angleSinCos.Sin;	MathElement(2, 2) = (T)angleSinCos.Cos;
		}
		void	RotationY(f3_t angle) nxpt {
			Identity();
			cnst SSinCos angleSinCos = getSinCos(angle);
			MathElement(0, 0) = (T)angleSinCos.Cos;	MathElement(0, 2) = (T)-angleSinCos.Sin;
			MathElement(2, 0) = (T)angleSinCos.Sin;	MathElement(2, 2) = (T)angleSinCos.Cos;
		}
		void	RotationZ(f3_t angle) nxpt {
			Identity();
			cnst SSinCos angleSinCos = getSinCos(angle);
			MathElement(0, 0) = (T)angleSinCos.Cos;	MathElement(0, 1) = (T)angleSinCos.Sin;
			MathElement(1, 0) = (T)-angleSinCos.Sin;	MathElement(1, 1) = (T)angleSinCos.Cos;
		}
		void	Scale(cnst TCoord & scale, bool eraseContent) nxpt {
			if(eraseContent)
				Identity();
			MathElement(0, 0) = (T)(MathElement(0, 0) * scale.x);
			MathElement(1, 1) = (T)(MathElement(1, 1) * scale.y);
			MathElement(2, 2) = (T)(MathElement(2, 2) * scale.z);
		}
		void	SetTranslation(cnst TCoord & translation, bool eraseContent) nxpt {
			if(eraseContent)
				Identity();
			MathElement(3, 0) = translation.x;
			MathElement(3, 1) = translation.y;
			MathElement(3, 2) = translation.z;
		}
		tplt<tpnm TNearFar>
		void	FieldOfView(f3_t angle, f3_t aspect, cnst minmax<TNearFar> & nearFar)	{ FieldOfView(angle, aspect, nearFar.Min, nearFar.Max); }
		tplt<tpnm TNearFar>
		void	FieldOfView(f3_t angle, f3_t aspect, TNearFar nearValue, TNearFar farValue) {
			*this = {};
			cnst f3_t tangent = tan(angle / 2);
			MathElement(0, 0) = (T)(1 / (aspect * tangent));
			MathElement(1, 1) = (T)(1 / tangent);
			MathElement(2, 2) = (T)(farValue / (farValue - nearValue));
			MathElement(2, 3) = 1;
			MathElement(3, 2) = (T)(-(farValue * nearValue) / (farValue - nearValue));
		}
		void	LookAt(cnst TCoord & position, cnst TCoord & target, cnst TCoord & worldUp) {
			TCoord front = TCoord{target - position}.Normalize();
			TCoord right = worldUp.Cross(front).Normalize();
			TCoord up = front.Cross(right).Normalize();
			View3D(position, right, up, front);
		}
		void	View3D(cnst TCoord & position, cnst TCoord & right, cnst TCoord & up, cnst TCoord & front) {
			MathElement(0, 0) = right.x;	MathElement(0, 1) = up.x;	MathElement(0, 2) = front.x;	MathElement(0, 3) = 0;
			MathElement(1, 0) = right.y;	MathElement(1, 1) = up.y;	MathElement(1, 2) = front.y;	MathElement(1, 3) = 0;
			MathElement(2, 0) = right.z;	MathElement(2, 1) = up.z;	MathElement(2, 2) = front.z;	MathElement(2, 3) = 0;
			MathElement(3, 0) = (T)-position.Dot(right);
			MathElement(3, 1) = (T)-position.Dot(up);
			MathElement(3, 2) = (T)-position.Dot(front);
			MathElement(3, 3) = 1;
		}
		void	Billboard(cnst TCoord & position, cnst TCoord & direction, cnst TCoord & worldUp) {
			TCoord up = worldUp - direction * worldUp.Dot(direction);
			up.Normalize();
			cnst TCoord right = up.Cross(direction);
			MathElement(0, 0) = right.x;		MathElement(0, 1) = right.y;		MathElement(0, 2) = right.z;		MathElement(0, 3) = 0;
			MathElement(1, 0) = up.x;		MathElement(1, 1) = up.y;		MathElement(1, 2) = up.z;		MathElement(1, 3) = 0;
			MathElement(2, 0) = direction.x;	MathElement(2, 1) = direction.y;	MathElement(2, 2) = direction.z;	MathElement(2, 3) = 0;
			MathElement(3, 0) = position.x;	MathElement(3, 1) = position.y;	MathElement(3, 2) = position.z;	MathElement(3, 3) = 1;
		}
		void	Rotation(cnst TCoord & angles) nxpt {
			m3a3<T, _math, _layout> rotation = {};
			rotation.Rotation(angles);
			Identity();
			for(u0_t row = 0; row < 3; ++row) {
				for(u0_t column = 0; column < 3; ++column) {
					(*this)(row, column) = rotation(row, column);
				}
			}
		}
		void	RotationArbitraryAxis(cnst TCoord & axis, T angle) {
			m3a3<T, _math, _layout> rotation = {};
			rotation.RotationArbitraryAxis(axis, angle);
			Identity();
			for(u0_t row = 0; row < 3; ++row) {
				for(u0_t column = 0; column < 3; ++column) {
					(*this)(row, column) = rotation(row, column);
				}
			}
		}
		void	SetOrientation(cnst quat<T> & orientation) nxpt {
			m3a3<T, _math, _layout> rotation = {};
			rotation.SetOrientation(orientation);
			Identity();
			for(u0_t row = 0; row < 3; ++row) {
				for(u0_t column = 0; column < 3; ++column) {
					(*this)(row, column) = rotation(row, column);
				}
			}
		}
		TMatrix & FromRotationDir(n3f2_c & direction, n3f2_c & up = {0, 1, 0}) {
			n3f2_t right = up.Cross(direction);
			right.Normalize();
			n3f2_t localUp = direction.Cross(right);
			localUp.Normalize();
			Identity();
			MathElement(0, 0) = (T)right.x;		MathElement(1, 0) = (T)localUp.x;		MathElement(2, 0) = (T)direction.x;
			MathElement(0, 1) = (T)right.y;		MathElement(1, 1) = (T)localUp.y;		MathElement(2, 1) = (T)direction.y;
			MathElement(0, 2) = (T)right.z;		MathElement(1, 2) = (T)localUp.z;		MathElement(2, 2) = (T)direction.z;
			rtrn *this;
		}
	};

	tplt<tpnm T, MATRIX_MATH _math, MATRIX_LAYOUT _layout>
	tri3<T> & transform(tri3<T> & triangle, cnst m4<T, _math, _layout> & matrix) {
		triangle.A = matrix.Transform(triangle.A);
		triangle.B = matrix.Transform(triangle.B);
		triangle.C = matrix.Transform(triangle.C);
		rtrn triangle;
	}

	tplt<tpnm T, MATRIX_MATH _math, MATRIX_LAYOUT _layout>
	tri3<T> & transformDirection(tri3<T> & triangle, cnst m4<T, _math, _layout> & matrix) {
		triangle.A = matrix.TransformDirection(triangle.A);
		triangle.B = matrix.TransformDirection(triangle.B);
		triangle.C = matrix.TransformDirection(triangle.C);
		rtrn triangle;
	}

	tydf m3a3<uc_t>	m3a3uc_t;	tdcs m3a3uc_t	m3a3uc_c;
	tydf m3a3<sc_t>	m3a3sc_t;	tdcs m3a3sc_t	m3a3sc_c;
	tydf m3a3<u0_t>	m3a3u0_t;	tdcs m3a3u0_t	m3a3u0_c;
	tydf m3a3<u1_t>	m3a3u1_t;	tdcs m3a3u1_t	m3a3u1_c;
	tydf m3a3<u2_t>	m3a3u2_t;	tdcs m3a3u2_t	m3a3u2_c;
	tydf m3a3<u3_t>	m3a3u3_t;	tdcs m3a3u3_t	m3a3u3_c;
	tydf m3a3<s0_t>	m3a3s0_t;	tdcs m3a3s0_t	m3a3s0_c;
	tydf m3a3<s1_t>	m3a3s1_t;	tdcs m3a3s1_t	m3a3s1_c;
	tydf m3a3<s2_t>	m3a3s2_t;	tdcs m3a3s2_t	m3a3s2_c;
	tydf m3a3<s3_t>	m3a3s3_t;	tdcs m3a3s3_t	m3a3s3_c;
	tydf m3a3<f2_t>	m3a3f2_t;	tdcs m3a3f2_t	m3a3f2_c;
	tydf m3a3<f3_t>	m3a3f3_t;	tdcs m3a3f3_t	m3a3f3_c;

	tydf m4<uc_t>	m4uc_t;	tdcs m4uc_t	m4uc_c;
	tydf m4<sc_t>	m4sc_t;	tdcs m4sc_t	m4sc_c;
	tydf m4<u0_t>	m4u0_t;	tdcs m4u0_t	m4u0_c;
	tydf m4<u1_t>	m4u1_t;	tdcs m4u1_t	m4u1_c;
	tydf m4<u2_t>	m4u2_t;	tdcs m4u2_t	m4u2_c;
	tydf m4<u3_t>	m4u3_t;	tdcs m4u3_t	m4u3_c;
	tydf m4<s0_t>	m4s0_t;	tdcs m4s0_t	m4s0_c;
	tydf m4<s1_t>	m4s1_t;	tdcs m4s1_t	m4s1_c;
	tydf m4<s2_t>	m4s2_t;	tdcs m4s2_t	m4s2_c;
	tydf m4<s3_t>	m4s3_t;	tdcs m4s3_t	m4s3_c;
	tydf m4<f2_t>	m4f2_t;	tdcs m4f2_t	m4f2_c;
	tydf m4<f3_t>	m4f3_t;	tdcs m4f3_t	m4f3_c;

#pragma pack(pop)
}

#endif // GPK_MATRIX3_H_23627
