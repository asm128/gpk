#include "gpk_quat.h"
#include "gpk_tri3.h"

#ifndef GPK_MATRIX_H_23627
#define GPK_MATRIX_H_23627

namespace gpk
{
#pragma pack(push, 1)
	tplt<tpnm _tBase>
	struct m4 {
		typedef				m4<_tBase>	_tMat4, TMatrix4;
		typedef				n3<_tBase>			Tn3;
							_tBase				_11, _12, _13, _14
								,				_21, _22, _23, _24
								,				_31, _32, _33, _34
								,				_41, _42, _43, _44
								;

		inline				Tn3&				operator[]					(uint32_t index)				{ return *(Tn3*)((&_11) + 4 * index); }
		constexpr			bool				operator ==					(const _tMat4& other)															const	noexcept	{ return _11 == other._11 && _12 == other._12 && _13 == other._13 && _14 == other._14 && _21 == other._21 && _22 == other._22 && _23 == other._23 && _24 == other._24 && _31 == other._31 && _32 == other._32 && _33 == other._33 && _34 == other._34 && _41 == other._41 && _42 == other._42 && _43 == other._43 && _44 == other._44; }
		constexpr inline	bool				operator !=					(const _tMat4& other)															const	noexcept	{ return !operator==(other); }

		constexpr			_tMat4				operator +					(const _tMat4& other)															const	noexcept	{ return {_11 + other._11, _12 + other._12, _13 + other._13, _14 + other._14, _21 + other._21	, _22 + other._22, _23 + other._23, _24 + other._24, _31 + other._31, _32 + other._32, _33 + other._33, _34 + other._34, _41 + other._41, _42 + other._42, _43 + other._43, _44 + other._44}; }
		constexpr			_tMat4				operator -					(const _tMat4& other)															const	noexcept	{ return {_11 - other._11, _12 - other._12, _13 - other._13, _14 - other._14, _21 - other._21	, _22 - other._22, _23 - other._23, _24 - other._24, _31 - other._31, _32 - other._32, _33 - other._33, _34 - other._34, _41 - other._41, _42 - other._42, _43 - other._43, _44 - other._44}; }
		constexpr			_tMat4				operator *					(double scalar)																	const	noexcept	{return {(_tBase)(_11 * scalar), (_tBase)(_12 * scalar), (_tBase)(_13 * scalar), (_tBase)(_14 * scalar), (_tBase)(_21 * scalar), (_tBase)(_22 * scalar), (_tBase)(_23 * scalar), (_tBase)(_24 * scalar), (_tBase)(_31 * scalar), (_tBase)(_32 * scalar), (_tBase)(_33 * scalar), (_tBase)(_34 * scalar), (_tBase)(_41 * scalar), (_tBase)(_42 * scalar), (_tBase)(_43 * scalar), (_tBase)(_44 * scalar) }; }
							_tMat4				operator /					(double scalar)																	const				{ return {(_tBase)(_11 / scalar), (_tBase)(_12 / scalar), (_tBase)(_13 / scalar), (_tBase)(_14 / scalar), (_tBase)(_21 / scalar), (_tBase)(_22 / scalar), (_tBase)(_23 / scalar), (_tBase)(_24 / scalar), (_tBase)(_31 / scalar), (_tBase)(_32 / scalar), (_tBase)(_33 / scalar), (_tBase)(_34 / scalar), (_tBase)(_41 / scalar), (_tBase)(_42 / scalar), (_tBase)(_43 / scalar), (_tBase)(_44 / scalar) }; }
		constexpr			_tMat4				operator *					(const _tMat4& right)															const	noexcept	{
			return
				{ _11*right._11 + _12*right._21 + _13*right._31 + _14*right._41, _11*right._12 + _12*right._22 + _13*right._32 + _14*right._42, _11*right._13 + _12*right._23 + _13*right._33 + _14*right._43, _11*right._14 + _12*right._24 + _13*right._34 + _14*right._44
				, _21*right._11 + _22*right._21 + _23*right._31 + _24*right._41, _21*right._12 + _22*right._22 + _23*right._32 + _24*right._42, _21*right._13 + _22*right._23 + _23*right._33 + _24*right._43, _21*right._14 + _22*right._24 + _23*right._34 + _24*right._44
				, _31*right._11 + _32*right._21 + _33*right._31 + _34*right._41, _31*right._12 + _32*right._22 + _33*right._32 + _34*right._42, _31*right._13 + _32*right._23 + _33*right._33 + _34*right._43, _31*right._14 + _32*right._24 + _33*right._34 + _34*right._44
				, _41*right._11 + _42*right._21 + _43*right._31 + _44*right._41, _41*right._12 + _42*right._22 + _43*right._32 + _44*right._42, _41*right._13 + _42*right._23 + _43*right._33 + _44*right._43, _41*right._14 + _42*right._24 + _43*right._34 + _44*right._44
				};
		}
		inline				_tMat4&				operator +=					(const _tMat4& other)																	noexcept	{ return *this = operator+(other	); }
		inline				_tMat4&				operator -=					(const _tMat4& other)																	noexcept	{ return *this = operator-(other	); }
		inline				_tMat4&				operator *=					(double scalar)																			noexcept	{ return *this = operator*(scalar	); }
		inline				_tMat4&				operator /=					(double scalar)			{ return *this = operator/(scalar	); }
		inline				_tMat4&				operator *=					(const _tMat4& right)																	noexcept	{ return *this = operator*(right	); }

		inline				void				SetTranspose				(const _tMat4& m)				{ *this = m.GetTranspose(); }
		inline				void				SetInverse					(const _tMat4& m)				{ *this = m.GetInverse();	}
		inline				void				Transpose					(const _tMat4& m)				{ *this = GetTranspose();	}
		inline				void				Invert						()						{ *this = GetInverse();		}

