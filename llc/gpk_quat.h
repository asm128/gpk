#include "gpk_n3.h"

#ifndef GPK_QUAT_H_230515
#define GPK_QUAT_H_230515

namespace gpk 
{
	template<typename _tBase>
	struct quat {
		_tBase					x, y, z, w;

		typedef	quat<_tBase>	Tq;
		typedef	n3  <_tBase>	Tn3;

		cnstxpr	bool			operator ==				(const Tq & other)					const	noexcept	{ return x == other.x && y == other.y && z == other.z && w == other.w; }
		inlcxpr	bool			operator !=				(const Tq & other)					const	noexcept	{ return !operator==(other); }
		inlcxpr	Tq				operator-				()									const	noexcept	{ return {x*-1, y*-1, z*-1, w * -1};	}
		inlcxpr	Tq				operator~				()									const	noexcept	{ return {-x, -y, -z, w};			}
		cnstxpr	Tq				operator +				(const Tq & other)					const	noexcept	{ return { x + other.x, y + other.y, z + other.z, w + other.w }; }
		cnstxpr	Tq				operator -				(const Tq & other)					const	noexcept	{ return { x - other.x, y - other.y, z - other.z, w - other.w }; }
		cnstxpr	Tq				operator *				(double scalar)						const	noexcept	{ return { (_tBase)(x * scalar), (_tBase)(y * scalar), (_tBase)(z * scalar), (_tBase)(w * scalar) }; }
		cnstxpr	Tq				operator /				(double scalar)						const				{ return { (_tBase)(x / scalar), (_tBase)(y / scalar), (_tBase)(z / scalar), (_tBase)(w / scalar) }; }
		Tq						operator *				(const Tq & q)						const	noexcept	{
			Tq							r;
			r.x			= w * q.x + x*q.w + y*q.z - z*q.y;
			r.y			= w * q.y + y*q.w + z*q.x - x*q.z;
			r.z			= w * q.z + z*q.w + x*q.y - y*q.x;
			r.w			= w * q.w - x*q.x - y*q.y - z*q.z;

			return r;
		}
		Tq						operator *				(const Tn3 & v)						const	noexcept	{
			return
				{	  w * v.x + y*v.z - z*v.y
				,	  w * v.y + z*v.x - x*v.z
				,	  w * v.z + x*v.y - y*v.x
				,	-(x*v.x + y*v.y + z*v.z)
				};
		}
		inline	Tq&				operator*=				(const Tq & other)							noexcept	{ return *this = operator*(other); }
		Tq&						operator+=				(const Tq & other)							noexcept	{ x += other.x; y += other.y; z += other.z; w += other.w; return *this; }
		Tq&						operator-=				(const Tq & other)							noexcept	{ x -= other.x; y -= other.y; z -= other.z; w -= other.w; return *this; }
		Tq&						operator*=				(double scalar)								noexcept	{ x = (_tBase)(x * scalar); y = (_tBase)(y * scalar); z = (_tBase)(z * scalar); w = (_tBase)(w * scalar); return *this;	}
		Tq&						operator/=				(double scalar)											{ x = (_tBase)(x / scalar); y = (_tBase)(y / scalar); z = (_tBase)(z / scalar); w = (_tBase)(w / scalar); return *this;	}