		inline				_tMat4&				LinearInterpolate			(const _tMat4&p, const _tMat4&q, double fTime)											noexcept	{ return *this = ((q-p)*fTime)+p; }
		constexpr			Tn3					InverseTranslate			(const Tn3 & vec)															const	noexcept	{ return { vec.x - _41, vec.y - _42, vec.z - _43 }; }
							void				InverseTranslateInPlace		(Tn3& vec)																	const	noexcept	{ vec.x -= _41; vec.y -= _42; vec.z -= _43; }
		constexpr			Tn3					Transform					(const Tn3 & v)																const				{
			return
			Tn3
				{	(v.x*_11 + v.y*_21 + v.z*_31 + _41)	// x
				,	(v.x*_12 + v.y*_22 + v.z*_32 + _42)	// y
				,	(v.x*_13 + v.y*_23 + v.z*_33 + _43)	// z
				}	// GVECTOR3
				/	(v.x*_14 + v.y*_24 + v.z*_34 + _44)	// w
			;
		}
		constexpr			Tn3				TransformDirection			(const Tn3 & vector)														const	noexcept	{
			return
				{	vector.x * _11 + vector.y * _21 + vector.z * _31
				,	vector.x * _12 + vector.y * _22 + vector.z * _32
				,	vector.x * _13 + vector.y * _23 + vector.z * _33
				};
		}
		constexpr			Tn3				TransformInverseDirection	(const Tn3 & _v)															const	noexcept	{
			return
				{	_v.x * _11 + _v.y * _12 + _v.z * _13
				,	_v.x * _21 + _v.y * _22 + _v.z * _23
				,	_v.x * _31 + _v.y * _32 + _v.z * _33
				};
		}
		//- Rotate avector using the inverse of the matrix
		//inline				void				InverseRotateInPlace	(Tn3& vector)				const			{ fpVec = InverseRotate(vector); }
		//						Tn3				InverseRotate			(const Tn3 & fpVec)			const
		//{
		//	return
		//	{	fpVec.x * _11 + fpVec.y * _21 + fpVec.z * _31
		//	,	fpVec.x * _12 + fpVec.y * _22 + fpVec.z * _32
		//	,	fpVec.x * _13 + fpVec.y * _23 + fpVec.z * _33
		//	};
		//}
							void				ViewportRH				(const ::gpk::n2<uint16_t> & offscreenMetrics)			noexcept	{
			_11 = (_tBase)(offscreenMetrics.x * .5f);	_12 =										_13 =				_14 =
			_21 = (_tBase)0;							_22 = (_tBase)(offscreenMetrics.y * .5f);	_23 =				_24 =
			_31 =										_32 = (_tBase)0;							_33 = (_tBase)1;	_34 =
			_41 =										_42 = (_tBase)0;							_43 = (_tBase)0;	_44 = (_tBase)1;
			_41											+= offscreenMetrics.x * .5f;
			_42											+= offscreenMetrics.y * .5f;
		}
							void				ViewportLH					(const ::gpk::n2<uint16_t> & offscreenMetrics)			noexcept	{
			_11 = (_tBase)(offscreenMetrics.x * .5f);	_12 =										_13 =				_14 =
			_21 = (_tBase)0;							_22 = -(_tBase)(offscreenMetrics.y * .5f);	_23 =				_24 =
			_31 =										_32 = (_tBase)0;							_33 = (_tBase)1;	_34 = 0;
			_41 = (_tBase)(offscreenMetrics.x * .5f);	_42 = (_tBase)(offscreenMetrics.y * .5f);	_43 = (_tBase)0;	_44 = (_tBase)1;
		}

		static	constexpr	_tMat4				GetIdentity					()			noexcept	{
			return
				{ (_tBase)1,  (_tBase)0,  (_tBase)0,  (_tBase)0
				, (_tBase)0,  (_tBase)1,  (_tBase)0,  (_tBase)0
				, (_tBase)0,  (_tBase)0,  (_tBase)1,  (_tBase)0
				, (_tBase)0,  (_tBase)0,  (_tBase)0,  (_tBase)1
				};
		}
							void				SetIdentity					()			noexcept	{
			*this									=
				{ (_tBase)1,  (_tBase)0,  (_tBase)0,  (_tBase)0
				, (_tBase)0,  (_tBase)1,  (_tBase)0,  (_tBase)0
				, (_tBase)0,  (_tBase)0,  (_tBase)1,  (_tBase)0
				, (_tBase)0,  (_tBase)0,  (_tBase)0,  (_tBase)1
				};
		}
		inline				void				Identity					()			noexcept	{ SetIdentity(); }
							void				RotationX					(double angle)																			noexcept	{
			::gpk::SSinCos							angleSinCos					= ::gpk::getSinCos(angle);
			_11 = (_tBase)1;	_12 =							_13 =
			_31 = (_tBase)0;	_32 = -(_tBase)angleSinCos.Sin;	_33 = (_tBase)angleSinCos.Cos;
			_21 = (_tBase)0;	_22 =  (_tBase)angleSinCos.Cos;	_23 = (_tBase)angleSinCos.Sin;
			_41 = _42 = _43 = _14 = _24 = _34 = (_tBase)0; _44 = (_tBase)1;
		}
							void				RotationY					(double angle)																			noexcept	{
			::gpk::SSinCos							angleSinCos					= ::gpk::getSinCos(angle);
			_11 = (_tBase)angleSinCos.Cos;	_12 = (_tBase)0;	_13 = -(_tBase)angleSinCos.Sin;
			_21 = (_tBase)0;				_22 = (_tBase)1;	_23 =  (_tBase)0;
			_31 = (_tBase)angleSinCos.Sin;	_32 = (_tBase)0;	_33 =  (_tBase)angleSinCos.Cos;
			//
			_41 = _42 = _43 = _14 = _24 = _34 = (_tBase)0; _44 = (_tBase)1;
		}
							void				RotationZ					(double angle)																			noexcept	{
			::gpk::SSinCos							angleSinCos					= ::gpk::getSinCos(angle);
			_11 =  (_tBase)angleSinCos.Cos;	_12 = (_tBase)angleSinCos.Sin;	_13 = (_tBase)0;
			_21 = -(_tBase)angleSinCos.Sin;	_22 = (_tBase)angleSinCos.Cos;	_23 =
			_31 =							_32 = (_tBase)0;				_33 = (_tBase)1;
			//
			_41 = _42 = _43 = _14 = _24 = _34 = (_tBase)0; _44 = (_tBase)1;
		}
							void				Scale						(_tBase x, _tBase y, _tBase z, bool bEraseContent)										noexcept	{ Scale({x, y, z}, bEraseContent); }
		inline				void				Scale		(const Tn3 & ypr, bool bEraseContent)												noexcept
		{
			if( bEraseContent ) {
				_11 = (_tBase)ypr.x;	_12 =					_13 =					_14 =
				_21 = (_tBase)0;		_22 = (_tBase)ypr.y;	_23 =					_24 =
				_31 =					_32 = (_tBase)0;		_33 = (_tBase)ypr.z;	_34 =
				_41 =					_42 =					_43 = (_tBase)0;		_44 = (_tBase)1;
			}
			else {
				_11 = (_tBase)(_11*ypr.x); _22 = (_tBase)(_22*ypr.y); _33 = (_tBase)(_33*ypr.z);
			}
		}
							void				SetTranslation				(const Tn3 & vTranslation, bool bEraseContent)										noexcept	{
			if( bEraseContent ) {
				_11 = (_tBase)1;	_12 =				_13 =				_14 =
				_21 = (_tBase)0;	_22 = (_tBase)1;	_23 =				_24 =
				_31 =				_32 = (_tBase)0;	_33 = (_tBase)1;	_34 = (_tBase)0;
				/*_41 =				_42 =				_43 */				_44 = (_tBase)1;
			}
			_41	= vTranslation.x; _42 = vTranslation.y; _43 = vTranslation.z;
		}
		tplt<tpnm _tNearFar>
		inline	void							FieldOfView					(double fAngle, double fAspect, const ::gpk::minmax<_tNearFar> & nearFar)	{ return FieldOfView(fAngle, fAspect, nearFar.Min, nearFar.Max); } // FoV
		tplt<tpnm _tNearFar>
		void									FieldOfView					(double fAngle, double fAspect, _tNearFar zNear, _tNearFar zFar)			{
			double										fTan						= tan( fAngle / 2.0 );
			_11 = (_tBase)(1.0 / ( fAspect * fTan )				);
			_22 = (_tBase)(1.0 / fTan							);
			_33 = (_tBase)(zFar / ( zFar - zNear )				);
			_34 = (_tBase)(1									);
			_43 = (_tBase)(( -(zFar * zNear) ) / (zFar - zNear)	);

			_12 = _13 = _14 = _21 = _23 = _24 = _31 = _32 = _41 = _42 = _44 = (_tBase)0;
		//	return *this;
		} // FoV
		void									LookAt		(const Tn3 & vPosition, const Tn3& vTarget, const Tn3& vUp)		{
			Tn3										F							= Tn3{vTarget - vPosition}.Normalize();
			Tn3										R							= vUp	.Cross(F).Normalize();
			Tn3										U							= F		.Cross(R).Normalize();

			_11 = R.x;	_12 = U.x;	_13 = F.x;	_14 = (_tBase)0;
			_21 = R.y;	_22 = U.y;	_23 = F.y;	_24 = (_tBase)0;
			_31 = R.z;	_32 = U.z;	_33 = F.z;	_34 = (_tBase)0;
			_41 = (_tBase)-vPosition.Dot(R); 	// x
			_42 = (_tBase)-vPosition.Dot(U); 	// y
			_43 = (_tBase)-vPosition.Dot(F);	// z
			_44 = (_tBase)1;
		}

							void				View3D		(const Tn3 & vPosition, const Tn3& vRight, const Tn3& vUp, const Tn3& vFront)	{
			_11 = vRight.x;	_12 = vUp.x; _13 = vFront.x; _14 = (_tBase)0;
			_21 = vRight.y;	_22 = vUp.y; _23 = vFront.y; _24 = (_tBase)0;
			_31 = vRight.z;	_32 = vUp.z; _33 = vFront.z; _34 = (_tBase)0;
			_41 = (_tBase)-vPosition.Dot(vRight	);
			_42 = (_tBase)-vPosition.Dot(vUp	);
			_43 = (_tBase)-vPosition.Dot(vFront	);
			_44 = (_tBase)1;
		}
							void				Billboard					(const Tn3 & vPos, const Tn3& vDir, const Tn3& vWorldUp)								{
			Tn3										vUp
				,										vRight
				;
			double										fAngle						= vWorldUp.Dot(vDir);
			vUp										= vWorldUp - vDir * fAngle;
			vUp.Normalize();
			vRight									= vUp.Cross( vDir );
			_11 = vRight.x;	_12 = vRight.y;	_13 = vRight.z;	_14=(_tBase)0;
			_21 = vUp.x; 	_22 = vUp.y; 	_23 = vUp.z; 	_24=(_tBase)0;
			_31 = vDir.x;	_32 = vDir.y;	_33 = vDir.z;	_34=(_tBase)0;
			_41 = vPos.x;	_42 = vPos.y;	_43 = vPos.z;	_44=(_tBase)1;
		} // Billboard
		inline				void				Rotation					(_tBase x, _tBase y, _tBase z)																		{ return Rotation({x, y, z}); }
							void				Rotation					(const Tn3 &vc)				{
			::gpk::SSinCos							yaw							= ::gpk::getSinCos(vc.z);
			::gpk::SSinCos							pitch						= ::gpk::getSinCos(vc.y);
			::gpk::SSinCos							roll						= ::gpk::getSinCos(vc.x);

			_14 = _24 = _34 = _41 = _42 = _43		= (_tBase)0;
			_44										= (_tBase)1;

			_11										= (_tBase)(pitch.Cos*yaw.Cos								);
			_12										= (_tBase)(pitch.Cos*yaw.Sin								);
			_13										= (_tBase)(-pitch.Sin										);
			_21										= (_tBase)(roll.Sin*pitch.Sin*yaw.Cos+roll.Cos*-yaw.Sin		);
			_22										= (_tBase)(roll.Sin*pitch.Sin*yaw.Sin+roll.Cos* yaw.Cos		);
			_23										= (_tBase)(roll.Sin*pitch.Cos								);
			_31										= (_tBase)(roll.Cos*pitch.Sin*yaw.Cos+-roll.Sin*-yaw.Sin	);
			_32										= (_tBase)(roll.Cos*pitch.Sin*yaw.Sin+-roll.Sin* yaw.Cos	);
			_33										= (_tBase)(roll.Cos*pitch.Cos								);

		//	return *this;
		} // Rota

							void				RotationArbitraryAxis		(const Tn3 & _vcAxis, _tBase a)																		{
			::gpk::SSinCos							pairSinCos					= ::gpk::getSinCos(a);
			Tn3										vcAxis						= _vcAxis;
			double										fSum						= 1.0 - pairSinCos.Cos;

			if( vcAxis.LengthSquared() != 1.0 )
				vcAxis.Normalize();

			_11										= (_tBase)(	(vcAxis.x * vcAxis.x) * fSum + pairSinCos.Cos				);
			_12										= (_tBase)(	(vcAxis.x * vcAxis.y) * fSum - (vcAxis.z * pairSinCos.Sin)	);
			_13										= (_tBase)(	(vcAxis.x * vcAxis.z) * fSum + (vcAxis.y * pairSinCos.Sin)	);

			_21										= (_tBase)(	(vcAxis.y * vcAxis.x) * fSum + (vcAxis.z * pairSinCos.Sin)	);
			_22										= (_tBase)(	(vcAxis.y * vcAxis.y) * fSum + pairSinCos.Cos 				);
			_23										= (_tBase)(	(vcAxis.y * vcAxis.z) * fSum - (vcAxis.x * pairSinCos.Sin)	);

			_31										= (_tBase)(	(vcAxis.z * vcAxis.x) * fSum - (vcAxis.y * pairSinCos.Sin)	);
			_32										= (_tBase)(	(vcAxis.z * vcAxis.y) * fSum + (vcAxis.x * pairSinCos.Sin)	);
			_33										= (_tBase)(	(vcAxis.z * vcAxis.z) * fSum + pairSinCos.Cos				);

			_14 = _24 = _34 = _41 = _42 = _43		= (_tBase)0;
			_44										= (_tBase)1;
		}