		template<typename _t>
		inlcxpr	quat<_t>		Cast					()									const	noexcept	{ return {(_t)x, (_t)y, (_t)z, (_t)w};	}
		cnstxpr	_tBase			LengthSquared			()									const	noexcept	{ return x * x + y * y + z * z + w * w;	}
		inlcxpr	double			Length					()									const				{ const _tBase sqLen = LengthSquared(); return (sqLen) ? ::sqrt(sqLen) : 0;	}
		cnstxpr	double			Dot						(const Tq & other)					const	noexcept	{ return x*other.x + y*other.y + z*other.z + w * other.w;	}
		inline	Tq&				Identity				()											noexcept	{ return *this = {0, 0, 0, 1};			}
		inline	Tq&				Normalize				()											noexcept	{ _tBase sqLen = LengthSquared(); return sqLen ? *this /= ::sqrt(sqLen) : *this; }
		inline	Tq				Normalized				()									const	noexcept	{ _tBase sqLen = LengthSquared(); return sqLen ? *this /  ::sqrt(sqLen) : *this; }
		inline	Tq&				LinearInterpolate		(const Tq & p, const Tq & q, double fTime)	noexcept	{ return *this = ((q-p)*fTime)+p;						}
		inline	Tq&				SetRotation				(const Tq & q, const Tq & p)				noexcept	{ return *this = q * p * ~q;																							}
		inline	Tq&				MakeFromEulerTaitBryan	(const Tn3 & v)											{ return MakeFromEulerTaitBryan(v.x, v.y, v.z);																			}
		//void					AddScaled				(const TCoord4& vector, double scale)		noexcept	{ Tq q = {(_tBase)(vector.x * scale), (_tBase)(vector.y * scale), (_tBase)(vector.z * scale), (_tBase)0}; q *= *this; w += (_tBase)(q.w * 0.5); x += (_tBase)(q.x * 0.5); y += (_tBase)(q.y * 0.5); z += (_tBase)(q.z * 0.5); }
		Tq&						AddScaled				(const Tn3 & vector, double scale)			noexcept	{ Tq q = {(_tBase)(vector.x * scale), (_tBase)(vector.y * scale), (_tBase)(vector.z * scale), (_tBase)0}; q *= *this; w += (_tBase)(q.w * 0.5); x += (_tBase)(q.x * 0.5); y += (_tBase)(q.y * 0.5); z += (_tBase)(q.z * 0.5); return *this; }
		Tn3						RotateVector			(const Tn3 & v)						const	noexcept	{
			const Tq					t						= {x, y, z, w};
			const Tq					r						= t * v * (~t);
			return {r.x, r.y, r.z};
		}
		Tq&						CreateFromAxisAngle		(const ::gpk::n3f32 & axis, double angle)				{
			const double				halfAngle				= angle * .5;
			const double				s						= ::gpk::sin(halfAngle);
			x						= (_tBase)(axis.x * s);
			y						= (_tBase)(axis.y * s);
			z						= (_tBase)(axis.z * s);
			w						= (_tBase)::gpk::cos(halfAngle);
			return *this;
		}
		/// Evaluates a rotation needed to be applied to an object positioned at sourcePoint to face destPoint
		Tq&						LookAt					(const ::gpk::n3f32 & sourcePoint, const ::gpk::n3f32 & destPoint, const ::gpk::n3f32 & up = {0, 1, 0}, const ::gpk::n3f32 & front = {1, 0, 0})	{
			::gpk::n3f32				forwardVector			= (destPoint - sourcePoint).Normalize();
			const double				dot						= front.Dot(forwardVector);
			if (::gpk::abs(dot - (-1.0)) < 0.000001)
				return *this = Tq{up.x, up.y, up.z, -(_tBase)::gpk::math_pi}.Normalize();
			if (::gpk::abs(dot - (1.0)) < 0.000001)
				return *this = {0, 0, 0, 1};

			const double				rotAngle				= ::gpk::acos(dot);
			::gpk::n3f32				rotAxis					= front.Cross(forwardVector);
			rotAxis.Normalize();
			return CreateFromAxisAngle(rotAxis, rotAngle);
		}