							void				SetOrientation				(const ::gpk::quat<_tBase>& qo)																	{
			// set matrix to identity
			_41 = _42 = _43 = _14 = _24 = _34		= (_tBase)0;
			_44										= (_tBase)1;

			double										wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

			x2										= qo.x + qo.x;
			y2										= qo.y + qo.y;
			z2										= qo.z + qo.z;

			xx										= qo.x * x2;
			xy										= qo.x * y2;
			xz										= qo.x * z2;

			yy										= qo.y * y2;
			yz										= qo.y * z2;
			zz										= qo.z * z2;

			wx										= qo.w * x2;
			wy										= qo.w * y2;
			wz										= qo.w * z2;

			_11										= (_tBase)(1.0 - (yy + zz)	);
			_21										= (_tBase)(xy - wz			);
			_31										= (_tBase)(xz + wy			);

			_12										= (_tBase)(xy + wz			);
			_22										= (_tBase)(1.0 - (xx + zz)	);
			_32										= (_tBase)(yz - wx			);

			_13										= (_tBase)(xz - wy			);
			_23										= (_tBase)(yz + wx			);
			_33										= (_tBase)(1.0 - (xx + yy)	);
		}

		cnstxpr	_tMat4		GetTranspose				()			const	noexcept	{ return {_11, _21, _31, _41,  _12, _22, _32, _42,  _13, _23, _33, _43,  _14, _24, _34, _44};	}
		inlcxpr	Tn3			GetTranslation				()			const	noexcept	{ return { _41, _42, _43 }; }
		_tMat4				GetInverse					()			const				{
			_tMat4					mTranspose					= GetTranspose()
				,					mResult						= *this
				;
			_tBase					fTemp[12];

			fTemp[ 0]			= mTranspose._33 * mTranspose._44;
			fTemp[ 1]			= mTranspose._34 * mTranspose._43;
			fTemp[ 2]			= mTranspose._32 * mTranspose._44;
			fTemp[ 3]			= mTranspose._34 * mTranspose._42;
			fTemp[ 4]			= mTranspose._32 * mTranspose._43;
			fTemp[ 5]			= mTranspose._33 * mTranspose._42;
			fTemp[ 6]			= mTranspose._31 * mTranspose._44;
			fTemp[ 7]			= mTranspose._34 * mTranspose._41;
			fTemp[ 8]			= mTranspose._31 * mTranspose._43;
			fTemp[ 9]			= mTranspose._33 * mTranspose._41;
			fTemp[10]			= mTranspose._31 * mTranspose._42;
			fTemp[11]			= mTranspose._32 * mTranspose._41;

			mResult._11			 = fTemp[0]*mTranspose._22 + fTemp[3]*mTranspose._23 + fTemp[4] *mTranspose._24;
			mResult._11			-= fTemp[1]*mTranspose._22 + fTemp[2]*mTranspose._23 + fTemp[5] *mTranspose._24;
			mResult._12			 = fTemp[1]*mTranspose._21 + fTemp[6]*mTranspose._23 + fTemp[9] *mTranspose._24;
			mResult._12			-= fTemp[0]*mTranspose._21 + fTemp[7]*mTranspose._23 + fTemp[8] *mTranspose._24;
			mResult._13			 = fTemp[2]*mTranspose._21 + fTemp[7]*mTranspose._22 + fTemp[10]*mTranspose._24;
			mResult._13			-= fTemp[3]*mTranspose._21 + fTemp[6]*mTranspose._22 + fTemp[11]*mTranspose._24;
			mResult._14			 = fTemp[5]*mTranspose._21 + fTemp[8]*mTranspose._22 + fTemp[11]*mTranspose._23;
			mResult._14			-= fTemp[4]*mTranspose._21 + fTemp[9]*mTranspose._22 + fTemp[10]*mTranspose._23;
			mResult._21			 = fTemp[1]*mTranspose._12 + fTemp[2]*mTranspose._13 + fTemp[5] *mTranspose._14;
			mResult._21			-= fTemp[0]*mTranspose._12 + fTemp[3]*mTranspose._13 + fTemp[4] *mTranspose._14;
			mResult._22			 = fTemp[0]*mTranspose._11 + fTemp[7]*mTranspose._13 + fTemp[8] *mTranspose._14;
			mResult._22			-= fTemp[1]*mTranspose._11 + fTemp[6]*mTranspose._13 + fTemp[9] *mTranspose._14;
			mResult._23			 = fTemp[3]*mTranspose._11 + fTemp[6]*mTranspose._12 + fTemp[11]*mTranspose._14;
			mResult._23			-= fTemp[2]*mTranspose._11 + fTemp[7]*mTranspose._12 + fTemp[10]*mTranspose._14;
			mResult._24			 = fTemp[4]*mTranspose._11 + fTemp[9]*mTranspose._12 + fTemp[10]*mTranspose._13;
			mResult._24			-= fTemp[5]*mTranspose._11 + fTemp[8]*mTranspose._12 + fTemp[11]*mTranspose._13;

			fTemp[ 0]			= mTranspose._13 * mTranspose._24;
			fTemp[ 1]			= mTranspose._14 * mTranspose._23;
			fTemp[ 2]			= mTranspose._12 * mTranspose._24;
			fTemp[ 3]			= mTranspose._14 * mTranspose._22;
			fTemp[ 4]			= mTranspose._12 * mTranspose._23;
			fTemp[ 5]			= mTranspose._13 * mTranspose._22;
			fTemp[ 6]			= mTranspose._11 * mTranspose._24;
			fTemp[ 7]			= mTranspose._14 * mTranspose._21;
			fTemp[ 8]			= mTranspose._11 * mTranspose._23;
			fTemp[ 9]			= mTranspose._13 * mTranspose._21;
			fTemp[10]			= mTranspose._11 * mTranspose._22;
			fTemp[11]			= mTranspose._12 * mTranspose._21;

			mResult._31			 = fTemp[0] *mTranspose._42 + fTemp[3] *mTranspose._43 + fTemp[4] *mTranspose._44;
			mResult._31			-= fTemp[1] *mTranspose._42 + fTemp[2] *mTranspose._43 + fTemp[5] *mTranspose._44;
			mResult._32			 = fTemp[1] *mTranspose._41 + fTemp[6] *mTranspose._43 + fTemp[9] *mTranspose._44;
			mResult._32			-= fTemp[0] *mTranspose._41 + fTemp[7] *mTranspose._43 + fTemp[8] *mTranspose._44;
			mResult._33			 = fTemp[2] *mTranspose._41 + fTemp[7] *mTranspose._42 + fTemp[10]*mTranspose._44;
			mResult._33			-= fTemp[3] *mTranspose._41 + fTemp[6] *mTranspose._42 + fTemp[11]*mTranspose._44;
			mResult._34			 = fTemp[5] *mTranspose._41 + fTemp[8] *mTranspose._42 + fTemp[11]*mTranspose._43;
			mResult._34			-= fTemp[4] *mTranspose._41 + fTemp[9] *mTranspose._42 + fTemp[10]*mTranspose._43;
			mResult._41			 = fTemp[2] *mTranspose._33 + fTemp[5] *mTranspose._34 + fTemp[1] *mTranspose._32;
			mResult._41			-= fTemp[4] *mTranspose._34 + fTemp[0] *mTranspose._32 + fTemp[3] *mTranspose._33;
			mResult._42			 = fTemp[8] *mTranspose._34 + fTemp[0] *mTranspose._31 + fTemp[7] *mTranspose._33;
			mResult._42			-= fTemp[6] *mTranspose._33 + fTemp[9] *mTranspose._34 + fTemp[1] *mTranspose._31;
			mResult._43			 = fTemp[6] *mTranspose._32 + fTemp[11]*mTranspose._34 + fTemp[3] *mTranspose._31;
			mResult._43			-= fTemp[10]*mTranspose._34 + fTemp[2] *mTranspose._31 + fTemp[7] *mTranspose._32;
			mResult._44			 = fTemp[10]*mTranspose._33 + fTemp[4] *mTranspose._31 + fTemp[9] *mTranspose._32;
			mResult._44			-= fTemp[8] *mTranspose._32 + fTemp[11]*mTranspose._33 + fTemp[5] *mTranspose._31;

			double					fDet							=	mTranspose._11*mResult._11
				+														mTranspose._12*mResult._12
				+														mTranspose._13*mResult._13
				+														mTranspose._14*mResult._14
				;

			fDet				= 1.0/fDet;

			mResult._11			= (_tBase)(mResult._11 * fDet);
			mResult._12			= (_tBase)(mResult._12 * fDet);
			mResult._13			= (_tBase)(mResult._13 * fDet);
			mResult._14			= (_tBase)(mResult._14 * fDet);

			mResult._21			= (_tBase)(mResult._21 * fDet);
			mResult._22			= (_tBase)(mResult._22 * fDet);
			mResult._23			= (_tBase)(mResult._23 * fDet);
			mResult._24			= (_tBase)(mResult._24 * fDet);

			mResult._31			= (_tBase)(mResult._31 * fDet);
			mResult._32			= (_tBase)(mResult._32 * fDet);
			mResult._33			= (_tBase)(mResult._33 * fDet);
			mResult._34			= (_tBase)(mResult._34 * fDet);

			mResult._41			= (_tBase)(mResult._41 * fDet);
			mResult._42			= (_tBase)(mResult._42 * fDet);
			mResult._43			= (_tBase)(mResult._43 * fDet);
			mResult._44			= (_tBase)(mResult._44 * fDet);

			return mResult;
		}

		double				GetDeterminant				()			const				{
			_tMat4					mTranspose					= GetTranspose()
				,					mThis						= *this
				;
			_tBase					fTemp[12];

			fTemp[ 0]			 = mTranspose._33 * mTranspose._44;
			fTemp[ 1]			 = mTranspose._34 * mTranspose._43;
			fTemp[ 2]			 = mTranspose._32 * mTranspose._44;
			fTemp[ 3]			 = mTranspose._34 * mTranspose._42;
			fTemp[ 4]			 = mTranspose._32 * mTranspose._43;
			fTemp[ 5]			 = mTranspose._33 * mTranspose._42;
			fTemp[ 6]			 = mTranspose._31 * mTranspose._44;
			fTemp[ 7]			 = mTranspose._34 * mTranspose._41;
			fTemp[ 8]			 = mTranspose._31 * mTranspose._43;
			fTemp[ 9]			 = mTranspose._33 * mTranspose._41;
			fTemp[10]			 = mTranspose._31 * mTranspose._42;
			fTemp[11]			 = mTranspose._32 * mTranspose._41;

			mThis._11			 = fTemp[0]*mTranspose._22 + fTemp[3]*mTranspose._23 + fTemp[4] *mTranspose._24;
			mThis._11			-= fTemp[1]*mTranspose._22 + fTemp[2]*mTranspose._23 + fTemp[5] *mTranspose._24;
			mThis._12			 = fTemp[1]*mTranspose._21 + fTemp[6]*mTranspose._23 + fTemp[9] *mTranspose._24;
			mThis._12			-= fTemp[0]*mTranspose._21 + fTemp[7]*mTranspose._23 + fTemp[8] *mTranspose._24;
			mThis._13			 = fTemp[2]*mTranspose._21 + fTemp[7]*mTranspose._22 + fTemp[10]*mTranspose._24;
			mThis._13			-= fTemp[3]*mTranspose._21 + fTemp[6]*mTranspose._22 + fTemp[11]*mTranspose._24;
			mThis._14			 = fTemp[5]*mTranspose._21 + fTemp[8]*mTranspose._22 + fTemp[11]*mTranspose._23;
			mThis._14			-= fTemp[4]*mTranspose._21 + fTemp[9]*mTranspose._22 + fTemp[10]*mTranspose._23;
			mThis._21			 = fTemp[1]*mTranspose._12 + fTemp[2]*mTranspose._13 + fTemp[5] *mTranspose._14;
			mThis._21			-= fTemp[0]*mTranspose._12 + fTemp[3]*mTranspose._13 + fTemp[4] *mTranspose._14;
			mThis._22			 = fTemp[0]*mTranspose._11 + fTemp[7]*mTranspose._13 + fTemp[8] *mTranspose._14;
			mThis._22			-= fTemp[1]*mTranspose._11 + fTemp[6]*mTranspose._13 + fTemp[9] *mTranspose._14;
			mThis._23			 = fTemp[3]*mTranspose._11 + fTemp[6]*mTranspose._12 + fTemp[11]*mTranspose._14;
			mThis._23			-= fTemp[2]*mTranspose._11 + fTemp[7]*mTranspose._12 + fTemp[10]*mTranspose._14;
			mThis._24			 = fTemp[4]*mTranspose._11 + fTemp[9]*mTranspose._12 + fTemp[10]*mTranspose._13;
			mThis._24			-= fTemp[5]*mTranspose._11 + fTemp[8]*mTranspose._12 + fTemp[11]*mTranspose._13;

			fTemp[ 0]			 = mTranspose._13 * mTranspose._24;
			fTemp[ 1]			 = mTranspose._14 * mTranspose._23;
			fTemp[ 2]			 = mTranspose._12 * mTranspose._24;
			fTemp[ 3]			 = mTranspose._14 * mTranspose._22;
			fTemp[ 4]			 = mTranspose._12 * mTranspose._23;
			fTemp[ 5]			 = mTranspose._13 * mTranspose._22;
			fTemp[ 6]			 = mTranspose._11 * mTranspose._24;
			fTemp[ 7]			 = mTranspose._14 * mTranspose._21;
			fTemp[ 8]			 = mTranspose._11 * mTranspose._23;
			fTemp[ 9]			 = mTranspose._13 * mTranspose._21;
			fTemp[10]			 = mTranspose._11 * mTranspose._22;
			fTemp[11]			 = mTranspose._12 * mTranspose._21;

			mThis._31			 = fTemp[0] *mTranspose._42 + fTemp[3] *mTranspose._43 + fTemp[4] *mTranspose._44;
			mThis._31			-= fTemp[1] *mTranspose._42 + fTemp[2] *mTranspose._43 + fTemp[5] *mTranspose._44;
			mThis._32			 = fTemp[1] *mTranspose._41 + fTemp[6] *mTranspose._43 + fTemp[9] *mTranspose._44;
			mThis._32			-= fTemp[0] *mTranspose._41 + fTemp[7] *mTranspose._43 + fTemp[8] *mTranspose._44;
			mThis._33			 = fTemp[2] *mTranspose._41 + fTemp[7] *mTranspose._42 + fTemp[10]*mTranspose._44;
			mThis._33			-= fTemp[3] *mTranspose._41 + fTemp[6] *mTranspose._42 + fTemp[11]*mTranspose._44;
			mThis._34			 = fTemp[5] *mTranspose._41 + fTemp[8] *mTranspose._42 + fTemp[11]*mTranspose._43;
			mThis._34			-= fTemp[4] *mTranspose._41 + fTemp[9] *mTranspose._42 + fTemp[10]*mTranspose._43;
			mThis._41			 = fTemp[2] *mTranspose._33 + fTemp[5] *mTranspose._34 + fTemp[1] *mTranspose._32;
			mThis._41			-= fTemp[4] *mTranspose._34 + fTemp[0] *mTranspose._32 + fTemp[3] *mTranspose._33;
			mThis._42			 = fTemp[8] *mTranspose._34 + fTemp[0] *mTranspose._31 + fTemp[7] *mTranspose._33;
			mThis._42			-= fTemp[6] *mTranspose._33 + fTemp[9] *mTranspose._34 + fTemp[1] *mTranspose._31;
			mThis._43			 = fTemp[6] *mTranspose._32 + fTemp[11]*mTranspose._34 + fTemp[3] *mTranspose._31;
			mThis._43			-= fTemp[10]*mTranspose._34 + fTemp[2] *mTranspose._31 + fTemp[7] *mTranspose._32;
			mThis._44			 = fTemp[10]*mTranspose._33 + fTemp[4] *mTranspose._31 + fTemp[9] *mTranspose._32;
			mThis._44			-= fTemp[8] *mTranspose._32 + fTemp[11]*mTranspose._33 + fTemp[5] *mTranspose._31;

			double					fDet						=	mTranspose._11*mThis._11
				+													mTranspose._12*mThis._12
				+													mTranspose._13*mThis._13
				+													mTranspose._14*mThis._14
				;

			return fDet;
		}