		Tq&						SLERP					(const Tq & p, const Tq & q, double fTime)				{
			//Calculate the dot product
			double						fDot					= Dot(q);

			Tq							q_;
			if(fDot < 0.0f) {
				q_						= -q;
				fDot					= -fDot;
			}

			if(fDot < 1.00001f && fDot > 0.99999f)
				return ::gpk::interpolate_linear(p, q_, fTime);

			// calculate the angle between the quaternions
			const double				fTheta					= acos(fDot);
			return *this = (p * sin(fTheta * (1 - fTime)) + q_ * sin(fTheta * fTime)) / sin(fTheta);
		}
		// Convert from Euler Angles
		Tq&						MakeFromEulerTaitBryan	(double fPitch, double fYaw, double fRoll)				{
			 //Basically we create 3 Quaternions, one for pitch, one for yaw, one for roll and multiply those together. the calculation below does the same, just shorter.
			fPitch					*= 0.5f;//
			fYaw					*= 0.5f;//
			fRoll					*= 0.5f;//

			const SSinCos				pairSinCosX				= ::gpk::getSinCos(fPitch	);
			const SSinCos				pairSinCosY				= ::gpk::getSinCos(fYaw	);
			const SSinCos				pairSinCosZ				= ::gpk::getSinCos(fRoll	);

			const double				cYcZ					= pairSinCosY.Cos * pairSinCosZ.Cos;
			const double				sYsZ					= pairSinCosY.Sin * pairSinCosZ.Sin;
			const double				cYsZ					= pairSinCosY.Cos * pairSinCosZ.Sin;
			const double				sYcZ					= pairSinCosY.Sin * pairSinCosZ.Cos;

			w						= (_tBase)(pairSinCosX.Cos * cYcZ + pairSinCosX.Sin * sYsZ);
			x						= (_tBase)(pairSinCosX.Sin * cYcZ - pairSinCosX.Cos * sYsZ);
			y						= (_tBase)(pairSinCosX.Cos * sYcZ + pairSinCosX.Sin * cYsZ);
			z						= (_tBase)(pairSinCosX.Cos * cYsZ - pairSinCosX.Sin * sYcZ);

			return Normalize();
		}
		void					GetEulersTaitBryan		(double* fPitch, double* fYaw, double* fRoll)			{
			const double				q00						= w * w;
			const double				q11						= x * x;
			const double				q22						= y * y;
			const double				q33						= z * z;

			const double				r11						= q00 + q11 - q22 - q33;
			const double				r21						= 2 * (x * y + w * z);
			const double				r31						= 2 * (x * z - w * y);
			const double				r32						= 2 * (y*z + w * x);
			const double				r33						= q00 - q11 - q22 + q33;

			const double				tmp						= abs(r31);
			if(tmp > 0.999999) {
				const double				r12						= 2 * (x * y - w * z);
				const double				r13						= 2 * (x * z + w * y);
				*fPitch					= 0.0f;
				*fYaw					= -((::gpk::math_pi_2) * r31 / tmp);
				*fRoll					= atan2(-r12, -r31*r13);
			}
			else {
				*fPitch					= atan2(r32, r33);
				*fYaw					= asin(-r31);
				*fRoll					= atan2(r21, r11);
			}
		} //
		// just in case you need that function also
	}; // struct quat

	typedef quat<float>		quatf32, quatf;
	typedef quat<double>	quatf64, quatd;
	typedef quat<uint8_t>	quatu8;
	typedef quat<uint16_t>	quatu16;
	typedef quat<uint32_t>	quatu32;
	typedef quat<uint64_t>	quatu64;
	typedef quat<int8_t>	quati8;
	typedef quat<int16_t>	quati16;
	typedef quat<int32_t>	quati32;
	typedef quat<int64_t>	quati64;


#define QUAT_F32 "{%f, %f, %f, %f}"
#define QUAT_F64 "{%g, %g, %g, %g}"
#define QUAT_I32 "{%i, %i, %i, %i}"
#define QUAT_U32 "{%u, %u, %u, %u}"
#define QUAT_I64 "{%lli, %lli, %lli, %lli}"
#define QUAT_U64 "{%llu, %llu, %llu, %llu}"

#define gpk_xyzw(quatvar) quatvar.x, quatvar.y, quatvar.z, quatvar.w

} // namespace

#endif // GPK_QUAT_H_230515