		_tMat4&				FromRotationDir(const ::gpk::n3f32 & direction, const ::gpk::n3f32 & up = {0,1,0}) {
			::gpk::n3f32 xaxis = up.Cross(direction);
			xaxis.Normalize();

			::gpk::n3f32 yaxis = direction.Cross(xaxis);
			yaxis.Normalize();

			_11 = xaxis.x;
			_21 = yaxis.x;
			_31 = direction.x;
			_12 = xaxis.y;
			_22 = yaxis.y;
			_32 = direction.y;
			_13 = xaxis.z;
			_23 = yaxis.z;
			_33 = direction.z;

			_41 = _42 = _43 = _14 = _24 = _34		= (_tBase)0;
			_44										= (_tBase)1;

			return *this;
		}
	};	// struct
	tplt<tpnm _tElement>
	::gpk::tri3<_tElement> &	transform			(::gpk::tri3<_tElement> & triangle, const ::gpk::m4<_tElement> & transform)									{
		triangle.A					= transform.Transform(triangle.A);
		triangle.B					= transform.Transform(triangle.B);
		triangle.C					= transform.Transform(triangle.C);
		return triangle;
	}
	tplt<tpnm _tElement>
	::gpk::tri3<_tElement> &	transformDirection	(::gpk::tri3<_tElement> & triangle, const ::gpk::m4<_tElement> & transform)									{
		triangle.A					= transform.TransformDirection(triangle.A);
		triangle.B					= transform.TransformDirection(triangle.B);
		triangle.C					= transform.TransformDirection(triangle.C);
		return triangle;
	}

	tplt<tpnm _tBase>
	struct m3 {
		typedef				m3<_tBase>	_tMat3;
		typedef				n3<_tBase>		_TCoord3D;

							_tBase				_11, _12, _13
								,				_21, _22, _23
								,				_31, _32, _33
								;

		constexpr			bool				operator ==					(const _tMat3& other)															const	noexcept	{ return _11 == other._11 && _12 == other._12 && _13 == other._13 && _21 == other._21 && _22 == other._22 && _23 == other._23 && _31 == other._31 && _32 == other._32 && _33 == other._33; }
		constexpr inline	bool				operator !=					(const _tMat3& other)															const	noexcept	{ return !operator==(other); }

		constexpr			_tMat3				operator +					(const _tMat3& other)															const	noexcept	{ return {_11 + other._11, _12 + other._12, _13 + other._13, _21 + other._21, _22 + other._22, _23 + other._23, _31 + other._31, _32 + other._32, _33 + other._33}; }
		constexpr			_tMat3				operator -					(const _tMat3& other)															const	noexcept	{ return {_11 - other._11, _12 - other._12, _13 - other._13, _21 - other._21, _22 - other._22, _23 - other._23, _31 - other._31, _32 - other._32, _33 - other._33}; }
		constexpr			_tMat3				operator *					(double scalar)																	const	noexcept	{ return {(_tBase)(_11 * scalar), (_tBase)(_12 * scalar), (_tBase)(_13 * scalar), (_tBase)(_21 * scalar), (_tBase)(_22 * scalar), (_tBase)(_23 * scalar), (_tBase)(_31 * scalar), (_tBase)(_32 * scalar), (_tBase)(_33 * scalar)}; }
							_tMat3				operator /					(double scalar)																	const				{ return {(_tBase)(_11 / scalar), (_tBase)(_12 / scalar), (_tBase)(_13 / scalar), (_tBase)(_21 / scalar), (_tBase)(_22 / scalar), (_tBase)(_23 / scalar), (_tBase)(_31 / scalar), (_tBase)(_32 / scalar), (_tBase)(_33 / scalar)}; }
		constexpr			_tMat3				operator *					(const _tMat3& right)															const	noexcept	{
			return
				{ _11*right._11 + _12*right._21 + _13*right._31, _11*right._12 + _12*right._22 + _13*right._32, _11*right._13 + _12*right._23 + _13*right._33
				, _21*right._11 + _22*right._21 + _23*right._31, _21*right._12 + _22*right._22 + _23*right._32, _21*right._13 + _22*right._23 + _23*right._33
				, _31*right._11 + _32*right._21 + _33*right._31, _31*right._12 + _32*right._22 + _33*right._32, _31*right._13 + _32*right._23 + _33*right._33
				};
		}

		inline				_tMat3&				operator +=					(const _tMat3& other)																	noexcept	{ return *this = operator+(other	);	}
		inline				_tMat3&				operator -=					(const _tMat3& other)																	noexcept	{ return *this = operator-(other	);	}
		inline				_tMat3&				operator *=					(double scalar)																			noexcept	{ return *this = operator*(scalar	);	}
		inline				_tMat3&				operator /=					(double scalar)			{ return *this = operator/(scalar	);	}
		inline				_tMat3&				operator *=					(const _tMat3& right)																	noexcept	{ return *this = operator*(right	);	}

							void				SetTranspose				(const _tMat3& m)																		noexcept	{ *this = m.GetTranspose();				}
							void				SetInverse					(const _tMat3& m)				{ *this = m.GetInverse();				}
							void				Transpose					(const _tMat3& m)																		noexcept	{ *this = GetTranspose();				}
							void				Invert						()						{ *this = GetInverse();					}

		inline				_tMat3&				LinearInterpolate			(const _tMat3&p, const _tMat3&q, double fTime)											noexcept	{ return *this = ((q-p)*fTime)+p; }
		constexpr			double				GetDeterminant				()			const	noexcept	{ return _11*(_22*_33-_23*_32) + _12*(_23*_31-_33*_21) + _13*(_21*_32-_22*_31); }
		constexpr			_tMat3				GetTranspose				()			const	noexcept	{ return {_11, _21, _31, _12, _22, _32, _13, _23, _33};	}
							_tMat3				GetInverse					()			const				{
			_tBase										A, B, C, D, E, F, G, H, I;

			A										= (_22*_33 - _23*_32);/**/ B = (_23*_31 - _21*_33);/**/ C = (_21*_32 - _22*_31);
			D										= (_13*_32 - _12*_33);/**/ E = (_11*_33 - _13*_31);/**/ F = (_12*_31 - _11*_32);
			G										= (_12*_23 - _13*_22);/**/ H = (_13*_21 - _11*_23);/**/ I = (_11*_22 - _12*_21);

			const double								det
				=	_11*(_22*_33-_23*_32)
				+	_12*(_23*_31-_33*_21)
				+	_13*(_21*_32-_22*_31)
				;

			_tMat3										result;
			if(0 == det) {
				result									=
					{	1, 0, 0
					,	0, 1, 0
					,	0, 0, 1
					};
			}
			else {
				result									=
					{	(_tBase)(A/det), (_tBase)(D/det), (_tBase)(G/det)
					,	(_tBase)(B/det), (_tBase)(E/det), (_tBase)(H/det)
					,	(_tBase)(C/det), (_tBase)(F/det), (_tBase)(I/det)
					};
			}

			return result;
		}


		constexpr			_TCoord3D			Transform					(const _TCoord3D& vector)														const	noexcept	{
			return
				{	vector.x * _11 + vector.y * _21 + vector.z * _31
				,	vector.x * _12 + vector.y * _22 + vector.z * _32
				,	vector.x * _13 + vector.y * _23 + vector.z * _33
				};
		}

		constexpr			_TCoord3D			TransformInverse			(const _TCoord3D& _v)															const	noexcept	{
			return
				{	_v.x * _11 + _v.y * _12 + _v.z * _13
				,	_v.x * _21 + _v.y * _22 + _v.z * _23
				,	_v.x * _31 + _v.y * _32 + _v.z * _33
				};
		}
							void				Identity					()			noexcept	{
			*this =
				{	(_tBase)1,  (_tBase)0,  (_tBase)0
				,	(_tBase)0,  (_tBase)1,  (_tBase)0
				,	(_tBase)0,  (_tBase)0,  (_tBase)1
				};
		}
							void				RotationX					(double angle)																			noexcept	{
			::gpk::SSinCos							angleSinCos					= {sin(angle), cos(angle)};
			_11 = (_tBase)1;	_12 =							_13 =
			_31 = (_tBase)0;	_32 = -(_tBase)angleSinCos.Sin;	_33 = (_tBase)angleSinCos.Cos;
			_21 = (_tBase)0;	_22 =  (_tBase)angleSinCos.Cos;	_23 = (_tBase)angleSinCos.Sin;
		}
							void				RotationY					(double angle)																			noexcept	{
			::gpk::SSinCos							angleSinCos					= ::gpk::getSinCos(angle);
			_11 = (_tBase)angleSinCos.Cos;	_12 = (_tBase)0;	_13 = -(_tBase)angleSinCos.Sin;
			_21 = (_tBase)0;				_22 = (_tBase)1;	_23 =  (_tBase)0;
			_31 = (_tBase)angleSinCos.Sin;	_32 = (_tBase)0;	_33 =  (_tBase)angleSinCos.Cos;
		}
							void				RotationZ					(double angle)																			noexcept	{
			::gpk::SSinCos							angleSinCos					= ::gpk::getSinCos(angle);
			_11 =  (_tBase)angleSinCos.Cos;	_12 = (_tBase)angleSinCos.Sin;	_13 = (_tBase)0;
			_21 = -(_tBase)angleSinCos.Sin;	_22 = (_tBase)angleSinCos.Cos;	_23 =
			_31 =							_32 = (_tBase)0;				_33 = (_tBase)1;
		}
		inline				void				Scale						(_tBase x, _tBase y, _tBase z, bool bEraseContent)										noexcept	{ Scale({x, y, z}, bEraseContent); }
							void				Scale		(const _TCoord3D& ypr, bool bEraseContent)												noexcept	{
			if( bEraseContent ) {
				_11 = (_tBase)ypr.x;		_12 =					_13 =
				_21 = (_tBase)0;			_22 = (_tBase)ypr.y;	_23 =
				_31 =						_32 = (_tBase)0;		_33 = (_tBase)ypr.z;
			}
			else {
				_11 = (_tBase)(_11*ypr.x); _22 = (_tBase)(_22*ypr.y); _33 = (_tBase)(_33*ypr.z);
			}
		}
		inline				void				Rotation					(_tBase x, _tBase y, _tBase z)															noexcept	{ return Rotation({x, y, z}); }
							void				Rotation					(const _TCoord3D &vc)																	noexcept	{
			::gpk::SSinCos							yaw							= ::gpk::getSinCos(vc.z);
			::gpk::SSinCos							pitch						= ::gpk::getSinCos(vc.y);
			::gpk::SSinCos							roll						= ::gpk::getSinCos(vc.x);

			_11										= (_tBase)(pitch.Cos * yaw.Cos									);
			_12										= (_tBase)(pitch.Cos * yaw.Sin									);
			_13										= (_tBase)(-pitch.Sin											);

			_21										= (_tBase)(roll.Sin * pitch.Sin * yaw.Cos + roll.Cos *-yaw.Sin	);
			_22										= (_tBase)(roll.Sin * pitch.Sin * yaw.Sin + roll.Cos * yaw.Cos	);
			_23										= (_tBase)(roll.Sin * pitch.Cos  		 	 					);

			_31										= (_tBase)(roll.Cos * pitch.Sin * yaw.Cos +-roll.Sin *-yaw.Sin	);
			_32										= (_tBase)(roll.Cos * pitch.Sin * yaw.Sin +-roll.Sin * yaw.Cos	);
			_33										= (_tBase)(roll.Cos * pitch.Cos									);
		} // Rota
							void				RotationArbitraryAxis		(const _TCoord3D& _vcAxis, _tBase a)																{
			::gpk::SSinCos							pairSinCos					= ::gpk::getSinCos(a);
			_TCoord3D									vcAxis						= _vcAxis;
			double										fSum						= 1.0 - pairSinCos.Cos;

			if( vcAxis.LengthSquared() != 1.0 )
				vcAxis.Normalize();

			_11										= (_tBase)(	(vcAxis.x * vcAxis.x) * fSum + pairSinCos.Cos				);
			_12										= (_tBase)(	(vcAxis.x * vcAxis.y) * fSum - (vcAxis.z * pairSinCos.Sin)	);
			_13										= (_tBase)(	(vcAxis.x * vcAxis.z) * fSum + (vcAxis.y * pairSinCos.Sin)	);

			_21										= (_tBase)(	(vcAxis.y * vcAxis.x) * fSum + (vcAxis.z * pairSinCos.Sin)	);
			_22										= (_tBase)(	(vcAxis.y * vcAxis.y) * fSum + pairSinCos.Cos 				);
			_23										= (_tBase)(	(vcAxis.y * vcAxis.z) * fSum - (vcAxis.x * pairSinCos.Sin)	);

			_31										= (_tBase)(	(vcAxis.z * vcAxis.x) * fSum - (vcAxis.y * pairSinCos.Sin)	);
			_32										= (_tBase)(	(vcAxis.z * vcAxis.y) * fSum + (vcAxis.x * pairSinCos.Sin)	);
			_33										= (_tBase)(	(vcAxis.z * vcAxis.z) * fSum + pairSinCos.Cos				);
		}

							void				SetOrientation				(const ::gpk::quat<_tBase>& qo)														noexcept	{
			double										wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

			x2										= qo.x + qo.x;
			y2										= qo.y + qo.y;
			z2										= qo.z + qo.z;

			xx										= qo.x * x2;
			xy										= qo.x * y2;
			xz										= qo.x * z2;

			yy										= qo.y * y2;
			yz										= qo.y * z2;
			zz										= qo.z * z2;

			wx										= qo.w * x2;
			wy										= qo.w * y2;
			wz										= qo.w * z2;

			_11										= (_tBase)(1.0 - (yy + zz)	);
			_21										= (_tBase)(xy - wz			);
			_31										= (_tBase)(xz + wy			);

			_12										= (_tBase)(xy + wz			);
			_22										= (_tBase)(1.0 - (xx + zz)	);
			_32										= (_tBase)(yz - wx			);

			_13										= (_tBase)(xz - wy			);
			_23										= (_tBase)(yz + wx			);
			_33										= (_tBase)(1.0 - (xx + yy)	);
		}

		 // Sets the value of the matrix from inertia tensor values.
		 //
							void				SetCoeffsAngularMass		(double ix, double iy, double iz, double ixy, double ixz, double iyz)					noexcept	{
			_11 =		(_tBase) ix		;
			_21 = _12 =	(_tBase)-ixy	;
			_31 = _13 =	(_tBase)-ixz	;
			_22 =		(_tBase) iy		;
			_32 = _23 =	(_tBase)-iyz	;
			_33 =		(_tBase) iz		;
		}

		// Sets the value of the matrix as an inertia tensor of a rectangular block aligned with the body's coordinate system with the given axis half-sizes and mass.
							void				SetBlockAngularMass			(const _TCoord3D &halfSizes, double mass)												noexcept	{
			_TCoord3D									squares						= halfSizes;
			squares.x								*= halfSizes.x; squares.y *= halfSizes.y; squares.z *= halfSizes.z;
			SetCoeffsAngularMass
				( 0.3f * mass * (squares.y + squares.z)
				, 0.3f * mass * (squares.x + squares.z)
				, 0.3f * mass * (squares.x + squares.y)
				);
		}
	};
	typedef m3<sc_t>		m3char;
	typedef m3<uc_t>		m3uchar;
	typedef m3<float>		m3f32;
	typedef m3<double>		m3f64;
	typedef m3<uint8_t>		m3u8;
	typedef m3<uint16_t>	m3u16;
	typedef m3<uint32_t>	m3u32;
	typedef m3<uint64_t>	m3u64;
	typedef m3<int8_t>		m3i8;
	typedef m3<int16_t>		m3i16;
	typedef m3<int32_t>		m3i32;
	typedef m3<int64_t>		m3i64;

	typedef m4<sc_t>		m4char;
	typedef m4<uc_t>		m4uchar;
	typedef m4<float>		m4f32;
	typedef m4<double>		m4f64;
	typedef m4<uint8_t>		m4u8;
	typedef m4<uint16_t>	m4u16;
	typedef m4<uint32_t>	m4u32;
	typedef m4<uint64_t>	m4u64;
	typedef m4<int8_t>		m4i8;
	typedef m4<int16_t>		m4i16;
	typedef m4<int32_t>		m4i32;
	typedef m4<int64_t>		m4i64;

#pragma pack(pop)
}

#endif // GPK_MATRIX_H_23627
